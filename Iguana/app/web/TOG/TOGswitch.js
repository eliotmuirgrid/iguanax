let TOGbuttonMap = {};
TOGbuttonMap.OFF    = "START";
TOGbuttonMap.ON     = "STOP";
TOGbuttonMap.YELLOW = "KILL"; 
TOGbuttonMap.RED    = "START"; 

let TOGcolourMap = {};
TOGcolourMap.OFF    = "TOGgrey";
TOGcolourMap.ON     = "TOGgreen";
TOGcolourMap.YELLOW = "TOGyellow"; 
TOGcolourMap.RED    = "TOGred"; 

function TOGbuttonWord(Light, On){
   if(On && Light == "YELLOW") return "KILL";
   if(On) return "STOP";
   return "START";
}

function TOGswitchHtml(ClassName, Light, On, Stop, Start){
   let Status = Light || "OFF";
   let Action = TOGbuttonWord(Light, On);
   let ButtonClass = "";
   let Title = "";
   if(!On && !Start || On && !Stop) ButtonClass = "BUTTONdisabled";
   if(!On && !Start) Title = "You do not have permission to start this component.";
   if( On && !Stop)  Title  = "You do not have permission to stop this component.";
   let Colour = TOGcolourMap[Status] || "";
   return /*html*/`<div class="TOGswitchDiv ${Colour} ${ClassName}">
      <div class="TOGbutton BUTTONdefault BUTTONstandard ${ButtonClass}" type="checkbox" title="${Title}">${Action}</div>
      <div class="TOGstatus"></div>
   </div>`;
}

function TOGswitchDescription(light, on) {
   switch (light) {
     case "RED":
       return on
         ? "Running (error)"
         : "Off (error)";
 
     case "YELLOW":
       return "Changing…";
 
     case "GREEN":
       return on
         ? "Running"
         : "Not running";
 
     default:
       // Fallback if light is missing/unknown
       return on ? "Running" : "Not running";
   }
 }

function TOGactivatePoller(Command, JobId){
   if      (Command == 'component/start') { DASHbulkStartPollerStart(JobId); }
   else if (Command == 'component/stop')  { DASHbulkStopPollerStart(JobId);  }
}

function TOGswitchApi(target){
   let Div = target.querySelector(".TOGbutton");
   if(!target) return;
   let Direction = Div.innerHTML == "START";
   let Command = Direction ? 'component/start' : 'component/stop';
   return Command;
}

function TOGswitchArg(target, Components){
   let Div = target.querySelector(".TOGbutton");
   if(!target) return {};
   let Force = Div.innerHTML == "KILL";
   let Arg = {};
   Arg.components = Components;
   Arg.force      = Force;
   Arg.tracked    = false; // only bulk actions want feedback
   return Arg;
}
