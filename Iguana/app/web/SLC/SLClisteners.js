function SLClisteners(){
   let div = document.querySelector(".SLClogConfigModal");
   if(!div) return;
   div.addEventListener("click", SLCclick);
   div.addEventListener("change", SLCchange);
}

function SLCclick(e){
   if(e.target.closest(".SLCsave")) return SLCsave();
   if(e.target.closest(".SLCkeyHelp")) return SLCkeyHelp();
}

function SLCchange(e){
   if(e.target.closest(".SLCsetLogEncryption")) return SLCupdateEditKeysHtml();
}

function SLCconfirmListeners(){
   let div = document.querySelector(".SLCconfirmPopup");
   if(!div) return;
   div.addEventListener("click", SLCconfirmClick);
   div.addEventListener("change", SLCconfirmChange);
}

function SLCconfirmClick(e){
   if(e.target.closest(".BUTTONdisabled")) return;
   if(e.target.closest(".SLCrestartCancel")) return POPpopoutClose(SLCconfirmCard);
   if(e.target.closest(".SLCrestartConfirm")){
      SLCsaveApiCall();
      POPpopoutClose(SLCconfirmCard);
   };
}

function SLCconfirmChange(e){
   if(e.target.closest(".SLCencAck"))  return SLCencAckChange();
   if(e.target.closest(".SLCencAck2")) return SLCencAckChange();
}

function SLCsaveApiCall(){
   APIcall("log/encryption/set", SLCsaveArgs(), function(R){
      if(!R.success) return SNCKsnackbar("error", R.error);
      if(R.data.length > 0) {
         SNCKsnackbar("success", R.data);
         window.location = "#settings/log_config";
         return;
      }
      setTimeout(()=>{window.location = "/";}, 2000);
   });
}