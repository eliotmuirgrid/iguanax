
// TODO - we are lacking a concept of keeping the row the user's mouse is on from moving around.
function DASHcomponentSwitchList(Direction, ComponentGuids = DASHbulkGetCheckedList()){
   let Command = Direction ? 'component/start':'component/stop'
   APIcall(
      Direction ? 'component/start':'component/stop',
      {'components': ComponentGuids,
       'tracked'   : true 
      },
      function(Response){
         let action1 = Direction ? 'start' : 'stop';
         let action2 = Direction ? 'started' : 'stopped';
         // TODO - this doesn't list off components which failed to start
         if(Direction && !LICcheckComponentStartError('component/start', Response, function(){ MODELforcePoll('DASHmodel'); })) { return; }
         if(Response.data.switched.length)  SNCKsnackbar('success','Bulk '+ action1 + " performed." , 10000);
         else                               SNCKsnackbar('error', 'No components ' + action2 + ".", 10000);
         if (Response?.data?.job_id){ TOGactivatePoller(Command, Response?.data?.job_id); }
         MODELforcePoll('DASHmodel');
      }
   );
}

function DASHbulkStartModal(ComponentGuids) {
   let Card = POPpopoutOpen({title: 'Bulk Start', content: DASHbulkStartHtml(ComponentGuids), className: 'DASHbulkActionPopup',callback: DASHresetBulkMenu, esc_close : true});
   document.querySelector('.DASHcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.DASHsaveTags').addEventListener('click', function() { DASHcomponentSwitchList(true, ComponentGuids); POPpopoutClose(Card);})
}

function DASHbulkStartHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkEditSubmitFeedback"></div>
   ${DASHcomponentTable('START', ComponentGuids)}
   <br>
   <div class="DASHbulkDialogConfirm">Would you like to start every component selected?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='DASHcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DASHsaveTags BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}

function DASHbulkStopModal(ComponentGuids) {
   let Card = POPpopoutOpen({title: 'Bulk Stop', content: DASHbulkStopHtml(ComponentGuids), className: 'DASHbulkActionPopup',callback: DASHresetBulkMenu, esc_close : true});
   document.querySelector('.DASHcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.DASHsaveTags').addEventListener('click', function() { DASHcomponentSwitchList(false, ComponentGuids); POPpopoutClose(Card);})
}

function DASHbulkStopHtml(ComponentGuids) {
   return /*html*/`
   <div class="DASHbulkEditSubmitFeedback"></div>
   ${DASHcomponentTable('STOP', ComponentGuids)}
   <br>
   <div class="DASHbulkDialogConfirm">Would you like to stop every component selected?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='DASHcancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DASHsaveTags BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}
