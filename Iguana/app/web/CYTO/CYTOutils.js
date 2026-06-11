const CYTOprocessingPulseIntervalMs = 700;
const CYTOstartupPulseDurationMs = 1200;
const CYTOpinSaveDelayMs = 250;
const CYTOviewportStoreName = 'CYTOviewportStates';
const CYTOviewportSaveDelayMs = 150;
const CYTOviewportPaddingDefault = {
   x: 72,
   y: 88
};
let CYTOeditModeEnabled = false;
let CYTOskipNextSearchViewportRestoreFlag = false;

function CYTOgetNodeById(id)    { return CYTOinstance ? CYTOinstance.nodes().getElementById(id) : null; }
function CYTOgetMarkedNodes()   { return CYTOinstance ? CYTOinstance.nodes('.CYTOMarked') : []; }
function CYTOgetMarkedNodeIds() { return CYTOinstance ? CYTOinstance.nodes('.CYTOMarked').map(n => n.id()) : [] }
function CYTOgetHoveredId()     { return CYTOinstance ? CYTOinstance.nodes('.CYTOHovered').id() : null }
function CYTOremoveAllStyles()  { if(CYTOinstance) { CYTOinstance.elements().removeClass('CYTOHighlighted CYTODimmed CYTOMarked'); } }
function CYTOremoveHovered()    { if(CYTOinstance) { CYTOinstance.elements().removeClass('CYTOHovered'); } }
function CYTOmapResponse()      { return MODELdata?.DASHcomponentMapData?.data; }
function CYTOmapLoaded()        { return CYTOmapResponse() !== undefined; }
function CYTOlistLoaded()       { return MODELdata?.DASHcomponentListData?.data?.component_data !== undefined; }
function CYTOisEditMode()       { return CYTOeditModeEnabled; }

function CYTOapplyEditMode(instance = CYTOinstance) {
   if(!instance) { return; }
   if(!CYTOisEditMode() && instance.connectMode) { CYTOexitConnectMode(); }
   if(CYTOisEditMode()) { instance.nodes().grabify(); }
   else                 { instance.nodes().ungrabify(); }
   if(!instance.connectMode) { instance.container().style.cursor = 'default'; }
}

function CYTOsetEditMode(enabled) {
   const wasEditMode = CYTOisEditMode();
   CYTOeditModeEnabled = !!enabled;
   CYTOapplyEditMode();
   CYTOsyncNodeContentOverlay();
   CYTOtoolbarSyncState();
   if(wasEditMode && !enabled) {
      CYTOcommitPinsToInstance();
   }
}

function CYTOtoggleEditMode() {
   CYTOsetEditMode(!CYTOisEditMode());
}

function CYTOmarkSearchViewportRestoreSkip() {
   CYTOskipNextSearchViewportRestoreFlag = true;
}

function CYTOconsumeSearchViewportRestoreSkip() {
   const skip = CYTOskipNextSearchViewportRestoreFlag;
   CYTOskipNextSearchViewportRestoreFlag = false;
   return skip;
}

function CYTOclearPinSaveTimeout(instance = CYTOinstance) {
   if(instance?.pinSaveTimeout) { clearTimeout(instance.pinSaveTimeout); }
   if(instance) { instance.pinSaveTimeout = null; }
}

function CYTOcommitPinsToInstance() {
   const Message = GINsavePins();
   GINaddAndCommit({message: Message, users: true, allow_empty: false});
}

function CYTOflushPinsSave(instance = CYTOinstance) {
   if(!instance) { return; }
   CYTOclearPinSaveTimeout(instance);
   if(instance.pinSaveInFlight) { return; }
   if(!instance.pinSaveQueued) { return; }
   instance.pinSaveQueued   = false;
   instance.pinSaveInFlight = true;
   CYTOsavePins(instance.nodes(), {
      onComplete: () => {
         instance.pinSaveInFlight = false;
         if(instance.pinSaveQueued) { CYTOflushPinsSave(instance); }
      }
   });
}

function CYTOschedulePinsSave(instance = CYTOinstance) {
   if(!instance) { return; }
   instance.pinSaveQueued = true;
   CYTOclearPinSaveTimeout(instance);
   instance.pinSaveTimeout = setTimeout(() => {
      instance.pinSaveTimeout = null;
      CYTOflushPinsSave(instance);
   }, CYTOpinSaveDelayMs);
}

function CYTOclearViewportSaveTimeout(instance = CYTOinstance) {
   if(instance && instance.viewportSaveTimeout) { clearTimeout(instance.viewportSaveTimeout); }
   if(instance) { instance.viewportSaveTimeout = null; }
}

function CYTOsavedViewportState(key = CYTOviewportFilterKey()) {
   const saved = STOREreadJsonVariable(CYTOviewportStoreName)[key] || {};
   const zoom  = Number(saved.zoom);
   const panX  = Number(saved.panX);
   const panY  = Number(saved.panY);
   if(!Number.isFinite(zoom) || !Number.isFinite(panX) || !Number.isFinite(panY)) { return null; }
   const minZoom = CYTOinstance.minZoom();
   const maxZoom = CYTOinstance.maxZoom();
   return {
      zoom: Math.min(Math.max(zoom, minZoom), maxZoom),
      pan : {x: panX, y: panY}
   };
}

function CYTOsaveViewportState(instance = CYTOinstance, key = instance.lastViewportFilterKey || CYTOviewportFilterKey()) {
   if(!instance) { return; }
   const zoom = Number(instance.zoom());
   const pan  = instance.pan();
   const states = STOREreadJsonVariable(CYTOviewportStoreName);
   states[key] = {
      zoom,
      panX: Number(pan.x),
      panY: Number(pan.y)
   };
   STOREsaveJsonVariable(CYTOviewportStoreName, states);
}

function CYTOclearViewportState(key = CYTOviewportFilterKey()) {
   const states = STOREreadJsonVariable(CYTOviewportStoreName);
   if(!(key in states)) { return; }
   delete states[key];
   STOREsaveJsonVariable(CYTOviewportStoreName, states);
}

function CYTOscheduleViewportSave(instance = CYTOinstance) {
   if(!instance) { return; }
   CYTOclearViewportSaveTimeout(instance);
   instance.viewportSaveTimeout = setTimeout(() => {
      instance.viewportSaveTimeout = null;
      CYTOsaveViewportState(instance);
   }, CYTOviewportSaveDelayMs);
}

function CYTOrestoreViewport({animate = false} = {}) {
   if(!CYTOinstance) { return false; }
   const saved = CYTOsavedViewportState();
   if(!saved) { return false; }
   if(animate) {
      CYTOinstance.animate({
         zoom: saved.zoom,
         pan : saved.pan
      }, {
         duration: 180,
         easing: 'ease-out-cubic'
      });
   } else {
      CYTOinstance.zoom(saved.zoom);
      CYTOinstance.pan(saved.pan);
   }
   return true;
}

function CYTOclearProcessingPulse(instance = CYTOinstance) {
   if(!instance) { return; }
   if(instance.processingPulseInterval) {
      clearInterval(instance.processingPulseInterval);
      instance.processingPulseInterval = null;
   }
   instance.processingPulseExpanded = false;
   instance.nodes('.CYTOProcessingPulse').removeClass('CYTOProcessingPulse');
}

function CYTOtriggerStartupPulse(node, durationMs = CYTOstartupPulseDurationMs) {
   if(!node || node.length === 0) { return; }
   node.data('startupPulseUntil', Date.now() + durationMs);
}

function CYTOrefreshProcessingClasses(instance = CYTOinstance) {
   if(!instance) { return instance ? instance.collection() : []; }
   const now = Date.now();
   instance.nodes().forEach(node => {
      const startupPulseUntil = Number(node.data('startupPulseUntil') || 0);
      const hasStartupPulse   = startupPulseUntil > now;
      if(startupPulseUntil && !hasStartupPulse) { node.removeData('startupPulseUntil'); }
      if(node.data('showActivityPulse') || hasStartupPulse) { node.addClass('CYTOProcessing'); }
      else {
         node.removeClass('CYTOProcessing');
         node.removeClass('CYTOProcessingPulse');
      }
   });
   return instance.nodes('.CYTOProcessing');
}

function CYTOsetProcessingPulseState(instance, expanded) {
   if(!instance) { return false; }
   const processingNodes = CYTOrefreshProcessingClasses(instance);
   instance.nodes('.CYTOProcessingPulse').not(processingNodes).removeClass('CYTOProcessingPulse');
   if(processingNodes.length === 0) {
      CYTOclearProcessingPulse(instance);
      return false;
   }
   if(expanded) { processingNodes.addClass('CYTOProcessingPulse'); }
   else         { processingNodes.removeClass('CYTOProcessingPulse'); }
   return true;
}

function CYTOsyncProcessingPulse() {
   if(!CYTOinstance) { return; }
   const hasProcessing = CYTOsetProcessingPulseState(CYTOinstance, !!CYTOinstance.processingPulseExpanded);
   if(!hasProcessing || CYTOinstance.processingPulseInterval) { return; }
   CYTOinstance.processingPulseExpanded = false;
   CYTOinstance.processingPulseInterval = setInterval(() => {
      if(!CYTOinstance) { return; }
      CYTOinstance.processingPulseExpanded = !CYTOinstance.processingPulseExpanded;
      CYTOsetProcessingPulseState(CYTOinstance, CYTOinstance.processingPulseExpanded);
   }, CYTOprocessingPulseIntervalMs);
}

function CYTOmapComponentCount() {
   const mapData = CYTOmapResponse();
   if(!mapData) { return 0; }
   return Object.values(mapData).filter(component => component && component.guid).length;
}

function CYTOhasScopedView() {
   return !!PAGEhashParameter('filter') || !!PAGEhashParameter('whole_word') || DASHshowOnlyCheckedRows();
}

function CYTOshouldShowWelcome() {
   return CYTOmapLoaded() && CYTOlistLoaded() && Number(DASHcomponentCount()) === 0 && !CYTOhasScopedView();
}

function CYTOresize({resetViewport = false, animate = false, restoreSavedViewport = true} = {}) {
   if(CYTOinstance) {
      CYTOinstance.resize();
      if(resetViewport) { CYTOinstance.stop(); }
      if(resetViewport && !(restoreSavedViewport && CYTOrestoreViewport({animate}))) { CYTOsetDefaultViewport({animate}); }
      CYTOinstance.pendingAnimatedResize = false;
      CYTOscheduleNodeContentSync();
   }
}

function CYTOresetView() {
   CYTOremoveAllStyles();
   CYTOclearViewportState();
   if(CYTOinstance) {
      CYTOinstance.stop();
      CYTOinstance.resize();
      CYTOsetDefaultViewport({useScopedView: false});
      CYTOinstance.pendingAnimatedResize = false;
      CYTOscheduleNodeContentSync();
   }
   CYTOsaveViewportState();
}

function CYTOdestroy() {
   const cyDiv = document.getElementById('cy');
   if(cyDiv) { cyDiv.remove(); }
   CYTOeditModeEnabled = false;
   if(CYTOinstance) {
      CYTOclearViewportSaveTimeout(CYTOinstance);
      CYTOclearPinSaveTimeout(CYTOinstance);
      CYTOinstance.pinSaveQueued   = false;
      CYTOinstance.pinSaveInFlight = false;
      CYTOsaveViewportState(CYTOinstance, CYTOinstance.lastViewportFilterKey);
      CYTOclearProcessingPulse();
      CYTOclearNodeContentOverlay(CYTOinstance);
      CYTOinstance.destroy();
      CYTOinstance = null;
   }
   CYTOnodePositionsReset();
}

function CYTOcenterOnNode(nodeId) {
   if(!CYTOinstance) { return; }
   const node = CYTOgetNodeById(nodeId);
   if(!node || node.length === 0) { return; }
   const zoom       = Number(CYTOinstance.zoom());
   const pan        = CYTOinstance.pan();
   const viewW      = CYTOinstance.container().clientWidth;
   const viewH      = CYTOinstance.container().clientHeight;
   const marginX    = 52;
   const marginTop  = 36;
   const marginBottom = 36;
   const rendered   = node.renderedPosition();
   const halfW      = (Number(node.width()) * zoom) / 2;
   const halfH      = (Number(node.height()) * zoom) / 2;
   const nodeLeft   = Number(rendered.x) - halfW;
   const nodeRight  = Number(rendered.x) + halfW;
   const nodeTop    = Number(rendered.y) - halfH;
   const nodeBottom = Number(rendered.y) + halfH;
   let dx = 0;
   let dy = 0;
   const outsideX = nodeLeft < marginX || nodeRight > viewW - marginX;
   const outsideY = nodeTop < marginTop || nodeBottom > viewH - marginBottom;
   if(nodeLeft < marginX) { dx = marginX - nodeLeft; }
   else if(nodeRight > viewW - marginX) { dx = (viewW - marginX) - nodeRight; }
   if(nodeTop < marginTop) { dy = marginTop - nodeTop; }
   else if(nodeBottom > viewH - marginBottom) { dy = (viewH - marginBottom) - nodeBottom; }
   const verticalCenterDelta = (viewH / 2) - Number(rendered.y);
   const verticalCenterDeadzone = 18;
   if(Math.abs(verticalCenterDelta) > verticalCenterDeadzone) {
      const alwaysVerticalCenterBias = 0.16;
      dy += verticalCenterDelta * alwaysVerticalCenterBias;
   }
   if(dx === 0 && dy === 0) { return; }
   let targetPan = {
      x: Number(pan.x) + dx,
      y: Number(pan.y) + dy
   };
   // When the node is out on both axes (corner case), bias a little toward center.
   if(outsideX && outsideY) {
      const centerBias = 0.24;
      const centerPan = {
         x: Number(pan.x) + ((viewW / 2) - Number(rendered.x)),
         y: Number(pan.y) + ((viewH / 2) - Number(rendered.y))
      };
      targetPan = {
         x: Number(targetPan.x) + ((Number(centerPan.x) - Number(targetPan.x)) * centerBias),
         y: Number(targetPan.y) + ((Number(centerPan.y) - Number(targetPan.y)) * centerBias)
      };
   } else if(outsideY) {
      const verticalCenterBias = 0.32;
      const centerPanY = Number(pan.y) + ((viewH / 2) - Number(rendered.y));
      targetPan = {
         x: Number(targetPan.x),
         y: Number(targetPan.y) + ((centerPanY - Number(targetPan.y)) * verticalCenterBias)
      };
   }
   CYTOinstance.animate({
      pan: targetPan
   }, {
      duration: 180,
      easing: 'ease-out-cubic'
   });
}

function CYTOsetDefaultViewport({animate = false, useScopedView = true} = {}) {
   if(!CYTOinstance) { return; }
   const panOnlySearch = !!CYTOinstance.pendingSearchPanOnly;
   if(panOnlySearch) { CYTOinstance.pendingSearchPanOnly = false; }
   const preserveYPanOnClear = !!CYTOinstance.pendingPreserveYPanOnClear;
   if(preserveYPanOnClear) { CYTOinstance.pendingPreserveYPanOnClear = false; }
   if(useScopedView && CYTOhasScopedView()) {
      const targetNodes = CYTOinstance.nodes();
      if(targetNodes && targetNodes.length) {
         if(panOnlySearch) {
            if(CYTOhasVisibleTargetNodes(targetNodes)) { return; }
            const zoom = Number(CYTOinstance.zoom());
            const bb   = targetNodes.boundingBox();
            const searchLeftPadExtra = 24;
            const targetPan = {
               x: -bb.x1 * zoom + CYTOdefaultPadX() + searchLeftPadExtra,
               y: (CYTOinstance.container().clientHeight / 2) - ((bb.y1 + (bb.h / 2)) * zoom)
            };
            if(animate) {
               CYTOinstance.animate({
                  pan: targetPan
               }, {
                  duration: 180,
                  easing: 'ease-out-cubic'
               });
            } else {
               CYTOinstance.pan(targetPan);
            }
            return;
         }
         CYTOinstance.fit(targetNodes, 84);
         return;
      }
   }
   const targetZoom = 0.8;
   const targetPan  = CYTOdefaultPanForZoom(targetZoom);
   if(preserveYPanOnClear) {
      const currentPan = CYTOinstance.pan();
      targetPan.y = Number(currentPan.y);
   }
   if(animate) {
      CYTOinstance.animate({
         zoom: targetZoom,
         pan : targetPan
      }, {
         duration: 180,
         easing: 'ease-out-cubic'
      });
      return;
   }
   CYTOinstance.zoom(targetZoom);
   CYTOinstance.pan(targetPan);
}

function CYTOhasVisibleTargetNodes(targetNodes) {
   if(!CYTOinstance || !targetNodes || !targetNodes.length) { return false; }
   const extent = CYTOinstance.extent();
   const nodes  = typeof targetNodes.toArray === 'function' ? targetNodes.toArray() : [];
   for(let i = 0; i < nodes.length; i++) {
      const bb = nodes[i].boundingBox();
      const outside =
         bb.x2 < extent.x1 ||
         bb.x1 > extent.x2 ||
         bb.y2 < extent.y1 ||
         bb.y1 > extent.y2;
      if(!outside) { return true; }
   }
   return false;
}

function CYTOviewportFilterKey() {
   const checkedOnly = DASHshowOnlyCheckedRows() ? '1' : '0';
   const checked     = checkedOnly === '1' ? DASHlistOfCheckedComponents().slice().sort().join(',') : '';
   return `${PAGEhashParameter('filter') || ''}|${PAGEhashParameter('whole_word') || ''}|${checkedOnly}|${checked}`;
}

function CYTOdefaultPadX() {
   return CYTOviewportPaddingDefault.x;
}

function CYTOsearchInputTop() {
   const container = document.querySelector('.CYTOcontainer');
   const searchBox = document.querySelector('.DASHsearchInput');
   if(!container || !searchBox) { return null; }

   const containerRect = container.getBoundingClientRect();
   const searchRect    = searchBox.getBoundingClientRect();
   const searchTop     = searchRect.top - containerRect.top;
   if(!Number.isFinite(searchTop)) { return null; }
   return {
      top   : searchTop,
      height: searchRect.height
   };
}

function CYTOdefaultPadY(zoom = CYTOinstance?.zoom?.() || 1) {
   const searchBox = CYTOsearchInputTop();
   if(searchBox === null) { return CYTOviewportPaddingDefault.y; }
   const renderedNodeHeight = 40 * zoom;
   return Math.round(searchBox.top + ((searchBox.height - renderedNodeHeight) / 2));
}

function CYTOdefaultPanForZoom(zoom) {
   const bb   = CYTOinstance.elements().boundingBox();
   return {
      x: -bb.x1 * zoom + CYTOdefaultPadX(),
      y: -bb.y1 * zoom + CYTOdefaultPadY(zoom)
   };
}

function CYTOalignTop() {
   const bb   = CYTOinstance.elements().boundingBox();
   const zoom = CYTOinstance.zoom();
   const pan  = CYTOinstance.pan();
   pan.y      = -bb.y1 * zoom + CYTOdefaultPadY(zoom);
   CYTOinstance.pan(pan);
}

function CYTOalignLeft() {
   const bb        = CYTOinstance.elements().boundingBox();
   const zoom      = CYTOinstance.zoom();
   const pan       = CYTOinstance.pan();
   pan.x           = -bb.x1 * zoom + CYTOdefaultPadX();
   CYTOinstance.pan(pan);
}

// returns the node object, null if DNE
function CYTOhover(nodeId) {
   if(!CYTOinstance) { return; }
   CYTOremoveHovered();
   const node = CYTOinstance.nodes().getElementById(nodeId);
   if(node && node.length !== 0) { node.addClass('CYTOHovered'); }
   return node;
}

function CYTOnoComps() {
   if(CYTOshouldShowWelcome()) {
      CYTOremoveToolbar();
      CYTOwelcomeRender();
      CYTOdestroy();
      return true;
   }
   return false;
}

function CYTOcosLayout(overrides = {}) {
   return {
      name                       : 'cose',
      directed                   : true,
      padding                    : 40,
      componentSpacing           : 40,
      nodeRepulsion              : 2000,
      nodeOverlap                : 20,
      edgeElasticity             : 100,
      gravity                    : 0.25,
      nodeDimensionsIncludeLabels: true,
      idealEdgeLength            : 30,
      animate                    : false,
      ...overrides
   };
}
