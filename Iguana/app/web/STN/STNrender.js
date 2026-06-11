function STNrender(){
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".STNmodal");
   if(skeleton) return;
   div.innerHTML = STNskeletonHtml();
   STNlisteners();
}

function STNskeletonHtml(){
   return /*html*/`
   <div class="STNmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="SETnotificationsTitle">
         <div class="SETnotificationsLabel">Notifications</div>
         <div class="STNeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Define notification rules with glob expression tag matching. A notification message will be sent to the component selected when a rule is triggered. [<a href="${LINKsettingsNotificationsHeader}" target="_blank">Learn more</a>]</div>
      <div class="SETselectNotificationComponent"></div>
      <div class="STNrulesTarget"></div>
   </div>`;
}