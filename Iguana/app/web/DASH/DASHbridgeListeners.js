const DASHbridge = (() => {
   let instance;
   return () => instance ??= BRIDGEinit('CYTO-DASH', ['dashToCyto', 'cytoToDash']);
})();

function DASHbindCytoBridgeListeners() {
   DASHbridge().cytoToDash.bind({
      onHover      : DASHhandleCytoBridgeHover,
      onHoverClear : DASHhandleCytoBridgeHoverClear,
      onNavigate   : DASHhandleCytoBridgeNavigate,
      onSelect     : DASHhandleCytoBridgeSelect,
      onUnselect   : DASHhandleCytoBridgeUnselect,
      onCheckedSet : DASHhandleCytoBridgeCheckedSet
   });
}

function DASHbridgeHoverEmit(guid)    { DASHbridge().dashToCyto.emit('onHover', guid); }
function DASHbridgeHoverClearEmit()   { DASHbridge().dashToCyto.emit('onHoverClear'); }
function DASHbridgeNavigateEmit(guid) { DASHbridge().dashToCyto.emit('onNavigate', guid); }
function DASHbridgeSelectEmit(guid)   { DASHbridge().dashToCyto.emit('onSelect', guid); }
function DASHbridgeUnselectEmit(guid) { DASHbridge().dashToCyto.emit('onUnselect', guid); }
function DASHbridgeCheckedSetEmit(guids) { DASHbridge().dashToCyto.emit('onCheckedSet', guids); }

function DASHhandleCytoBridgeHover(guid) {
   DASHcomponentRemoveHover();
   if(!guid) { return; }
   let componentRow = DASHrowSelector(guid);
   if(componentRow) { componentRow.classList.add('DASHrowHighlighted'); }
}

function DASHhandleCytoBridgeHoverClear() { DASHcomponentRemoveHover(); }

function DASHhandleCytoBridgeNavigate() {}

function DASHhandleCytoBridgeSelect(guid) {
   if(!guid) { return DASHhandleCytoBridgeCheckedSet([]); }
   const nextChecked = [...new Set([...DASHlistOfCheckedComponents(), guid])];
   DASHhandleCytoBridgeCheckedSet(nextChecked);
}

function DASHhandleCytoBridgeUnselect(guid) {
   const nextChecked = guid ? DASHlistOfCheckedComponents().filter(componentGuid => componentGuid !== guid) : [];
   DASHhandleCytoBridgeCheckedSet(nextChecked);
}

function DASHhandleCytoBridgeCheckedSet(guids = []) {
   DASHscrollToComponent = Array.isArray(guids) ? guids.filter(Boolean) : [];
   DASHclearSelectedRows();
   DASHsetCheckedComponents(guids, {emitToCyto: false});
}
