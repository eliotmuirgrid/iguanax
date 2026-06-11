function DASHbulkGetCheckedList(){
   let CheckedRows = document.querySelectorAll('.DASHselectRow:checked');
   let ComponentGuids = [];
   for (let i = 0; i < CheckedRows.length; i++) ComponentGuids.push( DASHrowGuid( DASHrowIndex( CheckedRows[i].closest('.DASHcomponentRow') ) ) );
   return ComponentGuids;
}

function DASHbulkActions() {
   let Action = document.querySelector('.DASHselectMenu').value;
   let Components = DASHbulkGetCheckedList();
   if(Action === 'DASHselectStart' && DASHhasBulkPermission("start", "Start Component", Components)) {
      DASHbulkStartModal(DASHbulkGetCheckedList());
   }
   if(Action === 'DASHselectStop' && DASHhasBulkPermission("stop", "Stop Component", Components)) {
      DASHbulkStopModal(DASHbulkGetCheckedList());
   }
   if(Action === 'DASHselectDelete' && DASHhasBulkPermission("delete_component", "Delete Component", Components)) {
      DASHdeleteSelected(DASHbulkGetCheckedList());
   }
   if(Action === 'DASHselectClearErrors' && DASHhasBulkPermission("clear_errors", "Clear Errors", Components)) {
      DASHbulkClearErrorsModal(DASHbulkGetCheckedList());
   }
   if(Action === 'DASHselectEdit' && DASHhasBulkPermission("edit_tags", "Edit Tags", Components)) {
      DASHbulkEditModal(DASHbulkGetCheckedList());
   }
   if(Action === 'DASHselectClearQueue' && DASHhasBulkPermission("clear_queue", "Clear Queue", Components)) {
      DASHbulkActionDialog("Bulk Clear Queue", "CLEARING THE QUEUE", "clear the queue of", DASHbulkGetCheckedList(), DASHclearQueue);
   }
   DASHresetBulkMenu();
   MODELforcePoll('DASHmodel');
}

function DASHhasBulkPermission(Perm, String, Components) {
   let Data = DASHuserPermissionAll();
   for(guid of Components) {
      if(!Data[guid] || !Data[guid][Perm]) {
         SNCKsnackbar("error", `You do not have '${String}' permission for all the components selected.`);
         return false;
      }
   }
   return true;
}

function DASHgetName(guid) {
   let Data = DASHlistData();
   for (let i = 0; i < Data.length; i++) {
       if (Data[i].guid === guid) {
           return Data[i].name;
       }
   }
   return null;
}

function DASHgetTags(Guid){
   let Data = DASHlistData();
   for (let i = 0; i < Data.length; i++) {
      if (Data[i].guid === Guid) {
          return Data[i].tags;
      }
  }
  return null;
}

function DASHgenerateComponentHtml(ComponentGuids) {
   let ComponentNames = [];
   for (let i = 0; i < ComponentGuids.length; i++) {
      let Tags = DASHgetTags(ComponentGuids[i]);
      let Html = `<div class="DASHbulkListName">${DASHgetName(ComponentGuids[i])}</div>  <div>${HASHTAGrenderTags(Tags)}</div>`;
      ComponentNames.push(Html);
   }

   let ComponentTable = '<div class="DASHcomponentList">';
   for (let i = 0; i < ComponentNames.length; i++) {
      ComponentTable += '<div>' + ComponentNames[i] + '</div>';
   }
   ComponentTable += '</div>';
   
   return ComponentTable;
}

function DASHcomponentTable(Action, ComponentGuids) {
   let Count = ComponentGuids?.length || document.querySelectorAll('.DASHselectRow:checked').length;
   let Card = document.querySelector('.CARDdetailedCard');
   let ComponentTable;
   ComponentTable = ComponentGuids?.length ? DASHgenerateComponentHtml(ComponentGuids) : (Card ? CARDgenerateComponentHtml() : DASHgenerateComponentHtml(ComponentGuids));
   return /*html*/`
   <div class="DASHcomponents">
      <div class="DASHbulkDialogHeader">CONFIRM ${Action} OF ${Count > 1 ? `COMPONENTS: (${Count} TOTAL)` : `COMPONENT:`}</div>
      ${ComponentTable}
   </div>`;
}

function DASHbulkActionHtml(Action, Action2, ComponentGuids) {
   return /*html*/`
   ${DASHcomponentTable(Action, ComponentGuids)}
   <br>
   <div class="DASHbulkDialogConfirm">Would you like to ${Action2} every component selected?</div>
   ${Action === 'DELETION' ? '<br><div>They will be moved the archived directory.</div>' : ''}
   <div class="DASHbuttonGroup BUTTONgroup">
      <div class="CARDcancelBulk BUTTONcancel BUTTONstandard">CANCEL</div>
      <div class="CARDconfirmBulk BUTTONaction BUTTONstandard">SUBMIT</div>
   </div>`;
}

function DASHbulkActionDialog(WindowName, Action, Action2, ComponentGuids, Callback = function(){}) {
   let Card = POPpopoutOpen({title: WindowName, content: DASHbulkActionHtml(Action, Action2, ComponentGuids), className: 'DASHbulkActionPopup',callback: DASHresetBulkMenu, esc_close : true});
   document.querySelector('.CARDcancelBulk') .addEventListener('click', function(){ POPpopoutClose(Card); } );
   document.querySelector('.CARDconfirmBulk').addEventListener('click', function(){ Callback(); POPpopoutClose(Card); } );
}

function DASHresetBulkMenu(){ document.querySelector('.DASHselectMenu').value = 'DASHselect'; }
