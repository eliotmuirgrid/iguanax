const CYTOgridConst = {nodeW: 160, nodeH: 40, padX: 20, padY: 20, gapY: 60, snapX: 100, snapY: 40};

let CYTOnodePositions = {};

function CYTOsnapCoordinate(value, step, mode = 'nearest') {
   if(mode === 'up') { return Math.ceil(value / step) * step; }
   if(mode === 'down') { return Math.floor(value / step) * step; }
   return Math.round(value / step) * step;
}

function CYTOnodePositionsReset()   { CYTOnodePositions = {} }
function CYTOcommitPositions(nodes) { nodes.forEach(n => { CYTOnodePositions[n.id()] = n.position(); n.removeData('unpositioned'); }); }

function CYTOsavePosition(id, node) {
   CYTOnodePositions[id] = node.position();
}

function CYTOprocessExistingNodes(elements, newNodeIds) {
   let hasNodeChange = false;
   const addedNodes    = [];
   for(const nodeData of elements.nodes) {
      const existing = CYTOinstance.getElementById(nodeData.data.id);
      if(!existing.length) {
         hasNodeChange = true;
         addedNodes.push(nodeData);
      } else {
         if(!existing.data('unpositioned')) { CYTOsavePosition(nodeData.data.id, existing); }
         if(nodeData.position) { existing.position(nodeData.position); }
         CYTOupdateNode(existing, nodeData);
      }
   }
   const removedNodes = CYTOinstance.nodes().filter(n => !newNodeIds.has(n.id()));
   removedNodes.forEach(n => {
      if(!n.data('unpositioned')) { CYTOsavePosition(n.id(), n); }
      n.remove();
   });
   return {addedNodes, hasNodeChange: hasNodeChange || removedNodes.length > 0};
}

function CYTOapplyAddedNodes(addedNodes) {
   for(const nodeData of addedNodes) {
      if(nodeData.data.unpositioned && CYTOnodePositions[nodeData.data.id]) {
         nodeData.position = CYTOnodePositions[nodeData.data.id];
         delete nodeData.data.unpositioned;
      }
   }
   CYTOinstance.add(addedNodes);
}

function CYTOupdateElements(elements, onDone) {
   const newNodeIds                  = new Set(elements.nodes.map(n => n.data.id));
   const {addedNodes, hasNodeChange} = CYTOprocessExistingNodes(elements, newNodeIds);

   if(hasNodeChange) {
      if(addedNodes.length > 0) { CYTOapplyAddedNodes(addedNodes); }
      CYTOinstance.edges().remove();
      CYTOinstance.add(elements.edges);
   } else {
      CYTOupdateEdges(elements.edges);
   }
   CYTOapplyEditMode();
   CYTOsyncProcessingPulse();
   CYTOscheduleNodeContentSync();
   if(onDone) { onDone(); }
}

function CYTOupdateNode(existing, nodeData) {
   const previousLight = existing.data('light') || 'OFF';
   const nextLight     = nodeData.data.light || 'OFF';
   const sourceCount   = Number(nodeData.data.sourceCount || 0);
   const borderColor   = String(nodeData.data.borderColor || '').trim();
   const shouldPulseOnStart = previousLight === 'OFF' && nextLight === 'ON' && sourceCount === 0;

   existing.data('light', nodeData.data.light);
   existing.data('label', nodeData.data.label);
   existing.data('queueCount', Number(nodeData.data.queueCount || 0));
   existing.data('errorCount', Number(nodeData.data.errorCount || 0));
   existing.data('sourceCount', sourceCount);
   existing.data('tags', nodeData.data.tags || []);
   existing.data('isTemplate', !!nodeData.data.isTemplate);
   existing.data('hostServerGuid', nodeData.data.hostServerGuid || '');
   existing.data('showActivityPulse', nodeData.data.showActivityPulse);
   if(borderColor) { existing.data('borderColor', borderColor); }
   else            { existing.removeData('borderColor'); }
   if(shouldPulseOnStart) { CYTOtriggerStartupPulse(existing); }
   if(nodeData.data.unpositioned) { existing.data('unpositioned', true); }
   else                           { existing.removeData('unpositioned'); }
   const classes  = nodeData.classes.split(' ');
   const current  = existing.classes();
   const toRemove = [];
   for(let i = 0; i < current.length; i++) {
      if(current[i] !== 'CYTOComponent' && current[i] !== 'CYTOMarked' && !classes.includes(current[i])) { toRemove.push(current[i]); }
   }
   for(let i = 0; i < classes.length; i++) { if(!existing.hasClass(classes[i])) { existing.addClass(classes[i]); } }
   for(let i = 0; i < toRemove.length; i++) { existing.removeClass(toRemove[i]); }
}

function CYTOupdateEdges(edges) {
   const incomingIds = new Set(edges.map(e => e.data.id));

   CYTOinstance.edges().filter(e => !incomingIds.has(e.id())).remove();

   const toAdd = edges.filter(edgeData => {
      if(CYTOinstance.getElementById(edgeData.data.id).length)      { return false; }
      if(!CYTOinstance.getElementById(edgeData.data.source).length) { return false; }
      return CYTOinstance.getElementById(edgeData.data.target).length;

   });
   if(toAdd.length > 0) { CYTOinstance.add(toAdd); }
}

// Buckets node indices by grid cell so overlap checks only run against nearby neighbors.
function CYTObuildGrid(nodeArr, cellW, cellH) {
   const grid = new Map();
   for(let i = 0; i < nodeArr.length; i++) {
      const {x, y} = nodeArr[i].position();
      const key    = `${Math.floor(x / cellW)},${Math.floor(y / cellH)}`;
      if(!grid.has(key)) { grid.set(key, []); }
      grid.get(key).push(i);
   }
   return grid;
}

function CYTOgetNeighborIndices(grid, cx, cy) {
   const indices = [];
   for(let dx = -1; dx <= 1; dx++) {
      for(let dy = -1; dy <= 1; dy++) {
         const neighbors = grid.get(`${cx + dx},${cy + dy}`);
         if(neighbors) { indices.push(...neighbors); }
      }
   }
   return indices;
}

function CYTOgetShares(pinnedI, pinnedJ) {
   let shareI, shareJ;
   if(pinnedJ) { shareI = 1; } else if(pinnedI) { shareI = 0; } else { shareI = 0.5; }
   if(pinnedI) { shareJ = 1; } else if(pinnedJ) { shareJ = 0; } else { shareJ = 0.5; }
   return {shareI, shareJ};
}

function CYTOapplyPush(a, b, nodeI, nodeJ, pushX, pushY, shareI, shareJ, overlapX, overlapY) {
   const pushAxis = Math.abs(overlapX) < Math.abs(overlapY) ? 'x' : 'y';
   if(pushAxis === 'x') {
      nodeI.position({x: a.x - pushX * shareI, y: a.y});
      nodeJ.position({x: b.x + pushX * shareJ, y: b.y});
   } else {
      nodeI.position({x: a.x, y: a.y - pushY * shareI});
      nodeJ.position({x: b.x, y: b.y + pushY * shareJ});
   }
}

function CYTOresolveOverlap(nodeArr, i, j, cellW, cellH, pinnedIds) {
   const a        = nodeArr[i].position();
   const b        = nodeArr[j].position();
   const ox       = b.x - a.x;
   const oy       = b.y - a.y;
   const overlapX = cellW - Math.abs(ox);
   const overlapY = cellH - Math.abs(oy);
   if(overlapX <= 0 || overlapY <= 0) { return false; }

   const pinnedI = pinnedIds.has(nodeArr[i].id());
   const pinnedJ = pinnedIds.has(nodeArr[j].id());
   if(pinnedI && pinnedJ) { return false; }

   const {shareI, shareJ} = CYTOgetShares(pinnedI, pinnedJ);
   const pushX            = overlapX * (ox >= 0 ? 1 : -1);
   const pushY            = overlapY * (oy >= 0 ? 1 : -1);

   CYTOapplyPush(a, b, nodeArr[i], nodeArr[j], pushX, pushY, shareI, shareJ, overlapX, overlapY);
   return true;
}

function CYTOoverlapPass(nodeArr, cellW, cellH, pinnedIds) {
   const grid = CYTObuildGrid(nodeArr, cellW, cellH);
   let moved  = false;
   CYTOinstance.batch(() => {
      for(let i = 0; i < nodeArr.length; i++) {
         const {x, y} = nodeArr[i].position();
         const cx     = Math.floor(x / cellW);
         const cy     = Math.floor(y / cellH);
         for(const j of CYTOgetNeighborIndices(grid, cx, cy)) {
            if(j <= i) { continue; }
            if(CYTOresolveOverlap(nodeArr, i, j, cellW, cellH, pinnedIds)) { moved = true; }
         }
      }
   });
   return moved;
}

// Runs up to 10 separation passes until no overlaps remain or the limit is hit.
function CYTOremoveOverlaps(nodes, pinnedIds = new Set(Object.keys(CYTOnodePositions)), {padX = CYTOgridConst.padX, padY = CYTOgridConst.padY} = {}) {
   const cellW   = CYTOgridConst.nodeW + padX;
   const cellH   = CYTOgridConst.nodeH + padY;
   const nodeArr = nodes.toArray();
   for(let p = 0; p < 10; p++) { if(!CYTOoverlapPass(nodeArr, cellW, cellH, pinnedIds)) {break;} }
}

function CYTOcalcGridDimensions(count) {
   const stepX   = Math.ceil((CYTOgridConst.nodeW + CYTOgridConst.padX) / CYTOgridConst.snapX) * CYTOgridConst.snapX;
   const stepY   = Math.ceil((CYTOgridConst.nodeH + CYTOgridConst.padY) / CYTOgridConst.snapY) * CYTOgridConst.snapY;
   const viewW   = CYTOinstance.container().clientWidth / CYTOinstance.zoom();
   const maxCols = Math.max(1, Math.floor(viewW / stepX));
   const cols    = Math.max(1, Math.min(count > 0 ? Math.ceil(Math.sqrt(count)) : 1, maxCols));
   return {cols, stepX, stepY};
}

function CYTOgridStartAboveOrigin(count) {
   const g    = CYTOcalcGridDimensions(count);
   const rows = Math.max(1, Math.ceil(count / g.cols));
   return {
      x: 0,
      y: CYTOsnapCoordinate(-(CYTOgridConst.gapY + (rows * g.stepY) - (CYTOgridConst.nodeH / 2)), CYTOgridConst.snapY, 'down')
   };
}

function CYTOplaceNodesInGrid(nodes, startX, startY) {
   const g = CYTOcalcGridDimensions(nodes.length);
   const alignedStartX = CYTOsnapCoordinate(startX, CYTOgridConst.snapX);
   const alignedStartY = CYTOsnapCoordinate(startY, CYTOgridConst.snapY);
   nodes.forEach((node, i) => {
      node.position({
         x: alignedStartX + (i % g.cols) * g.stepX,
         y: alignedStartY + Math.floor(i / g.cols) * g.stepY
      });
   });
}

function CYTOrunLayout(onDone) {
   const unpositioned = CYTOinstance.nodes().filter(n => n.data('unpositioned'));
   const singles      = unpositioned.filter(n => n.degree() === 0);
   const connected    = CYTOinstance.elements().not(singles).not(CYTOinstance.nodes().filter(n => !n.data('unpositioned')));

   function placeSingles() {
      const start = CYTOgridStartAboveOrigin(singles.length);
      CYTOplaceNodesInGrid(singles, start.x, start.y);
      CYTOcommitPositions(singles);
      CYTOinstance.fit();
      CYTOinstance.zoom(1);
      if(onDone) { onDone(); }
   }

   if(connected.length) {
      const layoutConnected = connected.layout(CYTOcosLayout());
      layoutConnected.on('layoutstop', () => {
         CYTOremoveOverlaps(connected.nodes(), new Set());
         CYTOcommitPositions(connected.nodes());
         placeSingles();
      });
      layoutConnected.run();
   } else {
      placeSingles();
   }
}

function CYTOpositionNewNodes(onDone) {
   const unpositioned = CYTOinstance.nodes().filter(n => n.data('unpositioned'));
   if(unpositioned.length === 0) { if(onDone) { onDone(); } return; }
   const start = CYTOgridStartAboveOrigin(unpositioned.length);

   CYTOplaceNodesInGrid(unpositioned, start.x, start.y);
   CYTOcommitPositions(unpositioned);
   CYTOresize();
   if(onDone) { onDone(); }
}
