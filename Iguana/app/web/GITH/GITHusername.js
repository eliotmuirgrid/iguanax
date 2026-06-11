let GITHsetUsernameEdit;

function GITHupdateUsername(){
   let Username = GITHgitUsername();
   GITHupdateValue(document.querySelector(".GITHusername"), ESChtmlEscape(Username) || GITHnotSet());
}

function GITHusernameHtml(){
   let Link = "";
   let LinkHtml = Link ? /*html*/`[<a class="SETlink" target="_blank" href="${Link}">...</a>]` : "";
   return /*html*/`
   <div class="GITHbox GITHserviceUser">
      <div class="GITHtitleRow">
         <div class="GITHtitle">Git Username</div>
         <div class="GITHdesc">Input Git Server username from account settings.
         ${LinkHtml}
         </div>
         <div class="GITHedit BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">Username:</div>
         <div class="GITHtableVal GITHusername"></div>
      </div>
   </div>`;
}

function GITHeditUsernameHtml(Service){
   let Username = GITHgitUsername();
   let HelpHtml = GITHusernameHelpHtml(Service);
   return /*html*/`
   <div class="GITHusernameEdit">
      ${HelpHtml}
      <div class="GITHtableTitle">${Service} Username:</div>
      <br>
      <input class="GITHtableVal GITHeditUsername FORMinput" value="${ESChtmlEscape(Username)}">
      <div class="GITHerror"></div>
      <div class="BUTTONgroup GITHbutton">
         <div class="GITHcancel BUTTONstandard BUTTONcancel">CANCEL</div>
         <div class="GITHsave BUTTONstandard BUTTONaction">SAVE</div>
      </div>
   </div>`;
}

function GITHusernameHelpHtml(Service){
   if(Service.includes("Server - Bitbucket")) return GITHbitbucketServerUsernameHelpHtml();
   if(Service.includes("Cloud - Bitbucket"))  return GITHbitbucketUsernameHelpHtml();
   if(Service.includes("Azure Devops"))       return GITHazureUsernameHelpHtml();
   return "";
}

function GITHsetUsernamePopout(){
   let Service = GITHgitProvider();
   GITHsetUsernameEdit = POPpopoutOpen({
      title:`Set Git Username`,
      width : "650px",
      content: GITHeditUsernameHtml(Service)
   });
   FORMfocus(document.querySelector(".GITHusernameEdit .FORMinput"));
   FORMenterListener(document.querySelector(".GITHusernameEdit"), GITHsetUsername);
   document.querySelector(".GITHusernameEdit .GITHsave")  .addEventListener("click", function(){ GITHsetUsername();});
   document.querySelector(".GITHusernameEdit .GITHlink")  .addEventListener("click", function(){ FORMfocus(document.querySelector(".GITHusernameEdit .FORMinput"));});
   document.querySelector(".GITHusernameEdit .GITHcancel").addEventListener("click", function(){ POPpopoutClose(GITHsetUsernameEdit); });
}

function GITHusernameWarning(){
   let Username = GITHgitUsername();
   return !Username?.length;
}

function GITHsetUsername(){
   let Data = {};
   Data.username = (document.querySelector(".GITHeditUsername")?.value || "").trim();
   APIcall("git/username/set", Data, function(R){
      if(!R.success) return GITHsetError(".GITHusernameEdit", R.error || "");
      let Message = GINsetUsername();
      GINaddAndCommit({message: Message, users: true});
      POPpopoutClose(GITHsetUsernameEdit);
      MODELforcePoll("GITHmodel");
      MODELforcePoll("GITHslowModel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
   });
}