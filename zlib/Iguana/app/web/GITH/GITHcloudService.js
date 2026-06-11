let GITHcloudServiceEdit;
function GITHwarningMessage(Message){
   return /*html*/`<div class="GITHnotSet">${Message} </div>`;
}

function GITHserverMessageHtml(){
   let Response = MODELdata?.GITHgitServer;
   if(!Response?.success) return GITHwarningMessage(Response?.error);
   let Link = GITHgitAddressLink();
   let Data = Response?.data || {};
   let Address = Data.address;
   let Html = `<a target="_blank" class="GITHserverLink" href="${Link}">${Address}</a>`;
   return Html;
}

function GITHupdateGitServer(){
   let ServerHtml = GITHserverMessageHtml() + `<div class="GITHcloudService">${GITHgitProvider() ? `( ${GITHgitProvider()} )` : ""}</div>`;
   GITHupdateValue(document.querySelector(".GITHserverAddress"), ServerHtml);
}

function GITHserviceHtml(){
   return /*html*/`
   <div class="GITHbox">
      <div class="GITHtitleRow">
         <div class="GITHtitle GITHgitCloudTitle">Git Server</div>
         <div class="GITHdesc GITHdescCreds">Enter the Git Server URL you would like Iguana to use.
         </div>
         <div class="GITHeditService BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">URL:</div>
         <div class="GITHtableVal GITHserverAddress"></div>
      </div>
   </div>`;
}

function GITHserverOptions(Servers){
   let html = "";
   for(Server in Servers) html = html + /*html*/`<option value="${Server}">`;
   return html;
}

function GITHserverListHtml(){
   let Servers = GITHgitServers();
   return /*html*/`
   <datalist id="GITHserverList">
      ${GITHserverOptions(Servers)}
   </datalist>`;
}

function GITHvalidAddressesHtml(){
   let Servers = GITHgitServers();
   let html = `<div class="GITHaddressRow">`;
   for(Server in Servers) {
      let Type = Servers[Server]?.type || "";
      html = html + /*html*/`
         <div class="GITHvalidAddress">${ESChtmlEscape(Server)}</div>
         <div class="GITHhelpRow">( ${Type} )</div>
      `;
   }
   return html + "</div>";
}

function GITHeditServiceHtml(){
   let Address  = GITHgitAddress();
   return /*html*/`
   <div class="GITHeditServiceModal">
      <div class="GITHtableTitle">Click one of the known URLs to populate the field below:</div>
      <br>
      <div class=GITHvalidBox>
         ${GITHvalidAddressesHtml()}
      </div>
      <br>
      <div class="GITHserverMessage">If you cannot find the right git server URL then - [<a href="#settings/git_servers" class="SETlink">Go to Git Servers</a>]</div>
      <br>
      <br>
      <div class="GITHtableTitle">URL:</div>
      <br>
      <div class="GITHtableVal">
         <input type="text" class="GITHlocalServerAddress FORMinput" list="GITHserverList" value="${Address}">
         ${GITHserverListHtml()}
      </div>
      <div class="GITHerror GITHserviceError"></div>
      <div class="BUTTONgroup GITHbutton">
         <div class="GITHcancel BUTTONstandard BUTTONcancel">CANCEL</div>
         <div class="GITHsave BUTTONstandard BUTTONaction">SAVE</div>
      </div>
   </div>`;
}

function GITHsetServicePopout(){
   GITHcloudServiceEdit = POPpopoutOpen({
      title:"Set Git Server",
      width: "550px",
      content: GITHeditServiceHtml()
   });
   FORMfocus(document.querySelector(".GITHeditServiceModal .FORMselectBox"));
   FORMenterListener(document.querySelector(".GITHeditServiceModal"), GITHsetServer);
   document.querySelector(".GITHeditServiceModal").addEventListener("click", GITHserviceListeners);
}

function GITHserviceListeners(e){
   if(e.target.closest(".GITHsave"))         return GITHsetServer();
   if(e.target.closest(".GITHcancel"))       return POPpopoutClose(GITHcloudServiceEdit);
   if(e.target.closest(".GITHvalidAddress")) return GITHsetAddress(e.target);
}

function GITHsetAddress(target){
   let Service       = target?.innerHTML || "";
   let ServerAddress = document.querySelector(".GITHlocalServerAddress");
   ServerAddress.value = Service;
}

function GITHsetServer(){
   let Data = {};
   Data.server = document.querySelector(".GITHlocalServerAddress")?.value.trim() || "";
   APIcall("git/server/set", Data, function(R){
      if(!R.success) return SNCKsnackbar('error', "Failed to set server");
      GINaddAndCommit({message: `Added server ${Data.server}`, users: true});
      POPpopoutClose(GITHcloudServiceEdit);
      MODELforcePoll("GITHmodel");
      MODELforcePoll("GITHslowModel");
      CORtokenValidInterval();
      MODELforcePoll("CORmodel", function(){
         MODELforcePoll("CORmodelSlow");
      });
      GITHresetTokenFields();
      GITHresetSshFields();
   });
}
function GITHcloudWarning(){
   return GITHserverError();
}