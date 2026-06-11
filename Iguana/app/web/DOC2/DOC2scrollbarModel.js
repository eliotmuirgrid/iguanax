let DOCscrollbarLineSelectedPosition;
let DOCscrollbarTopIndex;
let DOCscrollbarBottomIndex;
function DOCscrollbarModelInit(){
   MODELpollerAdd("DOCscrollbarModel", "DOCscrollbarNewest"      , 60,  "doc/search/threaded",        DOCscrollbarNewestArgs);
   MODELpollerAdd("DOCscrollbarModel", "DOCscrollbarOldest"      , 120,  "doc/search/threaded",        DOCscrollbarOldestArgs);
   MODELviewAdd  ('DOCscrollbarModel', "DOCscrollbarSetTop");
   MODELviewAdd  ('DOCscrollbarModel', 'DOCscrollbarSetBottom');
   MODELviewAdd  ('DOCscrollbarModel', 'DOCscrollbarSetTopDateTitle');
   MODELviewAdd  ('DOCscrollbarModel', 'DOCscrollbarLineSelectedVisible');
   MODELviewAdd  ('DOCscrollbarModel', 'DOCscrollbarLineSelectedPositionUpdate');

   MODELforcePoll('DOCscrollbarModel', function(){
      DOCscrollbarPopout();
   });
   MODELstart("DOCscrollbarModel");
   MODELupdate('DOCscrollbarModel');
}

function DOCscrollbarNewestDoc(){
   let Data = MODELdata.DOCscrollbarNewest?.data?.[0] || [];
   let TimeHex = Data?.created || "";
   let Time = parseInt(TimeHex,16);
   return Time;
}

function DOCscrollbarOldestDoc(){
   let Data = MODELdata.DOCscrollbarOldest?.data?.[0] || [];
   let TimeHex = Data?.created|| "";
   let Time = parseInt(TimeHex,16);
   return Time;
}

function toSec(t) {
   const n = Number(t);
   // If it's greater than 1e12, assume it's in milliseconds → convert to seconds
   return n > 1e12 ? Math.floor(n / 1000) : n;
 }
 
function DOCscrollbarSetTop()   { DOCscrollbarTopIndex    = toSec(DOCscrollbarNewestDoc()); }
function DOCscrollbarSetBottom(){ DOCscrollbarBottomIndex = toSec(DOCscrollbarOldestDoc()); }

function DOCstartTimeArgs(){
   let Start = PAGEhashParameter("to");
   let End   = PAGEhashParameter("from");
   let Time  = Start;
   if (Start < End) { Time = End; }
   return Time;
}

function DOCscrollbarSetTopDateTitle(){
   let Time = DOCstartTimeArgs();
   if(!Time) Time = DOCscrollbarTopIndex;
   let ScrollLine = document.querySelector(".DOC2scrollbarLineTop");
   let html = "Start Time - " + DOC2convertDate(Time);
   if(ScrollLine.title != html) ScrollLine.title = html;
}

function DOCscrollbarLineSelectedVisible(){
   let Visible = DOCscrollbarLineSelectedPosition;
   let ScrollLine = document.querySelector(".DOC2scrollbarLineSelected");
   if(!ScrollLine) return;
   if( Visible) ScrollLine.classList.remove("DOC2hidden");
   if(!Visible) ScrollLine.classList.add   ("DOC2hidden");
}

function DOCscrollbarLineSelectedPositionUpdate(){
   let Time = DOCscrollbarLineSelectedPosition;
   let ScrollLine = document.querySelector(".DOC2scrollbarLineSelected");
   DOCupdateLinePosition(ScrollLine, Time, 0);
}

function DOCupdateLinePosition(Line, Time, Offset, Round = true){
   if(!Line) return;
   let Percent = 100 - ( Time - DOCscrollbarBottomIndex ) / (DOCscrollbarTopIndex - DOCscrollbarBottomIndex) * 100;
   Percent = Percent + Offset;
   if( Round && Percent > 97 ){
      Percent = 99.5;
   }
   let PercentString = Percent + "%";
   if(Line.style.top != PercentString) Line.style.top = PercentString;
}

function DOCscrollbarPopout(){
   let Scrollbar = document.querySelector('.DOC2scrollbarBox');
   if(!Scrollbar) return;
   POPtooltip({
      target: Scrollbar,
      content:`<div class="DOC2scrollbarPopout"></div>`, 
      style: 'DOC2timeTooltip DOC2hidden',
      position: POPtooltipPosition.RIGHT
   });
}

function DOCscrollbarPositionUpdate(){
   let TopDoc = document.querySelector('.DOCtopVisible');
   let Idx = !TopDoc ? 0 : DOC2listRowIndex(TopDoc);
   let Data = DOCshowListData[Idx];
   let Time = parseInt(Data?.created,16);
   let ScrollLine = document.querySelector(".DOC2scrollbarLine");
   DOCupdateLinePosition(ScrollLine, Time, 0);
}