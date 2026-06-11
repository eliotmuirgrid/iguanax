const CYTObridge = (() => {
   let instance;
   return () => instance ??= BRIDGEinit('CYTO-DASH', ['dashToCyto', 'cytoToDash']); // ??= ensures BRIDGEinit only runs once, otherwise returns cached instance
})();

function CYTObindDashBridgeListeners() {
   CYTObridge().dashToCyto.bind({
      onHover      : CYTOhandleDashHoverEvent,
      onHoverClear : CYTOhandleDashHoverClearEvent,
      onNavigate   : CYTOhandleDashNavigateEvent,
      onSelect     : CYTOhandleDashSelectEvent,
      onUnselect   : CYTOhandleDashUnselectEvent,
      onCheckedSet : CYTOhandleDashCheckedSetEvent
   });
}

function CYTObridgeHoverEmit(guid)    { CYTObridge().cytoToDash.emit('onHover', guid); }
function CYTObridgeHoverClearEmit()   { CYTObridge().cytoToDash.emit('onHoverClear'); }
function CYTObridgeSelectEmit(guid)   { CYTObridge().cytoToDash.emit('onSelect', guid); }
function CYTObridgeUnselectEmit(guid) { CYTObridge().cytoToDash.emit('onUnselect', guid); }
function CYTObridgeCheckedSetEmit(guids) { CYTObridge().cytoToDash.emit('onCheckedSet', guids); }

function CYTOhandleDashHoverEvent(guid) {
   if(!CYTOinstance) { return; }
   CYTOremoveHovered();
   if(!guid) { return; }
   const node = CYTOgetNodeById(guid);
   if(node && node.length !== 0) { node.addClass('CYTOHovered'); }
}

function CYTOhandleDashHoverClearEvent() { CYTOremoveHovered(); }

function CYTOhandleDashNavigateEvent(guid) {
   if(!CYTOinstance || !guid) { return; }
   CYTOcenterOnNode(guid);
   if(PAGEhash() === 'dashboard/details' && PAGEhashParameter('component') === guid) {
      CYTOinstance.lastFocusedCardComponentId = guid;
   }
}

function CYTOsetMarkedNodes(guids = []) {
   if(!CYTOinstance) { return; }
   const markedIds = new Set(Array.isArray(guids) ? guids : [guids]);
   CYTOinstance.nodes('.CYTOMarked').forEach(node => {
      if(!markedIds.has(node.id())) { node.removeClass('CYTOMarked'); }
   });
   markedIds.forEach(guid => {
      const node = CYTOgetNodeById(guid);
      if(node && node.length !== 0) { node.addClass('CYTOMarked'); }
   });
   CYTOtoolbarSyncState();
}

function CYTOhandleDashSelectEvent(guid) {
   if(!CYTOinstance) { return; }
   CYTOsetMarkedNodes(guid ? [guid] : []);
   if(guid) { CYTOcenterOnNode(guid); }
}

function CYTOhandleDashUnselectEvent(guid) {
   if(!CYTOinstance) { return; }
   if(!guid) { return CYTOsetMarkedNodes([]); }
   CYTOsetMarkedNodes(CYTOgetMarkedNodeIds().filter(nodeGuid => nodeGuid !== guid));
}

function CYTOhandleDashCheckedSetEvent(guids = []) { CYTOsetMarkedNodes(guids); }
