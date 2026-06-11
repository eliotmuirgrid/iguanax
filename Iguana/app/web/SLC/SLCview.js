function SLCview(){
   SETrender();
   SLCrender();
   SLCmodelInit(true);
}

function SLCviewLogEncryptionHtml(Config){
   const EncryptionOn = Config;
   let Html = /*html*/`
   <div class="SETversionPanel">
      <div class="SETversionLabel">Log Encryption:</div>
      <div class="SETversionDetail STSdescription">${EncryptionOn ? "Enabled" : "Disabled"}</div>
   </div>
   `
   return Html;
}