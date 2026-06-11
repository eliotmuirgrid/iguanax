function SLCedit(){
   SETrender();
   SLCrender();
   SLCmodelInit(false);
}

function SLCeditLogEncryptionHtml(Config){
   const EncryptionOn = Config;
   let Html =/*html*/`
   <div class="SETpageDescription">The log encryption key must be a cryptographically secure 32-character string. IguanaX will not remember this key and it must be re-entered everytime IguanaX starts up.</div>
   <div class="SETversionPanel">
      <div class="SLCtable">
         <div class="FORMlabel">Log Encryption:</div>
         <select class="FORMselectBox SLCsetLogEncryption">
            <option value="disabled" ${EncryptionOn ? "" : "selected"}>Disabled</option>
            <option value="enabled"  ${EncryptionOn ? "selected" : ""}>Enabled</option>
         </select>
      </div>
      <div class="SLCtable SLCencryptionKeyBlock">
         ${EncryptionOn ? SLCeditKeysHtml() : ""}
      </div>
   </div>`
   return Html;
}

function SLCeditKeysHtml(){
   return /*html*/`
   <div class="FORMlabel">Encryption Key:</div>
   <div class="FORMinputGroup">
      <input class="FORMinput SLCencryptionKey" type="password" placeholder="Enter encryption key">
      <div class="BUTTONstandard BUTTONcancel SLCkeyHelp"><img src="SLC/SLCoptions.svg"></div>
   </div>
   <div class="FORMlabel">Confirm Encryption Key:</div>
   <input class="FORMinput SLCencryptionKeyConfirm" type="password" placeholder="Confirm encryption key">`;
}

function SLCupdateEditKeysHtml(){
   let Target = document.querySelector(".SLCencryptionKeyBlock");
   if (!Target) return;
   SLCencryptionValue() ? Target.innerHTML = SLCeditKeysHtml() : Target.innerHTML = "";
}