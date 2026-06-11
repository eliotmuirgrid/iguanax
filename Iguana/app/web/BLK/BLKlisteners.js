function BLKlisteners(){
   let Div = document.querySelector(".BLKmodal");
   Div.addEventListener("click", BLKclick);
}

function BLKclick(e){
   if(e.target.closest(".BLKclose"))           return BLKclose();
   if(e.target.closest(".BLKresubmitConfirm")) return BLKresubmitConfirm();
}

function BLKclose(){
   if(!BLKsingleResubmit()) return POPpopoutClose(BLKcard);
   let Args = PAGEhashParameters();
   let location = "#logs";
   let url = location + PAGEformatParams(Args);
   window.location = url;
}

function BLKselectBoxArrowGuid(){
   let Select = document.querySelector(".BLKselect");
   let Guid = Select?.value;
   return Guid;
}

function BLKresubmitConfirm(){
   let Guid = BLKselectBoxArrowGuid();
   if(!Guid) return;
   let Args = BLKresubmitArgs(Guid);
   BLKcheckRunning(Args, Guid);
}

function BLKcheckRunning(Args, Guid){
   APIcall("component/run_list", {}, function(R){
      if(!R.success) return true;
      let Data = R?.data || {};
      let Running = Data[Guid] ? true : false;
      if(!Running) LSTinit(Args, BLKcallback);
      else         BLKstopComponent(Guid);
   });
}

function BLKstopComponent(Guid){
   CONFIRMaction("To resubmit messages to a component, the component cannot be running. Do you want to stop it?", function(){ BLKstopComponentExecute(Guid) }, "CONFIRM");
}

function BLKstopComponentExecute(Guid){
   APIcall("component/stop", {components: [Guid]}, function(R){
      if(!R.success) return BLKerror(R?.error);
      BLKsuccess("Successfully stopped component. Confirm to resubmit logs.");
   });
}

function BLKdisableButtons(){
   let Div = document.querySelector(".BLKconfirmTarget");
   Div.classList.add("BLKbuttonLoading");
   Div.classList.remove("BLKresubmitConfirm");
   Div.classList.remove("BUTTONaction");
   Div.classList.add("BUTTONdisabled");
}

function BLKenableButtons(){
   let Div = document.querySelector(".BLKconfirmTarget");
   Div.classList.remove("BLKbuttonLoading");
   Div.classList.add("BLKresubmitConfirm");
   Div.classList.add("BUTTONaction");
   Div.classList.remove("BUTTONdisabled");
}

function BLKcallback(Success, Error){
   if(Success) return BLKclose();
   BLKenableButtons();  
   if(Error) return BLKerror(Error);   
}

function BLKerror(Error){
   let Div = document.querySelector(".BLKerror");
   Div.innerHTML = "Error: " + Error;
}

function BLKsuccess(Message){
   let Div = document.querySelector(".BLKsuccess");
   Div.innerHTML = Message;
}