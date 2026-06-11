let LOGscrollbarLineSelectedPosition;
let LOGscrollbarTopIndex;
let LOGscrollbarBottomIndex;



function LOGscrollbarModelInit(){
   // Make an APIcall for one message from the oldest and use that as the epoch timestamp and then 
   MODELpollerAdd("LOGscrollbarModel", "LOGscrollbarNewest", 10, "logs/retrieve", LOGscrollbarNewestArgs);
   MODELpollerAdd("LOGscrollbarModel", "LOGscrollbarOldest", 23, "logs/retrieve", LOGscrollbarOldestArgs);
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetTop');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetBottom');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetTopDate');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetTopDateTitle');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetBottomDate');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetBottomDateTitle');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetFilterBoundTop');
   // MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarPositionUpdate');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarSetFilterBoundBottom');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarLineSelectedVisible');
   MODELviewAdd  ('LOGscrollbarModel', 'LOGscrollbarLineSelectedPositionUpdate');
   MODELforcePoll('LOGscrollbarModel', function() {
      LOGscrollbarPopout();
   });
   MODELstart    ('LOGscrollbarModel');
}

function LOGscrollbarNewestLog(){
   let Data = MODELdata?.LOGscrollbarNewest?.data || [];
   let TimeString = Data[0]?.time || "";
   let Time = parseInt(TimeString);
   return Time;
}

function LOGscrollbarOldestLog(){
   let Data = MODELdata?.LOGscrollbarOldest?.data || [];
   let TimeString = Data[0]?.time || "";
   let Time = parseInt(TimeString);
   return Time;
}

function LOGscrollbarSetTop()   { LOGscrollbarTopIndex    = LOGscrollbarNewestLog(); }
function LOGscrollbarSetBottom(){ LOGscrollbarBottomIndex = LOGscrollbarOldestLog(); }

function LOGscrollbarPositionUpdate(){
   let TopLog = document.querySelector('.LOGtopVisible');
   let Idx = !TopLog ? 0 : LOGentryIndex(TopLog);
   let Data = LOGshowListData[Idx];
   let Time = Data?.time;
   let ScrollLine = document.querySelector(".LOGscrollbarLine");
   LOGupdateLinePosition(ScrollLine, Time, 0);
   LOGcontrolOutsideSearchWarning(Time);
}

function LOGcontrolOutsideSearchWarning(Time){
   let TimeBottom;
   if(PAGEhashParameter("search")|| PAGEhashParameter("tags")){
      TimeBottom = LOGsearchedUpTo ? LOGsearchedUpTo : LOGscrollbarTopIndex;
   } else if(LOGendTimeArgs()){
      TimeBottom = LOGendTimeArgs();
   }
   else{
      TimeBottom = LOGscrollbarBottomIndex;
   }
   TimeBottom = String(TimeBottom);
   let Div = document.querySelector(".LOGwarningBanner");
   if(!Div) return;
   let WithinSearch = Time >= TimeBottom;
   if(WithinSearch || !Time || !(PAGEhashParameter("search") || PAGEhashParameter("tags"))){     // IX-4185 Assuming we need something for "tags"
      Div.classList.add("LOGhidden");
   }
   else{
      let html = `Warning: These logs are not part of the current search [<div class="LOGbeginSearchHere">Begin search from here?</div>]`;
      Div.classList.remove("LOGhidden");
      if(Div.innerHTML !== html) Div.innerHTML = html;
   }
}

function LOGscrollbarSetFilterBoundTop(){
   let Time = LOGstartTimeArgs();
   if(!Time) Time = LOGscrollbarTopIndex;
   let ScrollLine = document.querySelector(".LOGscrollbarFilterTop");
   LOGupdateLinePosition(ScrollLine, Time, -0.5);
}

function LOGscrollbarSetFilterBoundBottom(){
   let Time;
   if((PAGEhashParameter("search") || PAGEhashParameter("tags"))){
      Time = LOGsearchedUpTo ? LOGsearchedUpTo : LOGscrollbarTopIndex;
   } else if(LOGendTimeArgs()){
      Time = LOGendTimeArgs();
   }
   else{
      Time = LOGscrollbarBottomIndex;
   }
   if(Time < LOGendTimeArgs()){
      Time = LOGendTimeArgs();
   }
   let ScrollLine = document.querySelector(".LOGscrollbarFilterBottom");
   LOGupdateLinePosition(ScrollLine, Time, 0.25);
}

function LOGupdateLinePosition(Line, Time, Offset, Round = true){
   if(!Line) return;
   let Percent = 100 - ( Time - LOGscrollbarBottomIndex ) / (LOGscrollbarTopIndex - LOGscrollbarBottomIndex) * 100;
   Percent = Percent + Offset;
   if( Round && Percent > 97 ){
      Percent = 99.5;
   }
   let PercentString = Percent + "%";
   if(Line.style.top != PercentString) Line.style.top = PercentString;
}

function LOGscrollbarSetTopDate(){
   let Time = LOGstartTimeArgs();
   if(!Time) Time = LOGscrollbarTopIndex;
   let ScrollLine = document.querySelector(".LOGscrollbarLineTop");
   LOGupdateLinePosition(ScrollLine, Time, -0.5);
}

function LOGscrollbarSetTopDateTitle(){
   let Time = LOGstartTimeArgs();
   if(!Time) Time = LOGscrollbarTopIndex;
   let ScrollLine = document.querySelector(".LOGscrollbarLineTop");
   let html = "Start Time - " + LOGtimeFormat(Time);
   if(ScrollLine.title != html) ScrollLine.title = html;
}

function LOGscrollbarLineSelectedVisible(){
   let Visible = LOGscrollbarLineSelectedPosition;
   let ScrollLine = document.querySelector(".LOGscrollbarLineSelected");
   if(!ScrollLine) return;
   if( Visible) ScrollLine.classList.remove("LOGhidden");
   if(!Visible) ScrollLine.classList.add   ("LOGhidden");
}

function LOGscrollbarLineSelectedPositionUpdate(){
   let Time = LOGscrollbarLineSelectedPosition;
   let ScrollLine = document.querySelector(".LOGscrollbarLineSelected");
   LOGupdateLinePosition(ScrollLine, Time, 0);
}

function LOGscrollbarSetBottomDate(){
   let ScrollLine = document.querySelector(".LOGscrollbarLineBottom");
   if(!ScrollLine) return;
   let Time = LOGendTimeArgs();
   if(!Time) ScrollLine.classList.add   ("LOGhidden");
   else      ScrollLine.classList.remove("LOGhidden");
   LOGupdateLinePosition(ScrollLine, Time, 0.25);
}

function LOGscrollbarSetBottomDateTitle(){
   let Time = LOGendTimeArgs();
   let ScrollLine = document.querySelector(".LOGscrollbarLineBottom");
   let html = "End Time - " + LOGtimeFormat(Time);
   if(ScrollLine.title != html) ScrollLine.title = html;
}