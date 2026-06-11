function CYTOlisteners() {
   if(!CYTOinstance) { return; }
   CYTObindDashBridgeListeners();
   CYTObindSyncEvents();
   CYTObindNodeDragEvents();
   CYTObindSelectionEvents();
   CYTObindHoverEvents();
   CYTObindOpenEvents();
   CYTObindContextMenuEvents();
   CYTObindTapEvents();
}

function CYTObindSyncEvents() {
   CYTOinstance.on('render', () => { CYTOscheduleNodeContentSync(); });
   CYTOinstance.on('pan zoom resize', () => { CYTOscheduleNodeContentSync(CYTOinstance, {debounceMs: 60}); });
   CYTOinstance.on('pan zoom', () => { CYTOscheduleViewportSave(); });
   CYTOinstance.on('position add remove data', 'node', () => { CYTOscheduleNodeContentSync(); });
}

function CYTObindNodeDragEvents() {
   CYTOinstance.on('dragfree', 'node', (e) => { CYTOhandleNodeDragFree(e); });
   CYTOinstance.on('grab', 'node', () => { CYTOhandleNodeGrab(); });
   CYTOinstance.on('free', 'node', () => { CYTOhandleNodeFree(); });
}

function CYTObindSelectionEvents() {
   CYTOinstance.on('boxstart', () => { CYTOinstance.elements().removeClass('CYTOHighlighted CYTODimmed'); });
   CYTOinstance.on('boxselect', 'node', (e) => { e.target.addClass('CYTOMarked'); CYTOscheduleCheckedSync(); });
}

function CYTObindHoverEvents() {
   CYTOinstance.on('mouseover', 'node, edge', (e) => { CYTOaddHover(e); });
   CYTOinstance.on('mouseout', 'node, edge', () => { CYTOremoveHover(); });
}

function CYTObindOpenEvents() {
   CYTOinstance.on('dbltap', 'node', (e) => {
      CYTOcancelScheduledNodeTap();
      CYTOremoveHover();
      DASHopenComponent(e.target.id());
   });
   CYTOinstance.on('dbltap', (e) => {
      if(e.target !== CYTOinstance) { return; }
      if(CYTOinstance.connectMode) { return; }
      if(CYTOgetMarkedNodeIds().length) { return; }
      CYTOfocusComponentSearchInput();
   });
}

function CYTOfocusComponentSearchInput() {
   const input = document.querySelector('.EDITfilter');
   if(!input) { return; }
   input.focus();
   if(typeof input.select === 'function') { input.select(); }
}

function CYTObindContextMenuEvents() {
   CYTOinstance.on('cxttap', 'node', (e) => { CYTOshowNodeMenu(e.originalEvent, e.target.id()); });
   CYTOinstance.on('cxttap', 'edge', (e) => {
      if(!CYTOisEditMode()) { return CYTOshowBackgroundMenu(e.originalEvent); }
      CYTOshowEdgeMenu(e.originalEvent, e.target.id(), e.target.source().id(), e.target.target().id());
   });
   CYTOinstance.on('cxttap', (e) => { if(e.target === CYTOinstance) { CYTOshowBackgroundMenu(e.originalEvent); } });
}

function CYTObindTapEvents() {
   CYTOinstance.on('tap', 'edge', (e) => { CYTOcenterOnNode(e.target.target().id()); });
   CYTOinstance.on('tap', 'node', (e) => { CYTOscheduleNodeTap(e); });
   CYTOinstance.on('tap', (e) => {
      if(e.target !== CYTOinstance) { return; }
      if(CYTOinstance.connectMode) { return CYTOexitConnectMode(); }
      CYTOclearSelections();
   });
}

let CYTOcheckedSyncTimeout = null;
let CYTOscheduledNodeTapTimeout = null;
const CYTOnodeTapDelayMs = 250;

function CYTOscheduleCheckedSync() {
   clearTimeout(CYTOcheckedSyncTimeout);
   CYTOcheckedSyncTimeout = setTimeout(() => {
      CYTOcheckedSyncTimeout = null;
      CYTObridgeCheckedSetEmit(CYTOgetMarkedNodeIds());
      CYTOtoolbarSyncState();
   }, 0);
}

function CYTOsnapToGrid(e) {
   const node  = e.target;
   const pos   = node.position();
   const snapX = CYTOgridConst.snapX;
   const snapY = CYTOgridConst.snapY;
   node.position({x: Math.round(pos.x / snapX) * snapX, y: Math.round(pos.y / snapY) * snapY});
}

function CYTOhandleNodeDragFree(e) {
   CYTOsnapToGrid(e);
   if(!CYTOinstance || !CYTOisEditMode()) { return; }
   CYTOschedulePinsSave();
}

function CYTOclearSelections() {
   CYTOcancelScheduledNodeTap();
   CYTOremoveAllStyles();
   CYTOremoveHover();
   CYTObridgeCheckedSetEmit([]);
   CYTOtoolbarSyncState();
}

function CYTOscheduleNodeTap(e) {
   if(CYTOinstance.connectMode) { return CYTOhandleNodeTapConnect(e.target.id()); }
   CYTOcancelScheduledNodeTap();
   const nodeId    = e.target.id();
   const shiftHeld = !!e.originalEvent?.shiftKey;
   const ctrlHeld  = !!e.originalEvent?.ctrlKey;
   CYTOscheduledNodeTapTimeout = setTimeout(() => {
      CYTOscheduledNodeTapTimeout = null;
      const node = CYTOgetNodeById(nodeId);
      if(!node || node.length === 0) { return; }
      CYTOhandleNodeTap(node, {shiftHeld, ctrlHeld});
   }, CYTOnodeTapDelayMs);
}

function CYTOcancelScheduledNodeTap() {
   if(CYTOscheduledNodeTapTimeout) { clearTimeout(CYTOscheduledNodeTapTimeout); }
   CYTOscheduledNodeTapTimeout = null;
}

function CYTOaddHover(e) {
   CYTObridgeHoverEmit(e.target.isNode() ? e.target.id() : '');
   e.target.addClass('CYTOHovered');
   CYTOinstance.container().style.cursor = CYTOcursorForTarget(e.target);
   if(e.target.isEdge()) { CYTOshowEdgeTooltip(e.target); }
}

function CYTOcursorForTarget(target) {
   if(CYTOinstance?.connectMode) { return 'crosshair'; }
   if(target?.isNode?.()) { return 'default'; }
   return 'pointer';
}

function CYTOhandleNodeGrab() {
   if(!CYTOinstance || CYTOinstance.connectMode || !CYTOisEditMode()) { return; }
   CYTOinstance.container().style.cursor = 'default';
}

function CYTOhandleNodeFree() {
   if(!CYTOinstance) { return; }
   if(CYTOinstance.connectMode) {
      CYTOinstance.container().style.cursor = 'crosshair';
      return;
   }
   const hoveredNode = CYTOinstance.nodes('.CYTOHovered');
   if(hoveredNode.length) {
      CYTOinstance.container().style.cursor = CYTOcursorForTarget(hoveredNode[0]);
      return;
   }
   const hoveredEdge = CYTOinstance.edges('.CYTOHovered');
   CYTOinstance.container().style.cursor = hoveredEdge.length ? CYTOcursorForTarget(hoveredEdge[0]) : 'default';
}

function CYTOshowEdgeTooltip(edge) {
   let tooltip = document.getElementById('cyto-edge-tooltip');
   if(!tooltip) {
      tooltip = document.createElement('div');
      tooltip.id = 'cyto-edge-tooltip';
      tooltip.className = 'CYTOedgeTooltip';
      document.body.appendChild(tooltip);
   }
   const sourceLabel = edge.source().data('label') || edge.source().id();
   const targetLabel = edge.target().data('label') || edge.target().id();
   tooltip.textContent = `${sourceLabel} → ${targetLabel}`;
   tooltip.classList.add('CYTOedgeTooltipVisible');
   document.addEventListener('mousemove', CYTOupdateTooltipPosition);
}

function CYTOhideEdgeTooltip() {
   const tooltip = document.getElementById('cyto-edge-tooltip');
   if(tooltip) { tooltip.classList.remove('CYTOedgeTooltipVisible'); }
   document.removeEventListener('mousemove', CYTOupdateTooltipPosition);
}

function CYTOupdateTooltipPosition(e) {
   const tooltip = document.getElementById('cyto-edge-tooltip');
   if(tooltip) {
      tooltip.style.left = `${e.clientX + 12  }px`;
      tooltip.style.top = `${e.clientY + 12  }px`;
   }
}

function CYTOremoveHover() {
   CYTObridgeHoverClearEmit();
   CYTOremoveHovered();
   CYTOhideEdgeTooltip();
   if(!CYTOinstance.connectMode) { CYTOinstance.container().style.cursor = 'default'; }
}

function CYTOenterConnectMode(sourceIds) {
   const ids = Array.isArray(sourceIds) ? sourceIds : [sourceIds];
   CYTOinstance.connectMode = { sourceIds: ids };
   ids.forEach(id => {
      const node = CYTOinstance.getElementById(id);
      if(node && node.length) { node.addClass('CYTOConnectSource'); }
   });
   CYTOinstance.container().style.cursor = 'crosshair';
   document.addEventListener('keydown', CYTOhandleConnectKey);
   CYTOinstance.boxSelectionEnabled(false);
   const connected = CYTOgetConnectedElements(ids);
   connected.addClass('CYTODimmed');
   CYTOtoolbarSyncState();
}

function CYTOexitConnectMode() {
   if(!CYTOinstance.connectMode) { return; }
   const connected = CYTOgetConnectedElements(CYTOinstance.connectMode.sourceIds);
   connected.removeClass('CYTODimmed');
   CYTOinstance.connectMode.sourceIds.forEach(id => {
      const node = CYTOinstance.getElementById(id);
      if(node && node.length) { node.removeClass('CYTOConnectSource'); }
   });
   CYTOinstance.connectMode = null;
   CYTOinstance.container().style.cursor = 'default';
   CYTOhideConnectStatus();
   CYTOhideTargetSearch();
   document.removeEventListener('keydown', CYTOhandleConnectKey);
   CYTOinstance.boxSelectionEnabled(true)
   CYTOtoolbarSyncState();
}

function CYTOhandleNodeTap(node, {shiftHeld = false, ctrlHeld = false} = {}) {
   if(shiftHeld) {
      node.toggleClass('CYTOMarked');
      CYTOscheduleCheckedSync();
      return;
   }

   CYTOinstance.elements().removeClass('CYTOHighlighted CYTODimmed');

   if(ctrlHeld) {
      const path = node.predecessors().union(node.successors()).union(node);
      path.addClass('CYTOHighlighted');
      CYTOinstance.elements().not(path).addClass('CYTODimmed');
   }
   node.toggleClass('CYTOMarked');
   CYTOscheduleCheckedSync();
}

function CYTOhandleNodeTapConnect(targetId) {
   if(!CYTOinstance.connectMode) { return; }
   const {sourceIds} = CYTOinstance.connectMode;
   if(sourceIds.includes(targetId)) { return SNCKsnackbar('error', 'Cannot connect node to itself'); }
   sourceIds.forEach(sourceId => CYTOaddConnection(targetId, sourceId));
   CYTOexitConnectMode();
}

function CYTOhandleConnectKey(e) {
   if(e.key === 'Escape') {
      CYTOexitConnectMode();
      CYTOhideTargetSearch();
   }
}

function CYTOgetConnectedElements(sourceIds) {
   const ids = Array.isArray(sourceIds) ? sourceIds : [sourceIds];
   let connected = CYTOinstance.collection();
   ids.forEach(id => {
      const node = CYTOinstance.getElementById(id);
      if(!node || !node.length) { return; }
      const predecessors = node.predecessors();
      const successors = node.successors();
      connected = connected.union(predecessors).union(successors);
   });
   return connected;
}

function CYTOshowTargetSearch(sourceIds) {
   const ids = Array.isArray(sourceIds) ? sourceIds : [sourceIds];
   let overlay = document.getElementById('cyto-search-overlay');
   if(!overlay) {
      overlay           = document.createElement('div');
      overlay.id        = 'cyto-search-overlay';
      overlay.className = 'CYTOsearchOverlay';
      overlay.innerHTML = `
         <div class="CYTOsearchContainer">
            <input type="text" class="CYTOsearchInput" placeholder="Click a node, type to search, or press Esc to cancel">
            <div class="CYTOsearchResults"></div>
         </div>
      `;
      document.body.appendChild(overlay);
   }
   const input             = overlay.querySelector('.CYTOsearchInput');
   const results           = overlay.querySelector('.CYTOsearchResults');
   input.value           = '';
   results.innerHTML     = '';
   overlay.classList.add('CYTOsearchOverlayVisible');
   input.focus();
   input.oninput   = () => { CYTOfilterTargetNodes(input.value, ids, results); };
   input.onkeydown = (e) => { CYTOhandleSearchKeydown(e, ids, results); };
}

function CYTOhandleSearchKeydown(e, sourceIds, resultsContainer) {
   const items    = [...resultsContainer.querySelectorAll('.CYTOsearchItem')];
   const selected = resultsContainer.querySelector('.CYTOsearchItem.CYTOsearchItemSelected');
   const index    = selected ? items.indexOf(selected) : -1;

   const navigate = next => {
      selected?.classList.remove('CYTOsearchItemSelected');
      items[next]?.classList.add('CYTOsearchItemSelected');
   };

   switch (e.key) {
      case 'ArrowDown':  e.preventDefault(); navigate(index + 1 < items.length ? index + 1 : 0); break;
      case 'ArrowUp':    e.preventDefault(); navigate(index > 0 ? index - 1 : items.length - 1); break;
      case 'Enter':
         e.preventDefault();
         if (selected) {
            sourceIds.forEach(sourceId => CYTOaddConnection(selected.dataset.nodeid, sourceId));
            CYTOexitConnectMode();
            CYTOhideTargetSearch();
         }
         break;
      case 'Escape':
         CYTOexitConnectMode(); CYTOhideTargetSearch();
         break;
      default:
         break;
   }
}

function CYTOhideTargetSearch() {
   const overlay = document.getElementById('cyto-search-overlay');
   if(overlay) { overlay.classList.remove('CYTOsearchOverlayVisible'); }
}

function CYTOfilterTargetNodes(query, sourceIds, resultsContainer) {
   const ids = Array.isArray(sourceIds) ? sourceIds : [sourceIds];
   const nodes                  = CYTOinstance.nodes();
   const filtered               = nodes.filter(n => {
      const label = n.data('label') || '';
      return !ids.includes(n.id()) && label.toLowerCase().includes(query.toLowerCase());
   });
   resultsContainer.innerHTML = '';
   filtered.forEach(node => {
      const item            = document.createElement('div');
      item.className      = 'CYTOsearchItem';
      item.dataset.nodeid = node.id();
      item.textContent    = node.data('label') || node.id();
      item.onclick        = () => {
         ids.forEach(sourceId => CYTOaddConnection(node.id(), sourceId));
         CYTOexitConnectMode();
         CYTOhideTargetSearch();
      };
      resultsContainer.appendChild(item);
   });
}

function CYTOhideConnectStatus() {
   const status = document.getElementById('cyto-connect-status');
   if(status) { status.classList.remove('CYTOconnectStatusVisible'); }
}
