function STUlisteners(){
   let div = document.querySelector(".SETmainContent");
   if(!div) return;
   div.addEventListener("click", STUclickListener)
}

function STUclickListener(e){
   if(e.target.closest(".BUTTONdisabled"))       return;
   if(e.target.closest(".LOGcomponentClearLog")) return STUclearComponentLogs(e);
   if(e.target.closest(".STUclearArchive"))      return STUarchiveRemoveListener();
   if(e.target.closest(".STUchange"))            return STUchange();
   if(e.target.closest(".STUchangeBackup"))      return STUchangeBackup();
}

function STUclearComponentLogs(e) {
   let ComponentName = e.target.closest(".SETloggingRow").querySelector('.STGcomponentIdOnTable').innerText.trim();
   let SourceData = STUcomponentData();
   let ComponentId = STGgetGuidFromName(ComponentName, SourceData);
   CONFIRMaction(STUclearLogsHtml(ComponentName), function() {
      APIcall("log/component/clear", {component : ComponentId}, function(R) {
         let DeleteSuccess = R.data.before_time != "";
         let Message = DeleteSuccess ? `Deleted all log files before ${R.data.before_time}` : `No logs to delete`;
         SNCKsnackbar("success", Message, 10000);
         STUrender();
      });
   }, "DELETE");
}

function STUclearLogsHtml(ComponentName){
   return /*html*/`
   IguanaX will delete the logs that can be safely removed.
   <br/>
   <br/>
   Are you certain you wish to delete the logs for the <b>${ComponentName}</b>?
   <br>
   <br>
   <b>Warning:</b> This action cannot be reversed.`;
}

function STUarchiveRemoveListener() {
   let Card = POPpopoutOpen({
      title    : "Clear archive?",
      content  : STUcreateArchiveRemoveTable(),
      className: 'DASHbulkActionPopup',
      esc_close: true
   });
   document.querySelector('.CARDcancelBulk').addEventListener('click', function(){ POPpopoutClose(Card); } );
   document.querySelector('.CARDconfirmBulk').addEventListener('click', function(){
      APIcall('component/archive_remove', {}, (R) => {
         POPpopoutClose(Card);
         if(!R?.success) return SNCKsnackbar('error', R?.error || "Could not clear the archive. ", 20000);
         SNCKsnackbar('success', "Archive cleared.", 20000);
         MODELforcePoll('STUmodel');
      })
   });
}

function STUcreateArchiveRemoveTable() {
   const Archived= STUarchiveData().components;
   let Data = STUarchiveData();
   let Val = Data?.size || undefined;
   let Size = STUfuzzySize(Val);
   let Path = Data?.dir || "";
   const STUcreateArchivedTable = () => {
      return `<div class="DASHcomponentList">${Archived.map(component => `<div>${component}</div>`).join('')}</div>`;
   };
   let html = STUcreateArchivedTable();
   return /*html*/`
      <div>This will permanently delete <b>${Archived.length} archived components and their log data</b> (${Size} total). This action cannot be undone.</div>
      <br>
      <div class="STUarchivePathDialog">Archive path: ${Path}</div>
      <br>
      <div class="DASHcomponents">
      <div class="DASHbulkDialogHeader">Component${Archived.length > 1 ? "s" : ""} to delete (${Archived.length}):</div>
      ${Archived.length ? html : "<div class='DASHcomponentList'><div><i>No components in the archive.</i></div></div>"}
   </div>
   <br>
   <div class="DASHbuttonGroup BUTTONgroup">
      <div class="CARDcancelBulk BUTTONcancel BUTTONstandard">CANCEL</div>
      <div class="CARDconfirmBulk BUTTONaction BUTTONwarning BUTTONstandard">DELETE ${Archived.length}</div>
   </div>`;
}