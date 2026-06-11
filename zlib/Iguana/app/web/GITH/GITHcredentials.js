let GITHcredCard;

function GITHupdateCreds(){
   let Data = GITHcredentials();
   GITHupdateValue(document.querySelector(".GITHname"),  ESChtmlEscape(Data?.name) || GITHnotSet());
   GITHupdateValue(document.querySelector(".GITHemail"), ESChtmlEscape(Data?.email) || GITHnotSet());
}

function GITHusernameAndEmailHtml(){
   return /*html*/`
   <div class="GITHbox GITHcredentials">
      <div class="GITHtitleRow">
         <div class="GITHtitle GITHgitCredTitle">Author Info</div>
         <div class="GITHdesc GITHdescCreds">These are needed to label your commits.
         [<a class="SETlink" target="_blank" href="${LINKsettingsGitCredentials}">...</a>]
         </div>
         <div class="GITHeditGitCred BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">Name:</div>
         <div class="GITHtableVal GITHname"></div>
         <div class="GITHtableTitle">Email:</div>
         <div class="GITHtableVal GITHemail"></div>
      </div>
   </div>`;
}

function GITHeditCredentialsHtml(){
   let Data = GITHcredentials();
   let Name = Data?.name || "";
   let Email = Data?.email || "";
   return /*html*/`
   <div class="GITHcredentialsEdit">
      <div class="GITHtableEdit">
         <div class="GITHtableTitle">Name:</div>
         <input class="GITHtableVal GITHeditName FORMinput" value="${ESChtmlEscape(Name)}">
         <div class="GITHtableTitle">Email:</div>
         <input class="GITHtableVal GITHeditEmail FORMinput" value="${ESChtmlEscape(Email)}">
      </div>
      <div class="GITHerror GITHcredentialsError"></div>
      <div class="BUTTONgroup GITHbutton">
         <div class="GITHcredsCancel BUTTONstandard BUTTONcancel">CANCEL</div>
         <div class="GITHcredsSave BUTTONstandard BUTTONaction">SAVE</div>
      </div>
   </div>`;
}

function GITHcredentialsError(Error){
   document.querySelector('.GITHcredentialsError').innerHTML = Error;
}

function GITHsetCredentialsPopout(){
   GITHcredCard = POPpopoutOpen({
      title:"Set Author Info",
      width: "650px",
      content: GITHeditCredentialsHtml()
   });
   FORMfocus(document.querySelector(".GITHeditName"));
   FORMenterListener(document.querySelector(".GITHcredentialsEdit"), GITHsetCredentials);
   document.querySelector(".GITHcredsSave")  .addEventListener("click", function(){ GITHsetCredentials();});
   document.querySelector(".GITHcredsCancel").addEventListener("click", function(){ POPpopoutClose(GITHcredCard); });
}

function GITHextractCredentials(){
   let Data = {};
   Data.name  = document.querySelector('.GITHeditName').value.trim();
   Data.email = document.querySelector('.GITHeditEmail').value.trim();
   return Data;
}

function GITHsetCredentials(){
   let Data = GITHextractCredentials();
   APIcall('git/credentials/set', Data, function(R){
      if (!R.success) return GITHcredentialsError(R.error);
      let Message = GINsetCredentials();
      GINaddAndCommit({message: Message, users: true});
      POPpopoutClose(GITHcredCard);
      MODELforcePoll("GITHmodel");
      MODELforcePoll("GITHslowModel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
   });
}

function GITHcredWarning(){
   let Data = GITHcredentials();
   return !Data?.name?.length || !Data?.email?.length;
}