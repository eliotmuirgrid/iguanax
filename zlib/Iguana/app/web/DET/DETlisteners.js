function DETsetListeners(){
   let Container = document.querySelector('.DETwindow');
   Container.addEventListener('click', DETclickHandler);
   Container.addEventListener('dblclick', DETdblclickHandler);
   Container.addEventListener('change', DETonChangeHandler);
   Container.addEventListener('input',  DETonInputHandler);
   FORMenterListener(Container.querySelector(".DETsearchNumber"), DETsearchNumberInput);
   FORMenterListener(Container.querySelector(".DETsearchInput"),  DETsearchInputEnter);
}

function DETclickHandler(e){
   if(e.target.closest('.DETdisabled'))         return;
   if(e.target.closest('.DETlight .TOGbutton')) return DETswitchClicked();
   if(e.target.closest('.DETexport'))           return DETexportScreen();
   if(e.target.closest('.DETreposition'))       return DETrepositionScreen();
   if(e.target.closest('.DETresubmitOrig'))     return BLKresubmit();
   if(e.target.closest('.DETresubmit'))         return RSBrender();
   if(e.target.closest('.DETlogEntry'))         return DETlogClicked(e.target);
   if(e.target.closest('.DETwrap'))             return DETwrapClicked();
   if(e.target.closest('.DETsearchIndexDown'))  return DETdownIdxClicked();
   if(e.target.closest('.DETsearchIndexUp'))    return DETupIdxClicked();
}

function DETdblclickHandler(e){
   if(e.target.closest('.DETupstreamEntry')) return DETupstreamClicked(e.target);
}

function DETlogClicked(target){
   let Data = DETtargetToLogData(target);
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

function DETupstreamClicked(target){
   let Data = DETtargetToUpstreamData(target);
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

function DETwrapClicked() {
   const contentDiv = document.querySelector('.DETdetailContent');
   const computedStyle = getComputedStyle(contentDiv);
   const whiteSpace = computedStyle.getPropertyValue('white-space');
   contentDiv.style.whiteSpace = whiteSpace === 'nowrap' ? 'pre-wrap' : 'nowrap'
}

function DETtargetToLogData(target){
   if(!target) return;
   let ListData = DETrelatedListData;
   const LogEntry = target.closest('.DETlogEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.DETlogEntry'));
   const Index = logEntries.indexOf(LogEntry);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}

function DETtargetToUpstreamData(target){
   if(!target) return;
   let ListData = DETupstreamListData;
   const LogEntry = target.closest('.DETupstreamEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.DETupstreamEntry'));
   const Index = logEntries.indexOf(LogEntry);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}

function DETonChangeHandler(e){
   if(e.target.closest(".DETstringView")) return DETviewModeChanged();
}

function DETviewModeChanged(){
   let Div = document.querySelector('.DETstringView');
   if(!Div) return;
   let Value = Div.value;
   STORElocalCreate("DETselectedViewMode", Value);
   DETrememberScroll();
   MODELremove("DETfindModel");
   MODELremove("DETmessageModel");
   DETmessageModelInit();
   DETfindExecute();
}

function DETonInputHandler(e){
   if(e.target.closest(".DETsearchInput")) return DETfindInput();
}