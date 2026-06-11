let LOGrenderDown = false;
let LOGrenderUp   = false;
let LOGdownIndex;
let LOGupIndex;
let LOGscrollTimerDown;
let LOGscrollTimerDate;
let LOGflagResetTimer;
let LOGscrollTimerUp;
let LOGstartOfLogs = false;
let LOGendOfLogs = false;
let LOGlimitScrollSpeed = true;
let LOGinProgress = false;

function LOGselectTopAndBottom(){
  // need logic in here to prevent rendering of more when we scroll
   clearTimeout(LOGscrollTimerDown);
   LOGscrollTimerDown = setTimeout(function(){ 
      LOGselectBottomVisibleElement();
      LOGbottomViewPosition();
      LOGupdateRenderMoreDown();
      LOGrenderingMoreDown();
   }, 15);
   clearTimeout(LOGscrollTimerUp);
   LOGscrollTimerUp = setTimeout(function(){ 
      LOGloadNewLogs();
   }, 17);
   clearTimeout(LOGflagResetTimer);
   LOGflagResetTimer = setTimeout(function(){ 
      LOGresetTopAndBottomFlags();
   }, 60);
   LOGscrollTimerDate = setTimeout(function(){ 
      LOGupdateDate();
      LOGscrollbarPositionUpdate();
   }, 23);
}

function LOGsetScrollSpeed(){
   // Get a reference to the scrollable div
   const scrollableDiv = document.querySelector('.LOGbody');
   // Get a reference to the scrollable div
   // Set a maximum scroll speed (adjust as needed)
   const maxScrollSpeed = 5; // Change this value to control the speed

   let lastScrollTime = 0;
   let lastScrollTop = 0;

   scrollableDiv.addEventListener('scroll', function(event) {
      const now = Date.now();
      const deltaTime = now - lastScrollTime;
      const scrollTop = scrollableDiv.scrollTop;
      LOGlimitScrollSpeed = false;
      if (LOGlimitScrollSpeed && deltaTime > 0) {
         // Calculate the scroll speed
         const scrollSpeed = Math.abs(scrollTop - lastScrollTop) / deltaTime;

         if (scrollSpeed > maxScrollSpeed) {
         // If the scroll speed exceeds the maximum, prevent further scrolling
         event.preventDefault();

         // Optionally, you can set a new scrollTop value to limit how far it can scroll in one event
         const newScrollTop = lastScrollTop + (maxScrollSpeed * deltaTime * (scrollTop > lastScrollTop ? 1 : -1));
         scrollableDiv.scrollTop = newScrollTop;
         }
      }

      lastScrollTime = now;
      lastScrollTop = scrollTop;
   });
}

function LOGloadNewLogs(){
   LOGselectTopVisibleElement();
   LOGtopViewPosition();
   LOGupdateRenderMoreUp();
   LOGrenderingMoreUp();
}

function LOGresetTopAndBottomFlags(){
   LOGstartOfLogs = false;
   LOGendOfLogs   = false;
}

function LOGbodyOffset(){
   return document.querySelector(".LOGbody").getBoundingClientRect().top - 40;
}

function LOGselectTopVisibleElement() {
   const parentDiv = document.querySelector(".LOGbody");
   const scrollPosition = parentDiv.scrollTop + LOGbodyOffset();
   const logEntries = parentDiv.querySelectorAll(".LOGlogEntry");

   let topElement = null;
   let smallestOffset = Infinity;

   logEntries.forEach((logEntry) => {
      const offset = logEntry.offsetTop;
      if (offset >= scrollPosition && offset < smallestOffset) {
         smallestOffset = offset;
         topElement = logEntry;
      }
   });

   if (topElement) {
      logEntries.forEach((logEntry) => {
         logEntry.classList.remove("LOGtopVisible");
      });
      topElement.classList.add("LOGtopVisible");
   }
}

function LOGselectBottomVisibleElement() {
   const parentDiv = document.querySelector(".LOGbody");
   const scrollPosition = parentDiv.scrollTop + parentDiv.clientHeight + LOGbodyOffset();
   const logEntries = parentDiv.querySelectorAll(".LOGlogEntry");

   let bottomElement = null;
   let largestOffset = -Infinity;

   logEntries.forEach((logEntry) => {
      const offset = logEntry.offsetTop;
      const entryHeight = logEntry.offsetHeight;
      const bottomPosition = offset + entryHeight;
      
      if (bottomPosition <= scrollPosition && bottomPosition > largestOffset) {
         largestOffset = bottomPosition;
         bottomElement = logEntry;
      }
   });

   if (bottomElement) {
      logEntries.forEach((logEntry) => {
         logEntry.classList.remove("LOGbottomVisible");
      });
      bottomElement.classList.add("LOGbottomVisible");
   }
}

function LOGentryIndex(target){
   const LogEntry = target.closest('.LOGlogEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.LOGlogEntry'));
   const Index = logEntries.indexOf(LogEntry);
   return Index;
}

function LOGtopViewPosition(){
   let TopLog = document.querySelector('.LOGtopVisible');
   LOGupIndex = !TopLog ? 0 : LOGentryIndex(TopLog);
   return !TopLog ? 0 : LOGentryIndex(TopLog);
}

function LOGbottomViewPosition(){
   let BottomLog = document.querySelector('.LOGbottomVisible');
   LOGdownIndex = !BottomLog ? Infinity :LOGentryIndex(BottomLog);
}

function LOGupdateRenderMoreDown(){
  // console.log(LOGdownIndex);
   if(LOGshowListData.length <= 1550) LOGrenderDown = LOGdownIndex >= 750;
   else                               LOGrenderDown = LOGdownIndex >= 1550;
}

function LOGupdateRenderMoreUp(){
   LOGrenderUp = LOGupIndex <= 250;
}

function LOGrenderingMoreDown(){
   // or maybe we have to switch it to use the filtering model
   if(!LOGrenderDown || LOGendOfLogs || LOGfilteringLogs() || LOGinProgress) return;
   LOGrenderDown = false;
   LOGrenderMoreDown();
}

function LOGrenderMoreDown(){
   let Args = LOGprevArgs();
   LOGinProgress = true;
   APIcall("logs/retrieve", Args, function(R){
      let Data = R.data || [];
      LOGshowListAppend(Data);
      LOGendOfLogs = Data.length == 0;
      LOGstartOfLogs = false;
      LOGinProgress = false;
   });
}

function LOGshowMoreResults(){
   let Div = document.querySelector(".LOGshowNewLogs");
   return Div;
}

function LOGrenderingMoreUp(){
   if(!LOGrenderUp || LOGstartOfLogs || LOGfilteringLogs() || LOGinProgress || LOGshowMoreResults()) return;
   LOGrenderUp = false;
   LOGrenderMoreUp();
}

function LOGrenderMoreUp(Callback){
   LOGinProgress = true;
   let Args = LOGnextArgs();
   APIcall("logs/retrieve", Args, function(R){
      let Data = R.data || [];
      LOGshowListPrepend(Data);
      let Number = Data.length;
      LOGstartOfLogs = Number == 0;
      LOGendOfLogs = false;
      LOGinProgress = false;
      if(Callback) Callback(Data);
   });
}