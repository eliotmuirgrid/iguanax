let DETrepositionPopout;

function DETrepositionScreen(){
   let Data = {component : PAGEhashParameter('component')};
   APIcall('log/queue/consumers', Data, DETrenderReposition);
}

function DETrepositionHtml(Data){
   return /*html*/`
   <div>This will position the queue after the message selected!</div>
   <br>
   <p class="FORMlabel DETactionDescription">Select the downstream component to which you want to position its queue to the log entry:</p>
   <select class="DETcomponentSelect FORMselectBox">${DETdownstreamList(Data)}</select>
   <br>
   <div class="BUTTONgroup DETbuttons">
      <div class='DETrepositionCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='DETrepositionConfirm BUTTONaction BUTTONstandard'>CONFIRM</div>
   </div>`;
}

function DETrenderReposition(Response) {
   if (!Response.success) return SNCKsnackbar('error', Response?.error || "");
   DETrepositionPopout = POPpopoutOpen({
      title:'Reposition Queue', 
      className : "DETmodalReposition", 
      content: DETrepositionHtml(Response.data)
   });
   DETcontrolButton();
   DETrepositionListeners();
}

function DETdownstreamList(Data){
   let H = Data.length ? `<option disabled>Select Component</option>` : `<option value="" disabled>There are no downstream components to select</option>`;
   for (let i = 0; i < Data.length; i++){
      let Guid = Data[i];
      H += `<option value="${Guid}">${DETcomponentName(Guid, DETcomponentListData())}</option>`;
   }
   return H;
}

function DETcontrolButton(){
   let Button = document.querySelector(".DETrepositionConfirm");
   if(!Button) return;
   let Div = document.querySelector(".DETcomponentSelect");
   let Disabled = !Div || Div.value == "";
   if(Disabled) {
      Button.classList.add("BUTTONdisabled");
      Button.classList.remove("BUTTONaction");
   }
   else {
      Button.classList.add("BUTTONaction");
      Button.classList.remove("BUTTONdisabled");
   }
}

function DETrepositionListeners() {
   const RepositionBtn = document.querySelector('.DETrepositionConfirm');
   const CancelBtn     = document.querySelector('.DETrepositionCancel');
   const Select        = document.querySelector('.DETcomponentSelect');
   if (RepositionBtn) RepositionBtn.addEventListener('click', DETrepositionQueue);
   if (CancelBtn)     CancelBtn.addEventListener    ('click', function() { POPpopoutClose(DETrepositionPopout); });
   if (Select)        Select.addEventListener('change', function() { DETcontrolButton(); });
}

function DETrepositionQueue() {
   let Guid = DETselectedComponent();
   const Data = {
      component : PAGEhashParameter('component'),
      position  : PAGEhashParameter('id'),
      datetime  : PAGEhashParameter('time'),
      consumer  : Guid,
   };
   APIcall('log/queue/position', Data, function(R){
      POPpopoutClose(DETrepositionPopout);
      if(!R.success) return SNCKsnackbar('error', R.error);
      SNCKsnackbar('success', `Updating queue position of ${DETcomponentName(Guid, DETcomponentListData())}`, 5000);
   });
}

function DETselectedComponent() {
   let Div = document.querySelector(".DETcomponentSelect");
   if(!Div) return; 
   return Div.value;
}
