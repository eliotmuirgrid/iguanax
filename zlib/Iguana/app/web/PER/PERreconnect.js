function PERupdateStatus(target, message){
   let Row = target.closest(".PERrow");
   let Status = Row.querySelector(".PERstatus");
   Status.innerHTML = message;
}

function PERreconnect(target){
   let Idx = PERrowToIdx(target);
   let PeerId = PERidxToId(Idx);
   PERupdateStatus(target, "Connecting...");
   APIcall("peers/connect", {peer : PeerId}, function(R){
      let Status;
      if(!R.success) Status = "Error connecting: " + R?.error || "";
      else           Status = "Successfully connected."
      PERupdateStatus(target, Status);
});
}