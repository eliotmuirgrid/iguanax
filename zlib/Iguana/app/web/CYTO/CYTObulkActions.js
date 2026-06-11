const CYTObulkAction = Object.freeze({
   start      : CYTObulkStart,
   stop       : CYTObulkStop,
   connect    : CYTObulkConnect,
   editTags   : CYTObulkEditTags,
   clearQueue : CYTObulkClearQueue,
   clearErrors: CYTObulkClearErrors,
   delete     : CYTObulkDelete
});

function CYTOresolveBulkComponentIds(ComponentGuids = []) {
   if(Array.isArray(ComponentGuids) && ComponentGuids.length > 0) { return ComponentGuids; }
   return CYTOgetMarkedNodeIds();
}

function CYTOcheckBulkPermission(Perm, Components) {
   const Data = DASHuserPermissionAll();
   return Components.every(guid => Data[guid] && Data[guid][Perm]);
}

function CYTObulkActionDialog(WindowName, Action, Action2, ComponentGuids, Callback = function() {}) {
   const Card = POPpopoutOpen({
      title    : WindowName,
      content  : DASHbulkActionHtml(Action, Action2, ComponentGuids),
      className: 'DASHbulkActionPopup',
      esc_close: true
   });
   document.querySelector('.CARDcancelBulk').addEventListener('click', function() { POPpopoutClose(Card); });
   document.querySelector('.CARDconfirmBulk').addEventListener('click', function() { Callback(); POPpopoutClose(Card); });
}

function CYTObulkConnect(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   CYTOenterConnectMode(markedNodeIds);
   CYTOshowTargetSearch(markedNodeIds);
}

function CYTObulkStart(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   DASHbulkStartModal(markedNodeIds);
}

function CYTObulkStop(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   DASHbulkStopModal(markedNodeIds);
}

function CYTObulkDelete(ComponentGuids) {
   const markedNodes = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!DASHhasBulkPermission("delete_component", "Delete Component", markedNodes)) { return; }
   if(DASHcomponentListCanBeDeleted(markedNodes)) {
      CYTObulkActionDialog("Confirm Bulk Deletion", "DELETION", "delete", markedNodes, () => { DASHdeleteComponent(markedNodes); });
   } else {
      SNCKsnackbar("error", "Bulk delete failed: Stop components before deleting.");
   }
}

function CYTObulkEditTags(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   if(markedNodeIds.length === 1) { return CYTOeditTagsDialog(markedNodeIds[0]); }
   DASHbulkEditModal(markedNodeIds);
}

function CYTObulkClearQueue(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   DASHbulkActionDialog("Bulk Clear Queue", "CLEARING THE QUEUE", "clear the queue of", markedNodeIds, () => { DASHclearQueue(markedNodeIds); });
}

function CYTObulkClearErrors(ComponentGuids) {
   const markedNodeIds = CYTOresolveBulkComponentIds(ComponentGuids);
   if(!markedNodeIds || markedNodeIds.length === 0) { return; }
   DASHbulkClearErrorsModal(markedNodeIds);
}
