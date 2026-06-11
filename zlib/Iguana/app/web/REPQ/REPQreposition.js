function REPQgetRepositionArgs(SelectedLog){
   const Time = SelectedLog.split("-")[0]
   const Id   = SelectedLog.split("-")[1]
   let Out = {}
   for (i of REPQmessages){
      if ((i.time == Time) && (i.id == Id)) {
         Out["datetime"]  = i.time;
         Out["position"]  = i.id;
         Out["component"] = i.comp;
         return Out;
      }
   }
}

function REPQdoReposition(){
   console.warn("REPOSITIONING");
   const SelectedLog = document.querySelector('.REPQselectMessage');
   if (!SelectedLog) return;
   let Data = REPQgetRepositionArgs(SelectedLog.value);
   Data["consumer"] = PAGEhashParameter('component')

   APIcall("log/queue/position", Data, function(R){  // IMPORTANT: must keep type: "message" - we can only reposition to data messages
      if(!R.success) { 
         let ErrorDisplay = document.querySelector(".REPQrepositionError");
         if (ErrorDisplay) { ErrorDisplay.innerHTML = R.error; }
         return;
      } 
      REPQclose();
   });
}