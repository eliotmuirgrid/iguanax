function HAview() {
   SETrender();
   HArender();
   HAmodelInit(true);
}

function HAedit() {
   SETrender();
   HArender();
   HAmodelInit(false);
}

function HArender() {
   document.querySelector('.HAheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) { return; }
   let skeleton = div.querySelector(".HAmodal");
   if(skeleton) { return; }
   div.innerHTML = HAskeletonHtml();
   HAlisteners();
}

function HAskeletonHtml() {
   return /*html*/`
   <div class="HAmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="HAtitle">
         <div class="">High Availability</div>
         <div class="HAeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Configure high availability settings for your IguanaX instance.</div>
      <div class="HAtarget"></div>
   </div>`;
}
