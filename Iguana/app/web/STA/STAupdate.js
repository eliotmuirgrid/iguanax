let STAupdateContainer;
let STAupdateVersion;

function STAupdatePopout(){
   let modal = document.querySelector(".STAupdateModal");
   if(modal) return;
   STAupdateContainer = POPpopoutOpen({title : `Update IguanaX`, width :"750px", content: STAupdateHtml()});
   STAupdateListeners();
   STAgetVersionInfo();
}

function STAgetVersionInfo(){
   if(STAtestChanglog) return STAgetChangelog();
   APIcall('release/info', {}, function(R) {
      // if(!R.success) { return; } // TODO error screen
      STAupdateVersion = R?.data?.version;
      STAupdateDownloadingContainer();
      STAgetChangelog();
   });
}

function STAgetChangelog(){
   STAdefaultMessageChangelog();
   if(STAtestChanglog) return STAsetChangeLog(STAtestChanglogHtml);
   APIcall('release/changelog', { "version" : STAupdateVersion }, function(R){
      if(!R.success) { 
         STAchangelogError(R.error);
      } else{
         STAsetChangeLog(R.data.changelog);
      }
   });
}

function STAupdateListeners(){
   let modal = document.querySelector(".STAupdateModal");
   modal.addEventListener("click", STAupdateClickHandler)
}

function STAupdateClickHandler(e){
   if(e.target.closest(".BUTTONdisabled")) return;
   if(e.target.closest(".STAcancel")) {
      POPpopoutClose(STAupdateContainer);
   }
   if(e.target.closest(".STAupdate")) {
      STAdownloadUpdate();
   }
   if(e.target.closest(".STAtryAgain")){
      STAgetVersionInfo();
   }
}

function STAdownloadUpdate(){
   APIcall("upgrade/request/latest", {}, function(){});
   STAbeginDownloadUi();
   APIcall("upgrade/download", {version : STAupdateVersion}, function(R) { 
      STAfinishDownloadUi();
      setTimeout(STAdownloadComplete, 500);
   });
}

function STAdownloadComplete(){
   APIcall("upgrade/replace", {version: STAupdateVersion}, function(R) { 
      // it does not reach here because iguana shuts down during the restart
   });
}

function STAbeginDownloadUi(){
   let D = document.querySelector(".STAdownloadingDiv");
   if(D) D.classList.remove("STAhidden");
   let B = document.querySelector(".STAupdate");
   if(B) B.classList.remove("BUTTONaction");
   if(B) B.classList.add   ("BUTTONdisabled");
   let C = document.querySelector(".STAcancel");
   if(C) C.classList.remove("BUTTONdefault");
   if(C) C.classList.add   ("BUTTONdisabled");
}

function STAfinishDownloadUi(){
   let D = document.querySelector(".STAdownloadingDiv");
   if(D) D.classList.add("STAhidden");
   let C = document.querySelector(".STAdownloadComplete");
   if(C) C.classList.remove("STAhidden");
}

function STAsetChangeLog(Changelog){
   let D = document.querySelector(".STAchangeLog");
   if(!D) return;
   if(D.innerHTML !== Changelog) D.innerHTML = Changelog;
   if(STAtestChanglog) return;
   let Permission = STAupgradePermission();
   let B = document.querySelector(".STAupdate");
   if(B && Permission) B.classList.add   ("BUTTONaction");
   if(B && Permission) B.classList.remove("BUTTONdisabled");
   if(!Permission) B.title = "You do not have permission to do this.";
}

function STAupdateHtml() {
   return /*html*/`
   <div class="STAupdateModal">
      <div class="STAchangeLog">
      </div>
      <br>
      To update, Iguana will start by downloading the latest version. After that, it will log you out and restart the service.
      <br>
      <br>
      <div class="STAdownloadingContainer"></div>
      ${STAcompletedDownloadHtml()}
      <div class='BUTTONgroup STAupdateButtons'>
         <div class="BUTTONdefault BUTTONstandard STAcancel">CANCEL</div>
         <div class="BUTTONdisabled BUTTONstandard STAupdate">UPDATE</div>
      </div>
   </div>`;
}

function STAupdateDownloadingContainer(IsBeta){
   let d = document.querySelector(".STAdownloadingContainer");
   if(!d) return;
   let html = IsBeta ? STAdownloadingBetaDivHtml() : STAdownloadingDivHtml();
   if(d.innerHTML !== html) d.innerHTML = html;
}

function STAdownloadingDivHtml(){
   return /*html*/`
   <div class="STAdownloadingDiv STAhidden">
      <br>
      <div class="STAupdateRow">
         <div class="STAspinner"></div>
         <div class="STAloading">Downloading Version ${STAupdateVersion} ...</div>
      </div>
   </div>`;
}

function STAdefaultMessageChangelog(){
   let html = /*html*/`<div class="STAloading">Getting info about the latest version...</div>`;
   let D = document.querySelector(".STAchangelog");
   if(!D) return;
   if(D.innerHTML !== html) D.innerHTML = html;
}

function STAcompletedDownloadHtml(){
   return /*html*/`
   <div class="STAdownloadComplete STAhidden STAloading">
      <br>
      Downloaded! Starting your update ...
   </div>`;
}

function STAchangelogError(Error){
   let D = document.querySelector(".STAchangeLog");
   if(!D) return;
   let html = STAchangelogErrorHtml(Error);
   if(D.innerHTML !== html) D.innerHTML = html;
}

function STAchangelogErrorHtml(Error){
   return /*html*/`
   <div class="STAupdateRow">
      <div class="STAloading">${Error}</div>
      <div class="STAtryAgain STAloading">Try again?</div>
   </div>`;
}