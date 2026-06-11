function ACTdeleteDirConfirmHtml(Path){
   return /*html*/`
   <div>The folder and all the sub-directories and files will be deleted:</div>
   <br>
   <div class="ACTfolderNameDiv">
      <div class="ACTfolderNamePrefix">${ESChtmlEscape(Path + FILsep())}</div>
   </div>
   <br>
   <br>
   <b>Are you sure you want to delete them?</b>
   <br>
   <br>
   <br>
   <div class="BUTTONgroup ACTdeleteButtons">
      <div class='ACTdeleteCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='ACTdelete BUTTONaction BUTTONstandard'>DELETE</div>
   </div>`;
}

function ACTdeleteDirWindow(DeleteFilePath) {
   let Content = ACTdeleteDirConfirmHtml(DeleteFilePath);
   let Popout = POPpopoutOpen({
      title:'Delete Folder', 
      content: Content, 
      width : '450px',
      esc_close : true,
      callback : TRANSsetCode

   });
   document.querySelector('.ACTdelete')      .addEventListener('click', function() { ACTdeleteDirCall(DeleteFilePath, Popout); });
   document.querySelector('.ACTdeleteCancel').addEventListener('click', function() { POPpopoutClose(Popout); });
   MENUremoveTooltip();
}

function ACTdeleteDirCall(DeleteFilePath, Popout) {
   POPpopoutClose(Popout);
   APIcall('component/dir/delete', { path : DeleteFilePath, component : TRANScomponentGuid(), development : TRANSdevelopment()}, function(Response){
      if(!Response.success) return SNCKsnackbar('error', Response['error'] + ' Cannot delete directory.');
      TRANSfileCurrentReload();
      if (TRANSfile().includes(DeleteFilePath)) TRANSfileChangeWithoutSaving("main.lua");
      else                                      MODELforcePoll('TRANSmodel');
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}
