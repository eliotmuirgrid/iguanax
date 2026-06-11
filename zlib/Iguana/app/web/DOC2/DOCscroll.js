let DOCrenderDown = false;
let DOCrenderUp   = false;
let DOCdownIndex;
let DOCupIndex;
let DOCscrollTimerDown;
let DOCscrollTimerDate;
let DOCflagResetTimer;
let DOCscrollTimerUp;
let DOCstartOfDocs = false;
let DOCendOfDocs = false;
let DOClimitScrollSpeed = true;
let DOCinProgress = false;

function DOCscrollListeners(){
   let Body = document.querySelector('.DOC2list');
   Body && Body.addEventListener("scroll", DOCselectTopAndBottom);
   DOCsetScrollSpeed();
}
function DOCselectTopAndBottom(){
   clearTimeout(DOCscrollTimerDown);
   DOCscrollTimerDown = setTimeout(function(){
      DOCselectBottomVisibleElement();
      DOCbottomViewPosition();
      DOCupdateRenderMoreDown();
      DOCrenderingMoreDown();
   },15);

   clearTimeout(DOCscrollTimerUp);
   DOCscrollTimerUp = setTimeout(function(){
      DOCselectTopVisibleElement();
      DOCtopViewPosition();
      DOCupdateRenderMoreUp();
      DOCrenderingMoreUp();
   },17);

   clearTimeout(DOCflagResetTimer);
   DOCflagResetTimer = setTimeout(function(){ 
      DOCresetTopAndBottomFlags();
   }, 60);
}

function DOCsetScrollSpeed(){
   // Get a reference to the scrollable div
   const scrollableDiv = document.querySelector('.DOC2list');
   // Get a reference to the scrollable div
   // Set a maximum scroll speed (adjust as needed)
   const maxScrollSpeed = 5; // Change this value to control the speed

   let lastScrollTime = 0;
   let lastScrollTop = 0;

   scrollableDiv.addEventListener('scroll', function(event) {
      const now = Date.now();
      const deltaTime = now - lastScrollTime;
      const scrollTop = scrollableDiv.scrollTop;
      DOClimitScrollSpeed = false;
      if (DOClimitScrollSpeed && deltaTime > 0) {
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

function DOCresetTopAndBottomFlags(){
   DOCstartOfDocs = false;
   DOCendOfDocs   = false;
}

function DOClistOffset(){
   return document.querySelector(".DOC2list").getBoundingClientRect().top - 40;
}

function DOCselectTopVisibleElement(){
   const ParentDiv      = document.querySelector(".DOC2list");
   const ScrollPosition = ParentDiv.scrollTop + DOClistOffset();
   const DocElements    = ParentDiv.querySelectorAll(".DOC2listRow");

   let TopElement = null;
   let SmallestOffset = Infinity;
   DocElements.forEach((DocElement) =>{
      const Offset = DocElement.offsetTop;
      if (Offset >= ScrollPosition && Offset < SmallestOffset){
         SmallestOffset = Offset;
         TopElement = DocElement;
      }});
   if (TopElement){
      DocElements.forEach((DocElement) => {
         DocElement.classList.remove("DOCtopVisible");
      });
      TopElement.classList.add("DOCtopVisible");
   }
}

function DOCselectBottomVisibleElement(){
   const ParentDiv      = document.querySelector(".DOC2list");
   const ScrollPosition = ParentDiv.scrollTop + ParentDiv.clientHeight + DOClistOffset();
   const DocElements    = ParentDiv.querySelectorAll(".DOC2listRow");

   let BottomElement = null;
   let LargestOffset = -Infinity;

   DocElements.forEach((DocElement) =>{
      const Offset = DocElement.offsetTop;
      const EntryHeight = DocElement.offsetHeight;
      const BottomPosition = Offset + EntryHeight;

      if (BottomPosition <= ScrollPosition && BottomPosition > LargestOffset){
         LargestOffset = BottomPosition;
         BottomElement = DocElement;
      }
   });

   if (BottomElement){
      DocElements.forEach((DocElement) => {
         DocElement.classList.remove("DOCbottomVisible");
      });
      BottomElement.classList.add("DOCbottomVisible");
   }
}

function DOCtopViewPosition(){
   let TopDoc = document.querySelector(".DOCtopVisible");
   DOCupIndex = !TopDoc ? 0 : DOCentryIndex(TopDoc);
   return DOCupIndex;
}

function DOCbottomViewPosition(){
   let BottomDoc = document.querySelector(".DOCbottomVisible");
   DOCdownIndex = !BottomDoc ? Infinity : DOCentryIndex(BottomDoc);
}

function DOCentryIndex(target){
   const DocElement = target.closest('.DOC2listRow');
   if(!DocElement) { return; }
   const ParentElement = DocElement.parentNode;
   const DocElements = Array.from(ParentElement.querySelectorAll('.DOC2listRow'));
   const Index = DocElements.indexOf(DocElement);
   return Index;
}

//Swap to hardcoded values when we determine best MAX_DOC_ENTRIES
function DOCupdateRenderMoreUp() {
   const TopThreshold = Math.max(10, Math.floor(DOCshowListData.length * 0.2));
   DOCrenderUp = DOCupIndex <= TopThreshold;
 }
 
 function DOCupdateRenderMoreDown() {
   const BottomThreshold = Math.max(10, Math.floor(DOCshowListData.length * 0.75));
   DOCrenderDown = DOCdownIndex >= BottomThreshold;
 }

function DOCrenderingMoreUp(){
   if(!DOCrenderUp || DOCstartOfDocs || DOCinProgress) return;
   DOCrenderUp = false;
   DOCrenderMoreUp();
}

function DOCrenderingMoreDown(){
   if(!DOCrenderDown || DOCendOfDocs || DOCinProgress) return;
   DOCrenderDown = false;
   DOCrenderMoreDown();
}

function DOCrenderMoreUp(){
   DOCinProgress = true;
   let Item = DOCnewestListItem();
   let Args = DOCrenderMoreUpArgs(Item);
   APIcall("doc/search/threaded",Args, function(R){
      R.data.reverse();
      let Data = R.data || [];
      if (Data.length > 0 && Data[Data.length-1].id === MODELdata.DOCscrollbarNewest?.data?.[0]?.id){
         DOCinProgress = false;
         DOCendOfDocs=false;
         return;
      } 
      DOCshowListPrepend(Data);
      let Number = Data.length;
      DOCstartOfDocs = (Number === 0) || (
         MODELdata.DOCscrollbarNewest?.data?.[0]?.id === DOCnewestListItem().id
       );
      DOCendOfDocs = false;
      DOCinProgress = false;
   });
}

function DOCrenderMoreDown(){
   DOCinProgress = true;
   let Item = DOColdestListItem();
   let Args = DOCrenderMoreDownArgs(Item);
   APIcall("doc/search/threaded",Args, function(R){
      R.data.shift();
      let Data = R.data || [];
         if (Data.length > 0 && Data[Data.length-1].id === MODELdata.DOCscrollbarOldest?.data?.[0]?.id && DOCendOfDocs){
            DOCinProgress = false;
            DOCstartOfDocs= false;
            return;
         } 
      DOCshowListAppend(Data);
      DOCendOfDocs = (Data.length == 0) || (
         MODELdata.DOCscrollbarOldest?.data?.[0]?.id === DOColdestListItem().id
       );
      DOCstartOfDocs = false;
      DOCinProgress  = false;
   });
}