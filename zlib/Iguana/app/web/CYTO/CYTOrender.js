// CYTOinstance.connectMode controls whether we are in connection mode or not (bool)
let CYTOinstance = null;

function CYTOrenderInit() {
   if(CYTOnoComps()) { return; }
   CYTOwelcomeRemove();
   if(!CYTOshouldRender()) { return; }
   const elements = CYTOprepareElements();
   if(!elements) { return; }
   CYTOcreateContainer();
   CYTOrender(elements);
}

function CYTOshouldRender() {
   const container = document.querySelector('.CYTOcontainer');
   if(!container) { return false; }
   const cyDiv = document.getElementById('cy');
   if(!cyDiv || !CYTOinstance) { CYTOdestroy(); }
   CYTOcreateHelpBox();
   return true;
}

function CYTOshouldDestroyForEmptyElements() {
   return !CYTOinstance || CYTOhasScopedView() || CYTOshouldShowWelcome();
}

function CYTOsyncViewportFilterState(currentViewportFilterKey) {
   if(!CYTOinstance || currentViewportFilterKey === CYTOinstance.lastViewportFilterKey) { return; }
   const previousKey = String(CYTOinstance.lastViewportFilterKey || '');
   const previousScopeParts = previousKey.split('|').slice(0, 2);
   const nextScopeParts     = String(currentViewportFilterKey || '').split('|').slice(0, 2);
   const previousSearchScope = previousScopeParts.join('|');
   const nextSearchScope     = nextScopeParts.join('|');
   const searchScopeChanged  = previousSearchScope !== nextSearchScope;
   const previousHasSearch   = !!(previousScopeParts[0] || previousScopeParts[1]);
   const nextHasSearch       = !!(nextScopeParts[0] || nextScopeParts[1]);
   const searchScopeCleared  = previousHasSearch && !nextHasSearch;
   CYTOinstance.pendingAnimatedResize = true;
   CYTOinstance.pendingSkipSavedViewport = searchScopeChanged && CYTOconsumeSearchViewportRestoreSkip();
   CYTOinstance.pendingSearchPanOnly = CYTOinstance.pendingSkipSavedViewport;
   CYTOinstance.pendingPreserveYPanOnClear = CYTOinstance.pendingSkipSavedViewport && searchScopeCleared;
   CYTOinstance.lastViewportFilterKey = currentViewportFilterKey;
}

function CYTOprepareElements() {
   const elements = CYTObuildElements(DASHmapData() || {}, DASHpinData() || {});
   if(elements.nodes.length === 0) {
      if(CYTOshouldDestroyForEmptyElements()) { CYTOdestroy(); }
      return null;
   }
   const currentViewportFilterKey = CYTOviewportFilterKey();
   CYTOsyncViewportFilterState(currentViewportFilterKey);
   const currentHash = CYTOhashElements(elements);
   if(CYTOinstance && currentHash === CYTOinstance.lastHash) { return null; }
   if(CYTOinstance) { CYTOinstance.lastHash = currentHash; }
   return elements;
}

function CYTOcreateContainer() {
   const container = document.querySelector('.CYTOcontainer');
   if(!container || container.querySelector('#cy')) { return; }
   container.appendChild(Object.assign(document.createElement('div'), {id: 'cy', className: 'CYTOmap'})
   );
}

// we only update when we have changes we care about -- Math.imul replaces the (hash << 5) - hash trick
function CYTOhashElements(elements) {
   let hash  = elements.nodes.length * 31 + elements.edges.length;
   const mix = str => { for(let i = 0; i < str.length; i++) { hash = Math.imul(31, hash) + str.charCodeAt(i) | 0; } };
   elements.nodes.forEach(({data: d, position: p}) => {
      const tags = Array.isArray(d.tags) ? d.tags.join(',') : '';
      mix(`|${d.id}|${d.light}|${d.label}|${tags}|${d.borderColor || ''}|${d.isTemplate ? 1 : 0}|${d.hostServerGuid}|${d.showActivityPulse}|${d.queueCount}|${d.errorCount}|${p ? p.x : ''}|${p ? p.y : ''}|`);
   });
   elements.edges.forEach(({data: d}) => mix(`>${d.id}<`));
   return hash.toString();
}

function CYTOrenderComplete(cyDiv, {shouldResetViewport, shouldAnimateViewport}) {
   const restoreSavedViewport = !(CYTOinstance && CYTOinstance.pendingSkipSavedViewport);
   if(CYTOinstance) { CYTOinstance.pendingSkipSavedViewport = false; }
   CYTOresize({resetViewport: shouldResetViewport, animate: shouldAnimateViewport, restoreSavedViewport});
   cyDiv.classList.remove('CYTOmapHidden');
   CYTOtoolbarRender();
   CYTOhandleDashCheckedSetEvent(DASHlistOfCheckedComponents());
   CYTOscheduleNodeContentSync();
   CYTOfocusDetailedCardComponent();
}

function CYTOrender(elements) {
   if(!elements || elements.nodes.length === 0) { return; }
   const cyDiv = document.getElementById('cy');
   cyDiv.classList.add('CYTOmapHidden');
   const shouldResetViewport   = !CYTOinstance || !!CYTOinstance.pendingAnimatedResize;
   const shouldAnimateViewport = !!CYTOinstance && !!CYTOinstance.pendingAnimatedResize;
   const renderOptions         = {shouldResetViewport, shouldAnimateViewport};
   const resizeAndShow         = () => { CYTOrenderComplete(cyDiv, renderOptions); };

   if(!CYTOinstance) {
      CYTOresolveInitLayout(CYTOinitInstance(elements, cyDiv), resizeAndShow);
   } else {
      const diff = CYTOdiffElements(elements);
      CYTOupdateElements(elements, () => CYTOresolveUpdateLayout(diff, resizeAndShow));
   }
}

function CYTOcyOptions(elements, cyDiv, hasPins) {
   const cyOptions = {
      container    : cyDiv,
      elements,
      style        : CYTOstylesheet(),
      minZoom      : 0.2,
      maxZoom      : 3,
      wheelSensitivity: 0.4,
      boxSelectionEnabled: true,
      selectionType: 'additive'
   };
   if(hasPins) { cyOptions.layout = {name: 'preset'}; }
   return cyOptions;
}

function CYTOdisableCytoscapeWarnings() {
   if(typeof cytoscape?.warnings !== 'function') { return; }
   cytoscape.warnings(false);
}

function CYTOfocusDetailedCardComponent() {
   if(!CYTOinstance || PAGEhash() !== 'dashboard/details') { return; }
   const componentId = PAGEhashParameter('component');
   if(!componentId || CYTOinstance.lastFocusedCardComponentId === componentId) { return; }
   const node = CYTOgetNodeById(componentId);
   if(!node || node.length === 0) { return; }
   CYTOcenterOnNode(componentId);
   CYTOinstance.lastFocusedCardComponentId = componentId;
}

function CYTOinitInstance(elements, cyDiv) {
   const hasPins     = elements.nodes.some(n => !n.data.unpositioned);
   const hasUnpinned = elements.nodes.some(n => n.data.unpositioned);
   CYTOdisableCytoscapeWarnings();
   CYTOinstance                = cytoscape(CYTOcyOptions(elements, cyDiv, hasPins));
   CYTOinstance.lastHash       = CYTOhashElements(elements);
   CYTOinstance.pendingAnimatedResize = false;
   CYTOinstance.pendingSkipSavedViewport = false;
   CYTOinstance.pendingSearchPanOnly = false;
   CYTOinstance.pendingPreserveYPanOnClear = false;
   CYTOinstance.lastViewportFilterKey = CYTOviewportFilterKey();
   CYTOinstance.lastFocusedCardComponentId = '';
   CYTOapplyEditMode();
   CYTOlisteners();
   CYTOsyncProcessingPulse();
   return {hasPins, hasUnpinned};
}

function CYTOresolveInitLayout({ hasPins, hasUnpinned }, onDone) {
   if (hasPins && hasUnpinned) { CYTOpositionNewNodes(onDone); }
   else if (hasPins)           { onDone(); }
   else                        { CYTOrunLayout(onDone); }
}

function CYTOdiffElements(elements) {
   const existingIds = new Set(CYTOinstance.nodes().map(n => n.id()));
   const newIds      = new Set(elements.nodes.map(n => n.data.id));
   return {
      hasNewNodesWithoutPins: elements.nodes.some(n => n.data.unpositioned && !existingIds.has(n.data.id) && !CYTOnodePositions[n.data.id]),
      hasRemovedNodes       : [...existingIds].some(id => !newIds.has(id))
   };
}

function CYTOresolveUpdateLayout({hasNewNodesWithoutPins}, onDone) {
   if(!hasNewNodesWithoutPins) { onDone(); return; }
   const hasPins = Object.keys(CYTOnodePositions).length > 0;
   if(hasPins) { CYTOpositionNewNodes(onDone); } else { CYTOrunLayout(onDone); }
}

function CYTOmetricCount(comp, ...keys) {
   for(let i = 0; i < keys.length; i++) {
      const value = Number(comp?.[keys[i]]);
      if(Number.isFinite(value)) { return Math.max(0, value); }
   }
   return 0;
}

function CYTOcomponentMetrics(comp) {
   const light      = comp.light || 'OFF';
   const sourceCount = Array.isArray(comp.sources) ? comp.sources.length : 0;
   const queueCount = CYTOmetricCount(comp, 'queue_count');
   const errorCount = CYTOmetricCount(comp, 'error_count', 'component_error_count');
   return {
      light,
      sourceCount,
      queueCount,
      errorCount,
      isTemplate      : !!comp.template,
      showActivityPulse: light === 'ON' && queueCount > 0
   };
}

function CYTOappendNodeClass(nodeClasses, shouldAdd, className) {
   if(shouldAdd) { nodeClasses.push(className); }
}

function CYTObuildNodeClasses(metrics, pin) {
   const nodeClasses = ['CYTOComponent'];
   CYTOappendNodeClass(nodeClasses, metrics.isTemplate, 'CYTOTemplate');
   CYTOappendNodeClass(nodeClasses, metrics.queueCount > 0, 'CYTOHasQueueCount');
   CYTOappendNodeClass(nodeClasses, metrics.errorCount > 0, 'CYTOHasErrorCount');
   CYTOappendNodeClass(nodeClasses, metrics.light === 'ON', 'CYTORunning');
   CYTOappendNodeClass(nodeClasses, metrics.light === 'YELLOW', 'CYTOInProgress');
   CYTOappendNodeClass(nodeClasses, metrics.light === 'RED', 'CYTOError');
   CYTOappendNodeClass(nodeClasses, metrics.showActivityPulse, 'CYTOProcessing');
   return nodeClasses.join(' ');
}

function CYTOnormalizeBorderColor(borderColor) {
   const value = String(borderColor || '').trim();
   return value || '';
}

function CYTObuildNodeData(comp, metrics) {
   const data = {
      id            : comp.guid,
      label         : comp.name || 'Unnamed',
      light         : metrics.light,
      queueCount    : metrics.queueCount,
      errorCount    : metrics.errorCount,
      sourceCount   : metrics.sourceCount,
      tags          : comp.tags || [],
      isTemplate    : metrics.isTemplate,
      hostServerGuid: comp.host_server_guid || '',
      showActivityPulse: metrics.showActivityPulse
   };
   const borderColor = CYTOnormalizeBorderColor(comp.border);
   if(borderColor) { data.borderColor = borderColor; }
   return data;
}

function CYTObuildNode(comp, pin) {
   const metrics = CYTOcomponentMetrics(comp);
   const node    = {
      data   : CYTObuildNodeData(comp, metrics),
      classes: CYTObuildNodeClasses(metrics, pin)
   };

   if(pin) {
      node.position = {x: pin.px, y: pin.py};
   } else {
      node.data.unpositioned = true;
   }
   return node;
}

function CYTOcomponentSortCompare(left, right) {
   const leftName  = String(left?.name || '');
   const rightName = String(right?.name || '');
   const nameDiff  = leftName.localeCompare(rightName);
   if(nameDiff !== 0) { return nameDiff; }
   return String(left?.guid || '').localeCompare(String(right?.guid || ''));
}

function CYTOmapComponents(mapData) {
   return Object.values(mapData).filter(comp => comp && comp.guid).sort(CYTOcomponentSortCompare);
}

function CYTOnormalizePin(pin) {
   return {px: Number(pin.px), py: Number(pin.py)};
}

function CYTOoffsetDuplicatePin(pin, duplicateIndex, duplicatePinNudge) {
   const normalizedPin = CYTOnormalizePin(pin);
   if(duplicateIndex === 0) { return normalizedPin; }
   return {
      px: normalizedPin.px - duplicatePinNudge * duplicateIndex,
      py: normalizedPin.py + duplicatePinNudge * duplicateIndex
   };
}

function CYTOresolvedPinForGuid(guid, pinData, seenPins, duplicatePinNudge) {
   const pin = pinData[guid];
   if(!pin) { return null; }
   const normalizedPin  = CYTOnormalizePin(pin);
   const key            = `${normalizedPin.px},${normalizedPin.py}`;
   const duplicateIndex = seenPins[key] || 0;
   seenPins[key]        = duplicateIndex + 1;
   return CYTOoffsetDuplicatePin(normalizedPin, duplicateIndex, duplicatePinNudge);
}

function CYTObuildElements(mapData, pinData) {
   const duplicatePinNudge = 12;
   const seenPins          = Object.create(null);
   const nodes             = [];
   const nodeMap           = {};
   const components        = CYTOmapComponents(mapData);
   for(let i = 0; i < components.length; i++) {
      const comp = components[i];
      const pin = CYTOresolvedPinForGuid(comp.guid, pinData, seenPins, duplicatePinNudge);
      nodes.push(CYTObuildNode(comp, pin));
      nodeMap[comp.guid] = true;
   }
   return { nodes, edges: CYTObuildEdges(nodeMap, mapData) };
}

function CYTObuildEdges(nodeMap, mapData) {
   const edges     = [];
   const mapValues = Object.values(mapData);
   for(let i = 0; i < mapValues.length; i++) {
      const nodeData = mapValues[i];
      if(!nodeData || !nodeData.guid) { continue; }
      const {guid}  = nodeData;
      const sources = nodeData.sources || [];
      for(let j = 0; j < sources.length; j++) {
         const sourceGuid = sources[j];
         if(!nodeMap[sourceGuid] || !nodeMap[guid]) { continue; }
         edges.push({data: {id: `${sourceGuid}_to_${guid}`, source: sourceGuid, target: guid}, classes: 'connection'});
      }
   }
   return edges.sort((left, right) => left.data.id.localeCompare(right.data.id));
}
