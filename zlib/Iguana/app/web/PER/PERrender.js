function PERview(){
   SETrender();
   PERrender();
   PERmodelInit(true);
}

function PERedit(){
   SETrender();
   PERrender();
   PERmodelInit(false);
}

function PERrender(){
   document.querySelector('.PERheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".PERmodal");
   if(skeleton) return;
   div.innerHTML = PERskeletonHtml();
   PERlisteners();
}

function PERdefault(){ window.location = PERlocation(); }

function PERlocation(){
   return "/#settings/peers";
}

function PERskeletonHtml(){
   return /*html*/`
   <div class="PERmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="PERtitle">
         <div class="">Peers</div>
         <div class="PEReditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Configure additional Iguana servers accessible by this instance. [<a target="_blank" href="${LINKsettingsGitServers}">Learn more</a>] </div>
      <div class="PERrulesTarget"></div>
   </div>`;
}
