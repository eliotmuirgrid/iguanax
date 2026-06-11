let TPEpeerScreenCard;

function TPEpeerScreen(){
   TUNscreen();
   TPEpeerScreenCard = POPpopoutOpen({
      title: `Iguana Server`, 
      width: '800px', 
      content: TPEpeerScreenHtml(),  
      esc_close : true,
      callback : TPEdestroy
   });
   TPEmodelPeerInit();
   TPElistenersInit();
}

function TPEmodelPeerInit(){
   MODELpollerAdd('TPEmodel','TPEinfo', 3, 'tunnel/peer', TPEnameArg);
   MODELviewAdd('TPEmodel', 'TPEpeerViewKey');
   MODELviewAdd('TPEmodel', 'TPEpeerViewStatus');
   MODELforcePoll('TPEmodel');
   MODELstart('TPEmodel');
}

function TPEnameArg(){
   return { name : PAGEhashParameter('name')};
}

function TPEpeerViewStatus(){
   let Name = PAGEhashParameter('name');
   let Data = MODELdata?.TPEinfo?.data;
   if(!Data) return;
   let H;
   let Connected = Data.connected;
   if(Connected) H = `<div class="TUNconnected"><b>${Name}</b> is connected</div>`;
   else          H = `<div class="TUNdisconnected"><b>${Name}</b> is not connected</div>`;
   let Div = document.querySelector('.TPEpeerStatus');
   if(!Div) return;
   if(Div.innerHTML != H ) Div.innerHTML = H;
}

function TPEpeerViewKey(){
   let E = document.querySelector('.TPEpublicKey');
   let Data = MODELdata.TPEinfo.data;
   let H = Data.key;
   if(E.innerHTML != H) E.innerHTML = H;
}

function TPElistenersInit(){
   let Div = document.querySelector('.TPEmodal');
   if(!Div) return;
   Div.addEventListener('click', TPEclick);
}

function TPEclick(e){ 
   if(e.target.closest('.TPEpeerDelete')) return TPEpeerDelete( PAGEhashParameter('name') );
   if(e.target.closest('.TPEcancel'))     return TPEdestroy();
}

function TPEdestroy(){
   location.hash = "cluster";
}

function TPEpeerScreenHtml(){
   return /*html*/ `
   <div class="TPEmodal">   
      <div class="TPEpeerStatus"></div>
      <br>
      <textarea disabled class="TPEpublicKey" placeholder="Public key" title="Public key of the peer you are trying to add."></textarea>
      <div class="BUTTONgroup TPEbuttonsRow">
         <div class="TPEcancel BUTTONstandard BUTTONdefault">CANCEL</div>
         <div class="TPEpeerDelete BUTTONaction BUTTONstandard">DELETE </div>
      </div>
   </div>`;
}

function TPEpeerDelete(Name){
   CONFIRMaction("Do you really want to delete peer " + Name + "?", function(){TPEpeerDeleteExecute(Name) }, "DELETE PEER");
}

function TPEpeerDeleteExecute(Name){
   APIcall("tunnel/peer/delete", {name : Name}, function() {
      window.location.hash = "cluster";
   });
}