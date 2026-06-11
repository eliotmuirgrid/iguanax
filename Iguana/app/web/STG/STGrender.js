function STGview(){
   SETrender();
   STGrender();
   STGmodelInit(true);
}

function STGedit(){
   SETrender();
   STGrender();
   STGmodelInit(false);
}

function STGrender(){
   document.querySelector('.STGpurging').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".STGmodal");
   if(skeleton) return;
   div.innerHTML = STGskeletonHtml();
   STGlisteners();
}

function STGskeletonHtml(){
   return /*html*/`
   <div class="STGmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="STGtitle">
         <div class="SETpurgingLabel">Log Purging</div>
         <div class="STGeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Define log purge rules with glob expression matching rule. Logs are auto-purged daily at midnight. Cut-off dates for purging may not be exact based on log file structure. [<a target="_blank" href="${LINKsettingsLogPurging}">Learn more</a>]</div>
      <div class="STGrulesTarget"></div>
   </div>`;
}
