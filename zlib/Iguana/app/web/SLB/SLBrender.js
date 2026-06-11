function SLBview(){
   SETrender();
   SLBrender();
   SLBopenDetails();
   SLBmodelInit();
   SLBlisteners();
}

function SLBrender() {
   document.querySelector('.SLBheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".SLBmodal");
   if(skeleton) return;
   SLBdataObj = {};
   SLBpageDataRendered = undefined;
   div.innerHTML = SLBskeletonHtml();
}

function SLBskeletonHtml() {
   let Description = "See every library your workspace uses, which components depend on it, and whether each one is up-to-date, outdated, or customized compared to its upstream version.";
   return /*html*/`
   <div class="SLBmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="SLBtitle">
         <div class="">Library Usage</div>
         <div class="SLBeditTarget"></div>
      </div>
      <div class="SETpageDescription">${Description} [<a target="_blank" href="${LINKsettingsDirectoryAccess}">Learn more</a>]</div>
      <div class="SLBtarget">
         <div class="SLBloadingDiv">
            <div class="CARDloading-spinner"></div>
            <div class="SLBinfo">Gathering library usage info...</div>
         </div>
      </div>
   </div>`;
}
