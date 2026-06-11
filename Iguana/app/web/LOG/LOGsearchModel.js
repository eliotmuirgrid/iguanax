let LOGsearchResultsData = [];
let LOGsearchIndex;
let LOGsearchCriteria;
let LOGsearchIndexInitialized;

function LOGbeginSearchIfNeeded(){
   let Search = PAGEhashParameter("search");
   let Tags   = PAGEhashParameter("tags");
   if(!Search && !Tags) return;
   LOGsearchStartSearch()
}

function LOGsearchStartSearch(Callback){
   LOGsetFilterStatus("Searching ...");
   LOGsearchCriteria = LOGsearchArgs();
   MODELremove("LOGsearchModel");
   LOGsearchIndex = -1;
   LOGsearchResultsData = [];
   APIcall("logs/retrieve", LOGsearchArgs(), function(R){
      LOGsearchModelInit();
      if(Callback) Callback();
   });
}

function LOGsearchModelInit(){
   LOGsearchedUpTo = undefined;
   LOGsearchIndexInitialized = false;
   MODELpollerAdd("LOGsearchModel", "LOGsearchData", 0.5, "logs/retrieve/results", LOGpollerArgs);
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchAppend');
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchIndexInit');
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchResultCount');
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchStatus');
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchUpdateIndex');
   MODELviewAdd  ('LOGsearchModel', 'LOGsearchModelControl');
   MODELforcePoll('LOGsearchModel');
   MODELstart    ('LOGsearchModel');
}

function LOGsearchData()      { return MODELdata?.LOGsearchData?.data?.data || []; }
function LOGsearchDone()      { return MODELdata?.LOGsearchData?.data?.done;         }
function LOGsearchEndOfLogs() { return MODELdata?.LOGsearchData?.data?.end_of_logs;  }
function LOGsearchMaxReached(){ return MODELdata?.LOGsearchData?.data?.max_reached;  }

function LOGsearchNextFile(){
   let Time = MODELdata?.LOGsearchData?.data?.next_file;
   return Time != 0 ? Time : undefined;
}

function LOGsearchAppend(){
   let Data = LOGsearchData();
   LOGsearchResultsData = LOGsearchResultsData.concat(Data);
   MODELupdate("LOGscrollbarModel");
}

function LOGsearchStatus(){
   let html;
   if( LOGsearchDone() ) {
      html = "Search finished";
      html += LOGsearchEndOfLogs()  ? "!" : "";
      if(LOGsearchMaxReached()) html = "Max 1000 results."
   }
   else{
      let Time = LOGsearchNextFile();
      let Date = LOGdayFormatBanner(Time);
      if(Date.includes("Nth")) Date = "...";
      html = "Searching " + Date;
   }
   LOGsetFilterStatus(html);
}

function LOGsearchIndexInit(){
   if(LOGsearchIndexInitialized) return;
   let InitSearch = LOGsearchIndex == -1 && LOGsearchResultsData.length != 0;
   if(!InitSearch) return;
   let Index = LOGclosestSearchIndexToSelected()?.index;
   if(Index == undefined) Index = -1;
   LOGsearchIndex = Index;
   if(LOGsearchIndex == -1) return;
   LOGsearchIndexInitialized = true;
   LOGupdateSearchSelect();
}

function LOGupdateSearchSelect(clicked){
   if(!LOGsearchResultsData) return;
   let Data = LOGsearchResultsData[LOGsearchIndex];
   if(!Data) return;
   let Time = Data?.time;
   let Id = Data?.id;
   if(!Time || !Id) return;
   LOGjumpToEntry(Time, Id, clicked);
}

function LOGsearchResultCount(){
   let Input = document.querySelector(".LOGsearchIndexInput");
   let Total = document.querySelector(".LOGsearchTotal");
   if(!Input || !Total) return;
   
   let currentIndex = LOGsearchIndex === -1 ? "" : LOGsearchIndex + 1;
   let totalCount = PAGEhashParameter("filter") ? LOGshowListData.length : LOGsearchResultsData.length;
   
   if(Input.value !== currentIndex) Input.value = currentIndex;
   
   let totalText = " / ";
   if(LOGsearchDone() === false) totalText += "... ";
   totalText += totalCount;
   
   if(Total.innerHTML !== totalText) Total.innerHTML = totalText;
   
   if(totalCount > 0) {
      Input.max = totalCount;
      Input.disabled = false;
   } else {
      Input.disabled = true;
   }
}

function LOGclosestSearchIndexToSelected(){
   let Selected = LOGcurrentlySelected();
   if(!Selected) return;
   let Result = LOGfindClosestIndex(Selected.time, Selected.id, LOGsearchResultsData);
   return Result;
}

function LOGsearchModelControl(){
   if(LOGsearchDone()){
      MODELremove("LOGsearchModel");
   }
}

function LOGsearchUpdateIndex(){
   let Time = LOGsearchNextFile();
   let NewTime;
   if( LOGsearchDone() ){
      let Data =  LOGshowListData[LOGshowListData.length - 1];
      NewTime = Data?.time;
   }
   let NewestTime = Time;
   if(NewTime){
      NewestTime = NewTime < Time ? NewTime : Time;
   }
   LOGsearchedUpTo = NewestTime;
   MODELupdate("LOGscrollbarModel");
}
