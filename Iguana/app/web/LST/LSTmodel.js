let LSTargs;

function LOGlstControl(){
   let Done = LOGlstDone();
   if(Done === undefined || Done) { return; }
   LSTmodelInit();
}

function LSTmodelInit(){
   MODELpollerAdd("LSTmodel", "LSTpoll", 0.5, "logs/resubmit/results", {});
   MODELviewAdd("LSTmodel", "LSTrenderSkeleton");
   MODELviewAdd("LSTmodel", "LSTupdateBar");
   MODELviewAdd("LSTmodel", "LSTupdateTitle");
   MODELviewAdd("LSTmodel", "LSTupdateStop");
   MODELviewAdd("LSTmodel", "LSTupdateDismiss");
   MODELviewAdd("LSTmodel", "LSTupdateHref");
   MODELviewAdd("LSTmodel", "LSTupdateDone");
   MODELforcePoll("LSTmodel");
   MODELstart("LSTmodel");
}

function LSTinit(Args, Callback){
   BLKdisableButtons();
   LSTargs = Args;
   APIcall("logs/resubmit", Args, function(R){
      if(Callback) Callback(R?.success, R?.error);
      LSTmodelInit();
   });
}

function LSTpollData(){
   return MODELdata?.LSTpoll?.data;
}

function LSTdone(){
   return LSTpollData()?.done;
}

function LSTtime(){
   let Time = LSTpollData()?.time;
   if(Time) Time = parseInt(Time);
   return Time;
}

function LSTstartTime(){
   let Time = LSTpollData()?.start;
   if(Time) Time = parseInt(Time);
   return Time;
}

function LSTendTime(){
   let Time = LSTpollData()?.end;
   if(Time) Time = parseInt(Time);
   return Time;
}

function LSTstatus(){
   return LSTpollData()?.status;
}