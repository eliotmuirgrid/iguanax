function GSVview(){
   SETrender();
   GSVrender();
   GSVmodelInit(true);
}

function GSVedit(){
   SETrender();
   GSVrender();
   GSVmodelInit(false);
}

function GSVrender(){
   document.querySelector('.GSVheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".GSVmodal");
   if(skeleton) return;
   div.innerHTML = GSVskeletonHtml();
   GSVlisteners();
}

function GSVskeletonHtml(){
   return /*html*/`
   <div class="GSVmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="GSVtitle">
         <div class="">Git Servers</div>
         <div class="GSVeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Configure your list of Git Servers. [<a target="_blank" href="${LINKsettingsGitServers}">Learn more</a>] </div>
      <div class="GSVrulesTarget"></div>
   </div>`;
}
