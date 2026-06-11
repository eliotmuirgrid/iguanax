let SLCkeyHelpPopup;

function SLCkeyHelp(){
   SLCkeyHelpPopup = POPpopoutOpen({
      title : "Encryption Key Help",
      content : SLCkeyHelpHtml(),
      width : "800px",
      dismissible : true
   });
   SLCgenKey();
   document.querySelector(".SLCkeyHelpRegenerate").addEventListener("click", SLCgenKey);
   document.querySelector(".SLCkeyHelpClose").addEventListener("click", function(){ POPpopoutClose(SLCkeyHelpPopup); });
}

function SLCgenKey(){
   APIcall("log/encryption/gen_key", {}, function(Response){
      if(!Response.success) return;
      let Key = Response.data.key;
      let Target = document.querySelector(".SLCkeyHelpTextarea");
      Target.value = Key;
   });
}

function SLCkeyHelpHtml(){
   return /*html*/`
   <div>
      <div class="SLCcontentText"><b class="SLCkeyHelpImportant">IMPORTANT:</b> Save this key in a secure location as IguanaX <b>will not</b> remember it.</div>
   </div>
   <br>
   <br>
   <div class="SLCkeyHelpTitle SLCcontentText">Log encryption requires a 32-character encryption key.</div>
   <br>
   <div class="SLCkeyHelpDiv">
      <div>Copy this 32-character encryption key to use as your encryption key:</div>
      <br>
      <textarea class="SLCkeyHelpTextarea FORMinput"></textarea>
      <br>
      <br>
      <div class="SLCkeyHelpGrid">
         <div class="SLCkeyHelpText">Click to generate another encryption key with OpenSSL's rand function</div>
         <div class="BUTTONaction BUTTONstandard SLCkeyHelpRegenerate">REGENERATE</div>
      </div>
   </div>
   <br>
   <div class="BUTTONgroup SLCconfirmButtons">
      <div class="BUTTONstandard BUTTONcancel SLCkeyHelpClose">CLOSE</div>
   </div>`;
}
