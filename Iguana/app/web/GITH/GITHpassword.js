let GITHtokenEdit;

function GITHscopeHint(){
   const Server = MODELdata?.GITHgitServer?.data?.service;
   if (Server.toLowerCase().includes("bitbucket")) {
      return " (Ensure token has read:user:bitbucket)";
    }
    return ""
}

function GITHtokenValid(){
   return /*html*/`
   <div class="GITHtokenMark">
      <div title="Successful token authentication with the Git account." class="GITHsuccessMark">****************</div>
   </div>`;
}

function GITHnoAccessToken(Message){
   return /*html*/`<div class="GITHnotSet">${Message}Please ensure that your internet connection is active and verify your firewall settings.</div>`;
}

function GITHinvalidToken(){
   return /*html*/`<div class="GITHnotSet">Invalid token set: Unable to authenticate with the Git account. Verify token has correct scopes and user has valid permissions.${GITHscopeHint()}</div>`;
}

function GITHdeprecatedToken(){ 
   return /*html*/`<div class="GITHnotSet">⚠️ <strong>You're using an app password that will be deprecated on September 9, 2025.</strong> Please click EDIT to switch to API tokens.</div>`;
}

function GITHmissingScopes(MissingScopes){
   return /*html*/`
   <div class="GITHtokenMark">
      <div title="Successful token authentication with the Git account." class="GITHsuccessMark">****************</div>
   </div>
   <div class="GITHnotSet"><strong>Token lacks required scopes, which may limit functionality:</strong>${MissingScopes}</div>`;
}

function GITHtokenDataHtml(Data){
   if(!Data?.set)    return GITHnotSet();
   if(!Data?.access) return GITHnoAccessToken(Data?.message ? Data?.message + ". ": "");
   if(!Data?.valid)  return GITHinvalidToken();
   if(Data?.message !== "") return GITHmissingScopes(Data?.message ? Data?.message + "": "");
   if(Data?.deprecated) return GITHdeprecatedToken();
   return GITHtokenValid();
}

function GITHupdateToken(){
   let Data = GITHgitTokenValid();
   GITHupdateValue(document.querySelector(".GITHtokenVal"), GITHtokenDataHtml(Data));
}

function GITHappPasswordHtml(Service){
   let Link = ""; //GITHserviceHelpLinks(Service);
   let LinkHtml = Link ? /*html*/`[<a class="SETlink" target="_blank" href="${Link}">...</a>]` : ""; 
   return /*html*/`
   <div class="GITHbox GITHappPassword">
      <div class="GITHtitleRow"> 
         <div class="GITHtitle">Server Token</div>
         <div class="GITHdesc">Generate and register Git Server token for Iguana.
         ${LinkHtml}
         </div>
         <div class="GITHedit BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">Token:</div>
         <div class="GITHtableVal GITHtokenVal">...</div>
      </div>
   </div>`;
}

function GITHeditAppPasswordHtml(Service){
   let HelpHtml = GITHappPasswordHelpHtml(Service);
   return /*html*/`
   <div class="GITHtokenEditModal">
      ${HelpHtml}
      <input class="GITHtableVal GITHeditToken FORMinput">
      <div class="GITHerror"></div>
      <div class="BUTTONgroup GITHbutton">
         <div class="GITHcancel BUTTONstandard BUTTONcancel">CANCEL</div>
         <div class="GITHsave BUTTONstandard BUTTONaction">SAVE</div>
      </div>   
   </div>`;
}

function GITHappPasswordHelpHtml(Service){
   if(Service == "Cloud - Azure Devops")  return GITHazureTokenHelpHtml();
   if(Service == "Server - Azure Devops") return GITHazureTokenHelpHtml();
   if(Service == "Cloud - Bitbucket")     return GITHbitbucketAppPasswordHelpHtml();
   if(Service == "Server - Bitbucket")    return GITHbitbucketServerAppPasswordHelpHtml();
   if(Service == "Cloud - GitLab")        return GITHgitLabTokenHelpHtml();
   if(Service == "Server - GitLab")       return GITHgitLabTokenHelpHtml();
   if(Service == "Cloud - GitHub")        return GITHgitHubTokenHelpHtml();
   if(Service == "Server - GitHub")       return GITHgitHubTokenHelpHtml();
   return GITHbitbucketAppPasswordHelpHtml();
}

function GITHtokenWidth(Service){
   if(Service.includes("GitHub")) return "1250px";
   return "750px";
}

function GITHsetAppPasswordPopout(){
   let Service = GITHgitProvider();
   GITHtokenEdit = POPpopoutOpen({
      title:`Set Server Token`, 
      width: GITHtokenWidth(Service),
      content: GITHeditAppPasswordHtml(Service),
      callback : function(){
         CORmodelUpdateSlow();
         // MODELforcePoll('GITHslowModel');
      }
   });
   let div = document.querySelector(".GITHtokenEditModal .FORMinput");
   if(Service == "Cloud - Bitbucket") div = div = document.querySelectorAll(".GITHtokenEditModal .FORMinput")[1];
   FORMfocus(div);
   FORMenterListener(document.querySelector(".GITHtokenEditModal"), GITHsetToken);
   document.querySelector(".GITHtokenEditModal .GITHsave")  ?.addEventListener("click", function(){ GITHsetToken();});
   document.querySelector(".GITHtokenEditModal .GITHlink")  ?.addEventListener("click", function(){ FORMfocus(document.querySelector(".GITHtokenEditModal .FORMinput"));});
   document.querySelector(".GITHtokenEditModal .GITHcancel")?.addEventListener("click", function(){ POPpopoutClose(GITHtokenEdit); });
}

function GITHsetToken(){
   let email =  (document.querySelector(".GITHeditEmail")?.value || "").trim();
   let Data = {};
   if(email) Data.email = email;
   Data.token = (document.querySelector(".GITHeditToken")?.value || "").trim();
   APIcall("git/token/set", Data, function(R){
      if(!R.success) return GITHsetError(".GITHtokenEditModal", R.error || "");
      let Message = GINsetToken();
      GINaddAndCommit({message: Message, users: true});
      POPpopoutClose(GITHtokenEdit);
      MODELforcePoll("GITHmodel");
      MODELforcePoll("GITHslowModel");
      MODELforcePoll("CORmodel");
      MODELforcePoll("CORmodelSlow");
      GITHresetTokenFields();
      GITHupdateOwners();
   });
}

function GITHtokenWarning(){
   let Data = GITHgitTokenValid();
   return !Data?.set || !Data?.access || !Data?.valid || Data?.deprecated;
}
