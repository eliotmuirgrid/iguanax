function SLCrender(){
   document.querySelector('.SLClogConfig').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let skeleton = div.querySelector(".SLClogConfigModal");
   if(skeleton) return;
   div.innerHTML = SLCskeletonHtml();
   SLClisteners();
}

function SLCskeletonHtml(){
   return /*html*/`
   <div class="SLClogConfigModal SETsettingsPane SETsettingsPaneLayout">
      <div class="STGtitle">
         <div class="SETpurgingLabel">Log Encryption</div>
         <div class="SLCeditTarget BUTTONgroup"></div>
      </div>
      <div class="SETpageDescription">Configure log encryption settings. [<a href="${LINKsettingsLogEncryption}" class="SLCkeyHelpLink">Learn more</a>]</div>
      <div class="SLClogEncryptTarget"></div>
   </div>`;
}

function SLCupdateLogEncryption(){
   let div = document.querySelector(".SLClogEncryptTarget");
   if(!div) return;
   let EncryptionConfig = SLClogEncryption();
   let Html = '';
   if (SLCeditPage()) { Html = SLCeditLogEncryptionHtml(EncryptionConfig); }
   else               { Html = SLCviewLogEncryptionHtml(EncryptionConfig); }
   div.innerHTML = Html;
}
