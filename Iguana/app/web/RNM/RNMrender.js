let RNMcard;

function RNMrender(NewName, OldName){
   TRANSopenDir = NewName;
   MODELforcePoll('TRANSmodel');
   MODELforcePoll('TRANSstatusModel', function(){
      RNMunlockedCheck(NewName, OldName);
   });
}

function RNMunlockedCheck(NewName, OldName){
   if(!LIBlibraryUnlocked(NewName)) return RNMrenderLibraryLocked(NewName, OldName);
   RNMrenderContent(NewName, OldName);
}

function RNMrenderLibraryLocked(NewName, OldName){
   CONFIRMaction(/*html*/`To use the rename library tool, the <b>${NewName}</b> library must be unlocked.<br>
   <br>
   Do you want to unlock the library?`, function(){
      RNMrenderLibraryLockedExecute(NewName, OldName);
   }, "CONFIRM");
}

function RNMrenderLibraryLockedExecute(NewName, OldName){
   LIBunlock(NewName, NewName, function(){
      RNMrender(NewName, OldName);
   });
}

function RNMrenderContent(NewName, OldName){
   let Path  = NewName;
   let H = /*html*/`
   <div><b>This tool is optional:</b> it will rename files and file content to match the new prefix in the library:</div>
   <br>
   <div class="ACTfolderNameDiv">
      <div class="ACTfolderNamePrefix">${ESChtmlEscape(Path + FILsep())}</div>
   </div>
   <br>
   <br>
   <div>Do you want to rename the  files and content to match the new prefix?</div>
   <br>
   <div class="RNMrow">
      <div class="FORMlabel">Rename file prefix from: </div>
      <input class="RNMoldName FORMinput" value="${OldName}">
      <div class="FORMlabel">Rename file prefix to: </div>
      <input class="RNMnewName FORMinput" value="${NewName}">
   </div>
   <br>
   <div class="RNMerrorFeedback"></div>
   <br>
   <div class="RNMbuttons BUTTONgroup">
      <div class="RNMcancel BUTTONstandard BUTTONcancel">CANCEL</div>
      <div class="RNMconfirm BUTTONstandard BUTTONaction">CONFIRM</div>
   </div>
   `;
   RNMcard = POPpopoutOpen({title : "Rename Inner Files and Content", content : H, className : "RNMmodal"});
   RNMfocus();
   FORMenterListener(document.querySelector('.RNMnewName'), function(){ RNMconfirm(Path); } );
   FORMenterListener(document.querySelector('.RNMoldName'), function(){ RNMconfirm(Path); } );
   document.querySelector('.RNMconfirm').addEventListener('click', function(){ RNMconfirm(Path); });
   document.querySelector('.RNMcancel').addEventListener('click', function(){ RNMclose(); });
}

function RNMfocus(){
   let NewName = document.querySelector('.RNMnewName');
   let OldName = document.querySelector('.RNMoldName');
   if(!OldName.value) return FORMfocus(OldName);
   FORMfocus(NewName);
}

function RNMerrorFeedback(Error){
   let Feedback = document.querySelector('.RNMerrorFeedback');
   Feedback.innerHTML = Error;
}

function RNMconfirm(Path){
   let NewName = document.querySelector('.RNMnewName').value;
   let OldName = document.querySelector('.RNMoldName').value;
   let Development = TRANSdevelopment();
   let Component = TRANScomponentGuid();
   APIcall("component/library/rename/content", { component : Component, old_name : OldName, new_name : NewName, path : Path, development : Development }, function(R){
      if(!R.success) return RNMerrorFeedback(R?.error || "There was an error renaming the library");
      TRANSfileCurrentReload();
      RNMclose();
   });
}

function RNMclose(){
   POPpopoutClose(RNMcard);
}
