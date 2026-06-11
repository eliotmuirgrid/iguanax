let SLCmodelData;
let SLCconfirmCard;

function SLCencryptionKeysMatch(){
   const Key  = SLCencryptionKey();
   const Key2 = SLCencryptionKeyConfirm();
   if (!Key && !Key2) { return true; }
   if (Key && Key2)   { return Key == Key2; }
   return false;
}

function SLCencryptionKey(){
   let Key = document.querySelector('.SLCencryptionKey');
   if (!Key) return ""
   return Key.value;
}

function SLCencryptionKeyConfirm(){
   let KeyConfirm = document.querySelector('.SLCencryptionKeyConfirm');
   if (!KeyConfirm) return ""
   return KeyConfirm.value;
}

function SLCencryptionValue(){
   let Selector = document.querySelector('.SLCsetLogEncryption');
   if (!Selector) return; 
   return (Selector.value == "enabled");
} 

function SLCsaveArgs(){
   let Selector = document.querySelector('.SLCsetLogEncryption')
   let Args = {}
   Args["encrypt"] = SLClogEncryption(); // default to value read from Iguana
   if (!Selector) return Args;
   Args["encrypt"] = SLCencryptionValue();
   Args["key"] = SLCencryptionKey();
   Args["key_confirm"] = SLCencryptionKeyConfirm();
   return Args;
}

function SLCvalidateFields(){
   if (!SLCencryptionKeysMatch()) {
      SNCKsnackbar("error", "Encryption keys provided do not match.");
      return false;
   }
   if (SLCcomponentsRunning()){
      SNCKsnackbar("error", "All components must be stopped first.");
      return false;
   }
   return true;
}

function SLCsaveConfirmHtml() {
   let isEnabled = SLCencryptionValue();
   let ButtonClass = !isEnabled ? " BUTTONaction" : " BUTTONdisabled";
   return /*html*/`
   <div>To enable or disable log encryption, Iguana will need to be restarted. If encryption is enabled, the encryption key will need to be entered on every startup of Iguana.</div>
   <br/>
   ${SLCconfirmCheckboxes(isEnabled)}
   <div>Are you sure you want to proceed?</div>
   <div class="SLCconfirmButtons BUTTONgroup">
      <div class="SLCrestartCancel BUTTONcancel BUTTONstandard">CANCEL</div>
      <div class="BUTTONstandard SLCrestartConfirm${ButtonClass}">CONFIRM</div>
   </div>`;
}

function SLCconfirmCheckboxes(){
   let isEnabled = SLCencryptionValue();
   return /*html*/`
   ${isEnabled ? /*html*/`
      <div class="SLCcheckboxContainer">
         <br/>
         <input type="checkbox" class="SLCencAck">  I confirm I have saved my encryption key somewhere safe.
         <br/>
         <input type="checkbox" class="SLCencAck2"> I confirm I understand it must be entered every time IguanaX starts up.
         <br/>
      </div>
   ` : ""}`
}

function SLCsaveConfirm(){
   SLCconfirmCard = POPpopoutOpen({
      title: "Iguana will be restarted", 
      width: "600px",
      content: SLCsaveConfirmHtml(), 
      className: 'SLCconfirmPopup', 
      esc_close : true
   });
   SLCconfirmListeners();
}

function SLCencAckChange() {
   let Div1 = document.querySelector(".SLCencAck");
   let Div2 = document.querySelector(".SLCencAck2");
   let button = document.querySelector(".SLCrestartConfirm");
   if (!Div1 || !Div2 || !button) return;
   let Checked1 = Div1.checked;
   let Checked2 = Div2.checked;
   let Add = Checked1 && Checked2 ?    "BUTTONaction" : "BUTTONdisabled";
   let Remove = Checked1 && Checked2 ? "BUTTONdisabled" : "BUTTONaction";
   button.classList.add(Add);
   button.classList.remove(Remove);
}

function SLCsave(){
   if (!SLCeditPage()) return;
   if (!SLCvalidateFields()) return;
   SLCsaveConfirm();
}

