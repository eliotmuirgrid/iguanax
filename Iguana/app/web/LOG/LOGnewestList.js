function LOGrenderNewest(Callback){
   LOGstartOfLogs = true;
   LOGendOfLogs = false;
   LOGclearLogs();
   LOGupdateBottomBanner("Loading results ...");
   APIcall("logs/retrieve", LOGnewestArgs(), function(R){
      LOGshowListData = R.data || [];
      LOGreceivedData = true;
      LOGstartOfLogs = true;
      LOGendOfLogs = false;
      LOGshowListRender();
      LOGscrollToTop();
      let TopLog = document.querySelectorAll(".LOGlogEntry")[0];
      LOGentryClicked(TopLog);
      LOGupdateBottomBanner("Oldest results");
      if(Callback) Callback();
   });
}

function LOGscrollToTop(){
   let Log = document.querySelector('.LOGtopBanner');
   if(!Log) return;
   Log.scrollIntoView(true);
}

function LOGscrollToBottom(){
   let Logs = document.querySelectorAll('.LOGlogEntry');
   let Bottom = Logs[Logs.length - 1];
   LOGlimitScrollSpeed = false;
   Bottom.scrollIntoView(true);
   setTimeout(function(){ LOGlimitScrollSpeed = true; }, 300);
}