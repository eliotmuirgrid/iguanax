function LOGsimpleArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.search    = PAGEhashParameter("search");
   Args.exact     = PAGEhashParameter("exact");
   Args.tags      = PAGEhashParameter("tags");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   Args.to        = PAGEhashParameter("to");
   Args.from      = PAGEhashParameter("from");
   return Args;
}

function LOGbasicArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.search    = PAGEhashParameter("search");
   Args.exact     = PAGEhashParameter("exact");
   Args.tags      = PAGEhashParameter("tags");
   Args.limit     = 1000;
   return Args;
}

function LOGbasicDownArgs(){
   let Args = LOGbasicArgs();
   let Time = PAGEhashParameter("time");
   let Id   = PAGEhashParameter("id");
   let To   = PAGEhashParameter("to");
   let From = PAGEhashParameter("from");
   if      (Time) Args.start = Time;
   else if   (To) Args.start = To;
   if(Time && Id) Args.start_id = Id;
   if      (From) Args.end = From;
   return Args;
}

function LOGinitArgsDown(StartTime, StartId){
   let Args = LOGbasicDownArgs();
   Args.include_non_matches = true;
   Args.formatted = true;
   if(StartTime) Args.start    = StartTime;
   if(StartId)   Args.start_id = StartId;
   return Args;
}

function LOGbasicUpArgs(){
   let Args = LOGbasicArgs();
   let Time = PAGEhashParameter("time");
   let Id   = PAGEhashParameter("id");
   let To   = PAGEhashParameter("to");
   let From = PAGEhashParameter("from");
   if      (Time) Args.start = Time;
   else if (From) Args.start = From;
   if(Time && From && Time < From)  Args.start = From;
   if(Time && Id) Args.start_id = Id;
   if        (To) Args.end = To;
   return Args;
}

function LOGinitArgsUp(StartTime, StartId){
   let Args = LOGbasicUpArgs();
   Args.include_non_matches = true;
   Args.formatted = true;
   Args.exclusive = true;
   Args.forward   = true;
   Args.limit     = 800;
   if(StartTime) Args.start    = StartTime;
   if(StartId)   Args.start_id = StartId;
   return Args;
}

function LOGsearchArgs(){
   let Args = LOGbasicArgs();
   let To   = PAGEhashParameter("to");
   let From = PAGEhashParameter("from");
   Args.start  = To;
   Args.end    = From;
   // Args.minify = true;
   Args.job    = SESSgetTabGuid();
   Args.limit  = 1000;
   return Args;
}

function LOGfilterArgs(){
   let Args = LOGbasicArgs();
   let To   = PAGEhashParameter("to");
   let From = PAGEhashParameter("from");
   Args.start = To;
   Args.end   = From;
   Args.job   = SESSgetTabGuid();
   Args.limit  = 1000;
   return Args;
}

function LOGpollerArgs(){
   let Args = {};
   Args.job = SESSgetTabGuid();
   return Args;
}

function LOGfilterLoadMoreClicked(){
   let ListData = LOGshowListData;
   const Item = ListData[ListData.length - 1];
   let Time = Item?.time || LOGfilterNextFile();
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.from      = PAGEhashParameter("from");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.to        = Time;
   Args.filter    = PAGEhashParameter("filter");
   Args.exact     = PAGEhashParameter("exact");
   window.location.hash = `logs${PAGEformatParams(Args)}`;
}

function LOGlogEntrySelected(e) {
   const Item = LOGtargetToItemData(e.target);
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.time      = Item?.time;
   Args.id        = Item?.id;
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.filter    = PAGEhashParameter("filter");
   Args.exact     = PAGEhashParameter("exact");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   window.location.hash = `logs/details${PAGEformatParams(Args)}`;
}

function LOGupdateStartTimeUrl(time){
   let Args = {};
   Args.component  = PAGEhashParameter("component");
   Args.from       = PAGEhashParameter("from");
   Args.id         = "1";
   Args.time       = PAGEhashParameter("time");
   Args.search     = PAGEhashParameter("search");
   Args.tags       = PAGEhashParameter("tags");
   Args.filter     = PAGEhashParameter("filter");
   Args.exact      = PAGEhashParameter("exact");
   Args.to         = time;
   window.location.hash = `logs${PAGEformatParams(Args)}`;
   LOGinputControl();
}

function LOGupdateEndTimeUrl(time){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.to        = PAGEhashParameter("to");
   Args.id        = "1";
   Args.time      = PAGEhashParameter("time");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.filter    = PAGEhashParameter("filter");
   Args.exact     = PAGEhashParameter("exact");
   Args.from      = time;
   Args.type      = PAGEhashParameter("type");
   window.location.hash = `logs${PAGEformatParams(Args)}`;
   LOGinputControl();
}

function LOGupdateTimeUrl(time){
   let End   = PAGEhashParameter("from");
   let Start = PAGEhashParameter("to");
   if(End && time < End)     return LOGbottomDateLineClicked();
   if(Start && time > Start) return LOGtopDateLineClicked();
   let Args = {};
   Args.component  = PAGEhashParameter("component");
   Args.time       = time;
   Args.id         = "1";
   Args.search     = PAGEhashParameter("search");
   Args.tags       = PAGEhashParameter("tags");
   Args.filter     = PAGEhashParameter("filter");
   Args.exact = PAGEhashParameter("exact");
   Args.from       = PAGEhashParameter("from");
   Args.to         = PAGEhashParameter("to");
   Args.type       = PAGEhashParameter("type");
   window.location.hash = `logs${PAGEformatParams(Args)}`;
}

function LOGlastErrorArgs() {
   let Args           = {};
   Args.component     = LOGcomponentId();
   Args.tags          = "#error";
   Args.exact         = true;
   Args.limit         = 1;
   Args.critical      = false
   return Args;
}

function LOGloadNewestLogsClicked(){
   if(LOGfilteringLogs()) return LOGscrollToTop();
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.filter    = PAGEhashParameter("filter");
   Args.exact     = PAGEhashParameter("exact");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   let hash =  `#logs${PAGEformatParams(Args)}`;
   let oldhash = window.location.hash;
   if(oldhash != hash) window.location.hash = hash;
   else                LOGscreen();
}

function LOGnewestArgs(){
   let Args = LOGbasicArgs()
   Args.include_non_matches = true;
   Args.formatted = true;
   let To = PAGEhashParameter("to");
   let From = PAGEhashParameter("from");
   if(To)   Args.start = To;
   if(From) Args.end   = From;
   return Args;
}

function LOGloadOldestLogsClicked(){
   if(LOGfilteringLogs()) return LOGscrollToBottom();
   let Start = PAGEhashParameter("to");
   let End = PAGEhashParameter("from");
   let Time = End;
   if (Start && End && Start < End) {
      Time = Start;
   }
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.time      = Time ? Time : LOGscrollbarBottomIndex;
   Args.id        = 1;
   Args.filter    = PAGEhashParameter("filter");
   Args.exact     = PAGEhashParameter("exact");
   Args.from      = End;
   Args.to        = Start;
   Args.type      = PAGEhashParameter("type");

   let hash =  `#logs${PAGEformatParams(Args)}`;
   let oldhash = window.location.hash;
   if(oldhash != hash) window.location.hash = hash;
   else                LOGscreen();
}

// function LOGwholewordResultsSelected(){
//    let Checked = LOGwholewordResults();
//    let Value = Checked ? true : "";
//    let Item = LOGcurrentlySelected();
//    let Args = {};
//    Args.component  = PAGEhashParameter("component");
//    Args.to         = PAGEhashParameter("to");
//    Args.from       = PAGEhashParameter("from");
//    Args.time       = Item?.time;
//    Args.id         = Item?.id;
//    Args.search     = PAGEhashParameter("search");
//    Args.tags       = PAGEhashParameter("tags");
//    Args.filter     = PAGEhashParameter("filter");
//    Args.exact      = Value;
//    Args.type       = PAGEhashParameter("type");
//    window.location.hash = `logs${PAGEformatParams(Args)}`;
// }

// function LOGfilterResultsSelected(){
//    let Checked = LOGfilterResults();
//    let Value = Checked ? true : "";
//    let Args = {};
//    Args.component  = PAGEhashParameter("component");
//    Args.to         = PAGEhashParameter("to");
//    Args.time       = PAGEhashParameter("time");
//    Args.id         = PAGEhashParameter("id");
//    Args.from       = PAGEhashParameter("from");
//    Args.search     = PAGEhashParameter("search");
//    Args.tags       = PAGEhashParameter("tags");
//    Args.filter     = Value;
//    Args.exact      = PAGEhashParameter("exact");
//    window.location.hash = `logs${PAGEformatParams(Args)}`;
// }

// function LOGclearDatesClicked(){
//    let Args = {};
//    Args.component  = PAGEhashParameter("component");
//    Args.time       = PAGEhashParameter("time");
//    Args.id         = PAGEhashParameter("id");
//    Args.search     = PAGEhashParameter("search");
//    Args.tags       = PAGEhashParameter("tags");
//    Args.filter     = PAGEhashParameter("filter");
//    Args.exact      = PAGEhashParameter("exact");
//    window.location.hash = `logs${PAGEformatParams(Args)}`;
// }

function LOGargsGetAsync(){
   let Args = {};
   Args.job = SESSgetTabGuid();
   return Args;
}

function LOGbeginSearchHereClicked(){
   let TopLog = document.querySelector('.LOGtopVisible');
   let Idx = !TopLog ? 0 : LOGentryIndex(TopLog);
   let Data = LOGshowListData[Idx];
   let Time = Data?.time;
   let Args = {};
   Args.component  = PAGEhashParameter("component");
   Args.from       = PAGEhashParameter("from");
   Args.id         = "1";
   Args.search     = PAGEhashParameter("search");
   Args.tags       = PAGEhashParameter("tags");
   Args.to         = Time;
   Args.filter     = PAGEhashParameter("filter");
   Args.exact      = PAGEhashParameter("exact");
   Args.type       = PAGEhashParameter("type");
   window.location.hash = `logs${PAGEformatParams(Args)}`;
}

function LOGprevArgs(){
   let Item = LOGoldestItem();
   let Args = LOGbasicArgs();
   Args.start     = Item.time;
   Args.start_id  = Item.id;
   Args.end       = PAGEhashParameter("from");
   Args.formatted = true;
   Args.include_non_matches = true;
   Args.exclusive = true;
   Args.limit     = 800;
   return Args;
}

function LOGnextArgs(){
   let Item = LOGnewestItem();
   let Args = LOGbasicArgs();
   Args.start     = Item.time;
   Args.start_id  = Item.id;
   Args.end       = PAGEhashParameter("to");
   Args.formatted = true;
   Args.include_non_matches = true;
   Args.exclusive = true;
   Args.forward   = true;
   Args.limit     = 800;
   return Args;
}

function LOGscrollbarNewestArgs(){
   let Args = {};
   Args.component = LOGcomponentId();
   Args.limit     = 1;
   return Args;
}

function LOGscrollbarOldestArgs(){
   let Args = LOGscrollbarNewestArgs();
   Args.forward = true;
   return Args;
}