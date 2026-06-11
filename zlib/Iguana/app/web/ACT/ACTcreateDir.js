

function ACTcreateDirHtml(path) {
   let FolderInHtml = ACTfolderWrapperHtml(path, `<input class='FORMinput ACTfolderName'/>`);
   return /*html*/`
<div class='FORMlabel'>Folder Name:</div>
<br>
${FolderInHtml}
<p class='ACTcreateFolderFeedback hint' ></p>
<br/>
<br/>
<div class='BUTTONgroup ACTbuttons'>
   <div class='ACTnewFolderCancel BUTTONstandard BUTTONcancel'>CANCEL</div>
   <div class='ACTnewFolderButton BUTTONstandard BUTTONaction'>CREATE</div>
</div>`;
}

function ACTcreateDirWindow(CurrentTreeNode) {
   let Popout = POPpopoutOpen({
      title:'New Folder',
      content: ACTcreateDirHtml(CurrentTreeNode.path),
      width: '600px',
      esc_close : true,
      callback : TRANSsetCode
   });
   UTLdisableButtonEl(document.querySelector('.ACTnewFolderButton'));
   document.querySelector('.ACTfolderName').addEventListener("keyup", function(e){ ACTcheckFolderName(e,CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTfolderName').addEventListener("input", function(e){ ACTcheckFolderName(e,CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTnewFolderButton').addEventListener("click", function() { ACTcreateFolder(CurrentTreeNode.path, Popout); });
   document.querySelector('.ACTnewFolderCancel').addEventListener("click", function() { POPpopoutClose(Popout); });
   document.querySelector('.ACTfolderName').focus();
   MENUremoveTooltip();
}

function ACTcheckFolderName(e, Path, Popout){
   let el = document.querySelector('.ACTfolderName');
   if(el.value.trim() === "") UTLdisableButtonEl(document.querySelector('.ACTnewFolderButton'));
   if(el.value.trim() !== "") UTLenableButtonEl (document.querySelector('.ACTnewFolderButton'));
   if( e.key === "Enter" && document.querySelector('.ACTnewFolderButton.BUTTONaction')) ACTcreateFolder(Path, Popout);
}

function ACTcreateFolder(NewFolderLocation, Popout){
   if(document.querySelector('.ACTnewFolderButton.BUTTONdisabled')) return;
   let FolderName = document.querySelector('.ACTfolderName').value.trim();
   let Path = (NewFolderLocation ? NewFolderLocation + FILsep() : "") + FolderName + FILsep();
   let data = { path: Path, component: TRANScomponentGuid(), development : TRANSdevelopment() };
   APIcall('component/dir/add', data, function(Response){
      if (!Response.success ){
         document.querySelector('.ACTcreateFolderFeedback').innerHTML = "Error - " + Response.error;
         return;
      }
      POPpopoutClose(Popout);
      TRANSopenDir = Path;
      TRANSopenMenu = Path;
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
      MODELforcePoll('TRANSmodel');
   });
}