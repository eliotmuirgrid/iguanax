function LOGactiveFiltering(){
   let Filter = PAGEhashParameter("filter");
   let Search = PAGEhashParameter("search");
   let Tags = PAGEhashParameter("tags");
   return Filter && (Search || Tags);
}
function LOGinit(Callback){
   if(LOGactiveFiltering()) return LOGrenderFiltered();   
   let Time = PAGEhashParameter("time");
   let Id   = PAGEhashParameter("id");
   if(!Time || !Id) return LOGrenderNewest(Callback); // this works well
   LOGinitRenderDown(undefined, undefined, Callback); // this is missing things for scrollbar init pos etc
}

function LOGclearLogs(){
   document.querySelector(".LOGlistDiv").innerHTML = "";
   LOGshowListData = [];
}

function LOGinitRenderDown(LogTime, LogId, Callback){
   let Args = LOGinitArgsDown(LogTime, LogId);
   LOGinProgress = true;
   APIcall("logs/retrieve", Args, function(R){
      LOGinProgress = false;
      LOGclearLogs();
      let Data = R.data || [];
      LOGshowListAppend(Data);
      LOGendOfLogs = Args.limit > Data.length;
      LOGinitRenderUp(LogTime, LogId);
      LOGselectTopAndBottom();
      LOGreceivedData = true;
      if(Callback) Callback();
   });
}

function LOGinitRenderUp(StartTime, StartId){
   let Args = LOGinitArgsUp(StartTime, StartId);
   LOGinProgress = true;
   APIcall("logs/retrieve", Args, function(R){
      LOGinProgress = false;
      LOGreceivedData = true;
      // console.log("init render up");
      let TopLog = document.querySelectorAll(".LOGlogEntry")[0];
      let Data = R.data || [];
      LOGshowListPrepend(Data);
      LOGstartOfLogs = Data.length == 0;
      LOGentryClicked(TopLog);
      if(TopLog) return TopLog.scrollIntoView({ behavior: "instant", block: "center"});
      let Logs = document.querySelectorAll(".LOGlogEntry");
      let BottomLog = Logs[Logs.length - 1];
      if(!BottomLog) return;
      BottomLog.scrollIntoView({ behavior: "instant", block: "center"});
      LOGentryClicked(BottomLog);
   });
}

function LOGfocusOnLog(Log){
   let ExistingSelect = document.querySelector(".LOGentrySelected");
   if(ExistingSelect) ExistingSelect.classList.remove("LOGentrySelected");
   if(Log) Log.classList.add("LOGentrySelected");
   let Item = LOGtargetToItemData(Log);
   let Time = Item?.time;
   if(Time) LOGscrollbarLineSelectedPosition = Time;
   LOGscrollbarPositionUpdate();
   MODELupdate("LOGscrollbarModel");
}

function LOGloadNewerLogsClicked(Amount = 1){
   let SelectedIdx = LOGcurrentlySelectedIndex();
   let NewIdx = SelectedIdx - Amount;
   if(NewIdx < 0) NewIdx = 0;
   let Logs = document.querySelectorAll(".LOGlogEntry");
   if(!Logs) return;
   let LogItem = Logs[NewIdx];
   if(!LogItem) return;
   // if(NewIdx <= 280){
   //    let LogData = LOGshowListData[NewIdx];
   //    LOGresetRenderedGlobals();
   //    LOGinitRenderDown(LogData.time, LogData.id);
   // }
   // else{
      LOGentryClicked(LogItem);
      LogItem.scrollIntoView({behavior : "smooth", block: "center"})
   // }
}

function LOGloadOlderLogsClicked(Amount = 1){
   let SelectedIdx = LOGcurrentlySelectedIndex();
   let NewIdx = SelectedIdx + Amount;
   if(NewIdx >= LOGshowListData.length ) NewIdx = LOGshowListData.length - 1;
   let Logs = document.querySelectorAll(".LOGlogEntry");
   if(!Logs) return;
   let LogItem = Logs[NewIdx];
   if(!LogItem) return;
   if(NewIdx >= 1520){
      let LogData = LOGshowListData[NewIdx];
      LOGresetRenderedGlobals();
      LOGinitRenderDown(LogData.time, LogData.id);
   }
   else{
      LOGentryClicked(LogItem);
      LogItem.scrollIntoView({behavior : "auto", block: "center"})
   }
}