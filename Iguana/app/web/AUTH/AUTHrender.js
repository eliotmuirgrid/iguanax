function AUTHview(){
   SETrender();
   AUTHrender();
   AUTHmodelInit(true);
}

function AUTHedit(){
   SETrender();
   AUTHrender();
   AUTHmodelInit(false);
}

function AUTHrender(){
   document.querySelector('.AUTHheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".AUTHmodal");
   if(skeleton) return;
   div.innerHTML = AUTHskeletonHtml();
   AUTHlisteners();
}

function AUTHskeletonHtml(){
   return /*html*/`
   <div class="AUTHmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="AUTHtitle">
         <div class="">Authentication</div>
         <div class="AUTHeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Configure SSO (Single Sign On). Modifying these settings as an external user will require re-authentication. [<a target="_blank" href="${LINKsettingsAuthentication}">Learn more</a>]</div>
      <div class="AUTHtarget"></div>
   </div>`;
}
