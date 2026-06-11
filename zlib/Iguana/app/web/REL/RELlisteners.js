function RELsetListeners(){
   let Container = document.querySelector('.RELmodal');
   Container.addEventListener("dblclick", RELclick);
}

function RELclick(e){
   if(e.target.closest('.RELupstreamEntry')) return RELupstreamClicked(e.target);
   if(e.target.closest('.RELlogEntry'))      return RELlogClicked(e.target);
}

function RELupstreamClicked(target){
   let Data = RELtargetToUpstreamData(target);
   let Args = {};
   Args.component = Data.comp;
   Args.time      = Data.time;
   Args.id        = Data.id;
   Args.search    = PAGEhashParameter("search");
   Args.whole_word = PAGEhashParameter("whole_word");
   Args.filter    = PAGEhashParameter("filter");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   Args.type      = PAGEhashParameter("type");
   window.location.hash = `logs/details${PAGEformatParams(Args)}`;
}

function RELlogClicked(target){
   let Data = RELtargetToLogData(target);
   let Args = {};
   Args.component = Data.comp;
   Args.time      = Data.time;
   Args.id        = Data.id;
   Args.search    = PAGEhashParameter("search");
   Args.whole_word = PAGEhashParameter("whole_word");
   Args.filter    = PAGEhashParameter("filter");
   Args.from      = PAGEhashParameter("from");
   Args.to        = PAGEhashParameter("to");
   Args.type      = PAGEhashParameter("type");
   window.location.hash = `logs/details${PAGEformatParams(Args)}`;
}

function RELtargetToLogData(target){
   if(!target) return;
   let ListData = RELrelatedListData;
   const LogEntry = target.closest('.RELlogEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.RELlogEntry'));
   const Index = logEntries.indexOf(LogEntry);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}

function RELtargetToUpstreamData(target){
   if(!target) return;
   let ListData = RELupstreamListData;
   const LogEntry = target.closest('.RELupstreamEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.RELupstreamEntry'));
   const Index = logEntries.indexOf(LogEntry);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}