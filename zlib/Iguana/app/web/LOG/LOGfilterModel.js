let LOGselectedScrolledTo = false;
let LOGsearchedUpTo;

function LOGrenderFiltered(){
   LOGclearLogs();
   LOGfilterStartSearch(LOGfilterModelInit);
}

function LOGfilterStartSearch(Callback){
   LOGsetFilterStatus("Searching ...");
   LOGsearchIndex = -1;
   APIcall("logs/retrieve", LOGfilterArgs(), function(R){
      Callback();
   });
}

function LOGsetFilterStatus(Message){
   let Div = document.querySelector(".LOGsearchStatus");
   if(!Div) return;
   if(Div.innerHTML != Message) Div.innerHTML = Message;
}

function LOGfilterModelInit(){
   LOGsearchedUpTo = undefined;
   LOGselectedScrolledTo = false;
   MODELpollerAdd("LOGfilterModel", "LOGfilterData", 0.5, "logs/retrieve/results", LOGpollerArgs);
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterAppend');
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterStatus');  
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterBannerUpdate');  
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterUpdateIndex');
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterResultCount');
   // MODELviewAdd  ('LOGfilterModel', 'LOGfilterIndexInit');
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterSelectFirstResult');
   MODELviewAdd  ('LOGfilterModel', 'LOGfilterModelControl');
   MODELforcePoll('LOGfilterModel');
   MODELstart    ('LOGfilterModel');
}

function LOGfilterData(){
   return MODELdata?.LOGfilterData?.data?.data || [];
}

function LOGfilterDone(){
   return MODELdata?.LOGfilterData?.data?.done;
}

function LOGfilterEndOfLogs(){
   return MODELdata?.LOGfilterData?.data?.end_of_logs;
}

function LOGfilterMaxReached(){
   return MODELdata?.LOGfilterData?.data?.max_reached;
}

function LOGfilterNextFile(){
   let Time = MODELdata?.LOGfilterData?.data?.next_file;
   return Time != 0 ? Time : undefined;
}

function LOGfilterAppend(){
   let Data = LOGfilterData();
   LOGshowListAppend(Data);
   LOGselectTopAndBottom();
   LOGupdateBottomBanner("Loading results ...");
   MODELupdate("LOGmodel");
   MODELupdate("LOGscrollbarModel");
}

function LOGupdateBottomBanner(Message){
   let Div = document.querySelector(".LOGbottomBanner");
   if(!Div) return;
   if(Div.innerHTML != Message) Div.innerHTML = Message;
}

function LOGfilterIndexInit(){
   if(LOGsearchIndexInitialized) return;
   let InitSearch = LOGsearchIndex == -1 && LOGshowListData.length != 0;
   if(!InitSearch) return;
   LOGsearchIndex = 0;
   LOGupdateFilterSelect();
}

function LOGfilterResultCount() {
   let Input = document.querySelector(".LOGsearchIndexInput");
   let Total = document.querySelector(".LOGsearchTotal");
   if(!Input || !Total) { return; }

   let currentIndex = LOGsearchIndex === -1 ? "" : LOGsearchIndex + 1;
   let totalCount   = LOGshowListData.length;
   if(Input.value !== currentIndex) { Input.value = currentIndex; }

   let totalText = " / ";
   if(LOGfilterDone() === false) { totalText += "... "; }
   totalText += totalCount;

   if(Total.innerHTML !== totalText) { Total.innerHTML = totalText; }

   if(totalCount > 0) {
      Input.max      = totalCount;
      Input.disabled = false;
   } else {
      Input.disabled = true;
   }
}

function LOGfilterUpdateIndex(){
   let Time = LOGfilterNextFile();
   let NewTime;
   if( LOGfilterDone() ){
      let Data =  LOGshowListData[LOGshowListData.length - 1];
      NewTime = Data?.time;
   }
   let NewestTime = Time;
   if(NewTime){
      NewestTime = NewTime < Time ? NewTime : Time;
   }
   LOGsearchedUpTo = NewestTime;
}

function LOGfilterSelectFirstResult(){
   if(LOGselectedScrolledTo) return;
   let div = document.querySelector(".LOGentrySelected");
   if(!div) return;
   let Time = PAGEhashParameter("time");
   let Id  = PAGEhashParameter("id");
   let idx = LOGbinarySearch(Time, Id);
   LOGselectedScrolledTo = true;
   LOGsearchIndex = idx;
   LOGscrollToEntry(idx);
   LOGsearchResultCount();
}

function LOGfilterStatus(){
   let html;
   if( LOGfilterDone() ) {
      html = "Search finished";
      html += LOGfilterEndOfLogs()  ? "!" : "";
      if(LOGfilterMaxReached()) html = "Max 1000 results. [<div class='LOGfilterLoadMore'>Load more</div>]";
   }
   else{
      let Time = LOGfilterNextFile();
      let Date = LOGdayFormatBanner(Time);
      if(Date.includes("Nth")) Date = "...";
      html = "Searching " + Date;
   }
   LOGsetFilterStatus(html);
}

function LOGfilterBannerUpdate(){
   if( !LOGfilterDone() ) return;
   let Message = "";
   if(LOGfilterEndOfLogs())  Message = "Oldest results";
   if(LOGfilterMaxReached()) Message = "<div class='LOGfilterLoadMore'>Click to load more results</div>";
   LOGupdateBottomBanner(Message);
}

function LOGfilterModelControl(){
   if(LOGfilterDone()){
      if(!LOGselectedScrolledTo) LOGfilterIndexInit()
      MODELremove("LOGfilterModel");
   }
}

function LOGfilteringLogs(){
   return PAGEhashParameter("filter") === 'true';
}

function LOGfilterScrollbarClicked(time){
   let Result = LOGfindClosestIndex(time, 1, LOGshowListData);
   let Logs = document.querySelectorAll(".LOGlogEntry");
   let LogItem = Logs[Result.index];
   if(!LogItem) return;
   LogItem.scrollIntoView({ block: 'center', behavior: 'smooth' });
   LOGfocusOnLog(LogItem);
   LOGentryClicked(LogItem);
}