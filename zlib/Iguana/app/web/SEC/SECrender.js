function SECview() {
   SETrender();
   SECrender();
   SECmodelInit();
}

function SECedit() {
   SETrender();
   SECrender();
   SECmodelInit(false);
}

function SECrender() {
   document.querySelector('.SANDaccess').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".SANDmodal");
   if(skeleton) return;
   div.innerHTML = SECskeletonHtml();
   SEClisteners();
}

function SECskeletonHtml() {
   return /*html*/`
   <div class="SANDmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="SECtitle">
         <div class="">Security</div>
         <div class="SANDeditTarget"></div>
      </div>
      <div class="SETpageDescription">Configure whether OS execution commands can be run and which data directories Iguana can access. [<a target="_blank" href="${LINKsettingsDirectoryAccess}">Learn more</a>]</div>
      <div class="SANDdirsTarget"></div>
   </div>`;
}

function SECeditPage(){
   let Page = PAGEhash();
   return Page === "settings/security/edit";
}