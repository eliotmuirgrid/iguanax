function LOGfilterIndexDownClicked(){
   LOGsearchIndex = LOGsearchIndex + 1;
   if(LOGsearchIndex >= LOGshowListData.length){
      LOGsearchIndex = 0;
   }
   if(!LOGshowListData.length){
      LOGsearchIndex = -1;
   }
   LOGupdateFilterSelect();
   LOGfilterResultCount();
}

function LOGfilterIndexUpClicked(){
   LOGsearchIndex = LOGsearchIndex - 1;
   if(LOGsearchIndex < 0){
      LOGsearchIndex = LOGshowListData.length - 1;
   }
   if(!LOGshowListData.length){
      LOGsearchIndex = -1;
   }
   LOGupdateFilterSelect();
   LOGfilterResultCount();
}

function LOGupdateFilterSelect(){
   let Logs = document.querySelectorAll(".LOGlogEntry");
   if(!Logs) return;
   let LogEntry = Logs[LOGsearchIndex];
   if(!LogEntry) return;
   LogEntry.scrollIntoView({ block: 'center', behavior: 'smooth' });
   LOGfocusOnLog(LogEntry);
}