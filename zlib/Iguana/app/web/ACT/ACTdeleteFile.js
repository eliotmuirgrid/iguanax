function ACTdeleteFileConfirmHtml(Location) { 
   return /*html*/`
   <div>The following file will be deleted:<div>
   <br>
   <div class="ACTfolderNameDiv">
      <div class="ACTfolderNamePrefix">${ESChtmlEscape(Location)}</div>
   </div>
   <br>
   <br>
   <b>Are you sure you want to delete this file?</b>
   <br>
   <br>
   <div class='BUTTONgroup ACTdeleteButtons'>
      <div class='ACTclose BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='ACTdelete BUTTONstandard BUTTONaction'>DELETE</div>
   </div>`;
}

function ACTdeleteFileWindow(DeleteFilePath) {
   let Card = POPpopoutOpen({
      title: 'Delete File', 
      content: ACTdeleteFileConfirmHtml(DeleteFilePath), 
      width : "500px",
      esc_close : true,
      callback : TRANSsetCode
   });
   document.querySelector('.ACTdelete').addEventListener('click', function(){ ACTdeleteApiCall(DeleteFilePath, Card); });
   document.querySelector('.ACTclose') .addEventListener('click', function(){ POPpopoutClose(Card); });
   MENUremoveTooltip();
}

function ACTdeleteApiCall(FilePath, Popout){
   POPpopoutClose(Popout);
   APIcall('component/file/delete', { path : FilePath, component : TRANScomponentGuid(), development : TRANSdevelopment()}, function(Response){
      if(!Response.success) return SNCKsnackbar('error', "Could not delete file: " + Response['error']);
      TRANSfileCurrentReload();
      if (TRANSfile().includes(FilePath)) TRANSfileChangeWithoutSaving("main.lua");
      else                                MODELforcePoll('TRANSmodel');
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}