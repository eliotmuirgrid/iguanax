function ACTrevertFileConfirm(DiffCard, CurrentTreeNode) {
   POPpopoutClose(DiffCard);
   let Popout = POPpopoutOpen({
      title: `Revert File`, 
      content: ACTconfirmRevertHtml(CurrentTreeNode.path), 
      esc_close: true,
      width: "600px",
      callback : TRANSsetCode
   });
   document.querySelector('.ACTconfirmRevert').addEventListener('click', function() {
      POPpopoutClose(Popout);
      console.log(CurrentTreeNode);
      TGITopenBranchTerminal (ACTrevertFileGitCommand(CurrentTreeNode) );
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
   document.querySelector('.ACTcancelRevert').addEventListener('click', function()  {
      POPpopoutClose(Popout);
      MENUviewChanges(CurrentTreeNode);
   });
}

function ACTrevertFileGitCommand(CurrentTreeNode){
   let Path = CurrentTreeNode.path;
   let ParentFolder = ACTparentFolder(Path);
   let ParentDirPath = ACTremoveLastFileSep(ParentFolder);
   let Command = `git${ParentFolder !== "" ? ` -C "${ParentDirPath}"` : ""}`;
   TERMexecuteCommand([`${Command} checkout -- "${CurrentTreeNode.name}"`])
}

function ACTconfirmRevertHtml(File){
  return /*html*/`
  <div>The following file will have it's changes reverted:<div>
  <br>
  <div class="ACTfolderNameDiv">
     <div class="ACTfolderNamePrefix">${ESChtmlEscape(File)}</div>
  </div>
  <br>
  <br>
   <div class='FORMlabel'>Are you sure you want to revert the changes to this file?</div>
  <br>
  <br>
   <div class='TGITfileRevertConfirm BUTTONgroup'>
      <div class='ACTcancelRevert BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='ACTconfirmRevert BUTTONstandard BUTTONaction'>CONFIRM</div>
   </div>`;
}