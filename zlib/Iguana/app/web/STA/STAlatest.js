let STAlatestContainer;

function STAlatestHtml(){
   return /*html*/`
   <div class="STAlatestModal">
      You are trying to upgrade to the latest beta release. IguanaX will start by downloading the latest beta. After that, it will log you out and restart the service.
      <br>
      <br>
      <b>Warning : Updating to the latest beta release may lead to unstable performance. Exercise caution before proceeding!</b>
      <br>
      <br>
      <input type="checkbox" class="STAlatestAck"> I acknowledge the risks of updating to a beta release and would like to proceed.
      <br>
      <br>
      <div class="STAdownloadingContainer"></div>
      ${STAcompletedDownloadHtml()}
      <div class='BUTTONgroup STAupdateButtons'>
         <div class="BUTTONdefault BUTTONstandard STAcancel">CANCEL</div>
         <div class="BUTTONdisabled BUTTONstandard STAlatest">UPDATE TO BETA</div>
      </div>
   </div>`;
}

function STAlatestPopout(){
   let modal = document.querySelector(".STAupdateModal");
   if(modal) return;
   STAlatestContainer = POPpopoutOpen({title : `Update to Beta Release`, width :"550px", content: STAlatestHtml(), dismissible: false});
   STAlatestAckChange();
   STAlatestListeners();
}

function STAlatestListeners(){
   let Div = document.querySelector(".STAlatestModal");
   Div.addEventListener("click", STAlatestClick);
   Div.addEventListener("change", STAlatestChange);
}

function STAlatestClick(e){
   if(e.target.closest(".STAlatest")) return STAupgradeToLatest(e.target);
   if(e.target.closest(".STAcancel")) return POPpopoutClose(STAlatestContainer);
}

function STAlatestChange(e){
   if(e.target.closest(".STAlatestAck")) return STAlatestAckChange();
}

function STAdownloadingBetaDivHtml(){
   return /*html*/`
   <div class="STAdownloadingDiv STAhidden">
      <br>
      <div class="STAupdateRow">
         <div class="STAspinner"></div>
         <div class="STAloading">Downloading beta ...</div>
      </div>
   </div>`;
}

function STAbeginDownloadBetaUi(){
   let D = document.querySelector(".STAdownloadingDiv");
   if(D) D.classList.remove("STAhidden");
   let B = document.querySelector(".STAlatest");
   if(B) B.classList.remove("BUTTONaction");
   if(B) B.classList.add   ("BUTTONdisabled");
   let C = document.querySelector(".STAcancel");
   if(C) C.classList.remove("BUTTONdefault");
   if(C) C.classList.add   ("BUTTONdisabled");
}

function STAupgradeToLatest(target){
   console.log("would upgrade now!");
   if(target.closest(".BUTTONdisabled")) return;
   STAupdateDownloadingContainer(true);
   APIcall("upgrade/request/special", {}, function(){});
   STAbeginDownloadBetaUi();
   // Run upgrade_special process with guid set to latest. The build servers symlink the last successful build to latest.
   let Guid = "latest";
   APIcall("upgrade/download/special", {guid : Guid}, function(R) { 
      STAfinishDownloadUi();
      setTimeout(STAdownloadDone, 500, R, Guid); 
   });
}

function STAdownloadDone(R, Version){
   POPpopoutClose(STAlatestContainer);
   if(!R.success) { 
      return SNCKsnackbar('error', R.error, 20000); 
   }
   APIcall("upgrade/replace", {version: Version}, function(R) {
      // it does not reach here because iguana shuts down during the restart
      UPGRADEdone(R); 
   });
}

function STAlatestAckChange(){
   let Div = document.querySelector(".STAlatestAck");
   let button = document.querySelector(".STAlatest");
   if(!Div) return;
   let Permission = STAupgradePermission();
   let Checked = Div.checked && Permission; 
   let Add    = Checked ? "BUTTONaction"   : "BUTTONdisabled";
   let Remove = Checked ? "BUTTONdisabled" : "BUTTONaction";
   button.classList.add(Add);
   button.classList.remove(Remove);
   let title = !Permission ? "You do not have permission to do this." : "";
   if(button.title != title) button.title = title;
}