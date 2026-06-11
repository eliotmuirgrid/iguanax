function DASHclearErrorsBulk(ComponentGuids = DASHlistOfCheckedComponents()){
   APIcall("component/clear_error", {'components': ComponentGuids}, function(Response){
      if(!Response.success){
         SNCKsnackbar('error', 'Could not clear errors: ' + Response.error);
         return;
      }
      let Success = Response.data.cleared.length === ComponentGuids.length;
      if(Success){
         SNCKsnackbar('success', Response.data.cleared.length + (Response.data.cleared.length == 1 ? ' component ' : ' components ') + 'had their errors cleared.', 10000);
      }
      else{
         if(Response.data.cleared.length){
            SNCKsnackbar('warning', `Only ${Response.data.cleared.length} ${(Response.data.cleared.length == 1 ? ' component ' : ' components ')} of requested ${ComponentGuids.length} had their errors cleared.`, 10000);
         }
         else{
            SNCKsnackbar('error', 'No components had their errors cleared.', 10000);
         }
      }
   });
}

function DASHbulkClearErrorsModal(ComponentGuids) {
   let Card = POPpopoutOpen({title: 'Bulk Clear Errors', content: DASHbulkClearErrorsHtml(ComponentGuids), className: 'DASHbulkActionPopup',callback: DASHresetBulkMenu, esc_close : true});
   document.querySelector('.DASHcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.DASHsaveTags').addEventListener('click', function() { DASHclearErrorsBulk(ComponentGuids); POPpopoutClose(Card);})
}

function DASHbulkClearErrorsHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkEditSubmitFeedback"></div>
   ${DASHcomponentTable('CLEARING ERRORS', ComponentGuids)}
   <br>
   <div class="DASHbulkDialogConfirm">Would you like to clear the errors of every component selected?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='DASHcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DASHsaveTags BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}
