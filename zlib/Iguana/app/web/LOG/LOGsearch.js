let LOGsearchTimeout;

function LOGjumpToEntry(LogTime, LogId, clicked){
   let LogIndex = LOGbinarySearch(LogTime, LogId);
   if(LogIndex != -1 && LogIndex < 1520) LOGscrollToEntry(LogIndex, clicked);
   else if(!LOGendOfLogs){
      LOGresetRenderedGlobals();
      if(!LOGactiveFiltering()) LOGinitRenderDown(LogTime, LogId);
   }
}

function LOGresetRenderedGlobals(){
   LOGmodelInit();
   LOGupdateFilterCheckbox();
   LOGupdateWholeWordCheckbox();
   LOGsetStartTime();
   LOGsetEndTime();
   LOGresetGlobals();
   LOGupdateBottomBanner("Oldest results");
   LOGvisibility();
   LOGscrollbarModelInit();
}

function LOGscrollToEntry(LogIndex, clicked){
   let LogList = document.querySelectorAll(".LOGlogEntry");
   if(!LogList) return;
   let LogEntry = LogList[LogIndex];
   if(!LogEntry) return;
   LOGlimitScrollSpeed = false;
   if(!clicked) LogEntry.scrollIntoView({ block: 'center', behavior: 'smooth' });
   LOGfocusOnLog(LogEntry);
}

function LOGtextVal(className) {
   let Val = document.querySelector(`.${className}`) && document.querySelector(`.${className}`).value;
   if(Val === "") Val = "";
   return Val;
}

function LOGtagsText() {
   return document.querySelector('.LOGtagsInput') && document.querySelector('.LOGtagsInput').value;
}

function LOGsearchInput(){
   let NewFilter  = LOGtextVal("LOGsearchInput");
   let TagsFilter = LOGtextVal("LOGtagsInput");
   let Start = LOGstartTimeValue();
   let End   = LOGendTimeValue();
   let Exact = LOGwholewordResults() ? true : "";
   let Filter = LOGfilterResults() ? true : "";
   LOGsearchInputUrlUpdate(NewFilter, TagsFilter, Start, End, Exact, Filter);
   // LOGsearchIndex = undefined;
}

function LOGsearchInputUrlUpdate(NewFilter, TagsFilter, Start, End, Exact, Filter){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.filter    = Filter;
   Args.exact     = Exact;
   Args.search    = NewFilter;
   Args.tags      = TagsFilter;
   Args.to        = Start;
   Args.from      = End;
   window.location.hash = "#logs" + PAGEformatParams(Args);
}

function LOGclearInput(ClassName){
   let div = document.querySelector(`.${ClassName}`);
   if(!div) return;
   div.value = "";
   LOGinputControl();
   FORMfocus(div);
}