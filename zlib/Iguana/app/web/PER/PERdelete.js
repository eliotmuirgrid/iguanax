function PERdelete(){
   PERview();
   PERdeletePopout();
}

function PERdeletePopout(){
   if(document.querySelector(".PERdelete")) return;
   POPpopoutOpen({
      title: 'Delete peer', 
      content: PERdeleteHtml(), 
      esc_close : true,
      width : "400px",
      callback : PERdefault
   });
   document.querySelector(".PERdelete").addEventListener("click", PERremove);
}

function PERdeleteHtml(){
   let Peer = PAGEhashParameter("peer");
   let PeerVal = "";
   if(Peer) PeerVal = Peer;
   return /*html*/`
   <div class="FORMlabel">Peer ID:</div>
   <br>
   <div class="PERdisplay">${PeerVal}</div>
   <br>
   <div class="PERerror"></div>
   <br>
   <div class="PERgroup BUTTONgroup">
      <a href="/#settings/peers" class="BUTTONcancel BUTTONstandard">CANCEL</a>
      <div class="PERdelete BUTTONaction BUTTONstandard">DELETE</div>
   </div>`;
}

function PERremove(){
   let Peer = PAGEhashParameter("peer");
   APIcall("peers/delete", { peer : Peer }, function(R){
      if(!R?.success) return PERrenderError(R?.error);
      let Message = GINdeletePeer(Peer);
      GINaddAndCommit({message: Message, settings: true});
      window.location = PERlocation();
   });
}