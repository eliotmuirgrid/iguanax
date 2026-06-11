let LOGstartDateTimeInFocus;

function LOGsetListeners(){
   let Container = document.querySelector('.LOGcontainer');
   if(!Container) return;
   Container.addEventListener('mousedown', LOGmouseDownHandler);
   Container.addEventListener('mouseup',   LOGmouseUpHandler);
   Container.addEventListener('click',     LOGclickHandler);
   Container.addEventListener('dblclick',  LOGdblclickHandler);
   Container.addEventListener('change',    LOGchangeHandlerWrapper);
   Container.addEventListener('input',     LOGinputHandler);
   Container.addEventListener('keydown',   LOGkeydownHandler);
   let Scrollbar = document.querySelector(".LOGscrollbar");
   Scrollbar.addEventListener('mousemove',  LOGscrollbarMouseMove);
   Scrollbar.addEventListener('mouseleave', LOGscrollbarMouseLeave);

   let Start = document.querySelector(".LOGstartTime");
   if(Start) {
      Start.addEventListener('focus', LOGstartTimeBarFocus);
      Start.addEventListener('blur',  LOGstartTimeBarBlur);
   }
   let End = document.querySelector(".LOGendTime");
   if(End) {
      End.addEventListener('focus', LOGendTimeBarFocus);
      End.addEventListener('blur',  LOGendTimeBarBlur);
   }
   let BulkStart = document.querySelector(".LOGbulkStart");
   BulkStart && BulkStart.addEventListener('focus', LOGbulkStartSelect);
   
   let BulkEnd = document.querySelector(".LOGbulkEnd");
   BulkEnd && BulkEnd.addEventListener('focus', LOGbulkEndSelect);

   let Body = document.querySelector('.LOGbody');
   Body && Body.addEventListener("scroll", LOGselectTopAndBottom);
   LOGsetScrollSpeed();
}

function LOGmouseDownHandler(e){
   if(e.target.closest('.LOGloadNewer')) return LOGstartScrollingNewer();
   if(e.target.closest('.LOGloadOlder')) return LOGstartScrollingOlder();
}

function LOGmouseUpHandler(){
   clearInterval(LOGscrollNewerInterval);
   clearInterval(LOGscrollOlderInterval);
}

let LOGscrollNewerInterval;
let LOGscrollOlderInterval;
let LOGscrollPageNewerInterval;
let LOGscrollPageOlderInterval;

function LOGstartScrollingNewer(){
   LOGloadNewerLogsClicked();
   LOGscrollNewerInterval = setInterval(LOGloadNewerLogsClicked, 200);
}

function LOGstartScrollingOlder(){
   LOGloadOlderLogsClicked();
   LOGscrollOlderInterval = setInterval(LOGloadOlderLogsClicked, 200);
}

function LOGstartPageScrollingNewer(){
   LOGloadNewerLogsClicked(10);
   LOGscrollPageNewerInterval = setInterval(function(){
      LOGloadNewerLogsClicked(10);
   }, 200);
}

function LOGstartPageScrollingOlder(){
   LOGloadOlderLogsClicked(10);
   LOGscrollPageOlderInterval = setInterval(function(){
      LOGloadOlderLogsClicked(10);
   }, 200);
}

function LOGclickHandler(e){
   if(e.target.closest('.LOGtag'))                 return LOGtagClicked(e.target);
   if(e.target.closest('.LOGlight'))               return LOGswitchClicked();
   if(e.target.closest('.LOGshowNewLogs'))         return LOGloadNewestLogsClicked();
   if(e.target.closest('.LOGsearchButton'))        return LOGsearchInput();
   if(e.target.closest('.LOGloadNewest'))          return LOGloadNewestLogsClicked();
   if(e.target.closest('.LOGloadOldest'))          return LOGloadOldestLogsClicked();
   if(e.target.closest('.LOGselectButton'))        LOGselectButton(e.target);
   if(e.target.closest('.LOGlogEntry'))            return LOGentryClicked(e.target);
   if(e.target.closest('.LOGclearSearch'))         return LOGclearInput("LOGsearchInput");
   if(e.target.closest('.LOGclearTags'))           return LOGclearInput("LOGtagsInput");
   if(e.target.closest('.LOGclearStart'))          return LOGclearInput("LOGstartTime");
   if(e.target.closest('.LOGclearEnd'))            return LOGclearInput("LOGendTime");
   if(e.target.closest('.LOGfilterLoadMore'))      return LOGfilterLoadMoreClicked();
   // if(e.target.closest('.LOGsearchLoadMore'))      return LOGsearchLoadMoreClicked();
   if(e.target.closest('.LOGclearSearchFilters'))  return LOGclearSearchFiltersClicked();
   if(e.target.closest('.LOGscrollbarLineTop'))    return LOGtopDateLineClicked();
   if(e.target.closest('.LOGscrollbarLineBottom')) return LOGbottomDateLineClicked();
   if(e.target.closest('.LOGscrollbarLine'))       return LOGscrollDateLineClicked();
   if(e.target.closest('.LOGscrollbarBox'))        return LOGscrollToPositionClicked(e);
   // if(e.target.closest('.LOGclearDateFilters'))    return LOGclearDatesClicked();
   if(e.target.closest('.LOGsearchIndexUp'))       return LOGindexUpClicked();
   if(e.target.closest('.LOGsearchIndexDown'))     return LOGindexDownClicked();
   if(e.target.closest('.LOGbeginSearchHere'))     return LOGbeginSearchHereClicked();
   if(e.target.closest('.LOGclearLogs'))           return LOGclearLogsClicked();
   if(e.target.closest('.LOGfilterCheckbox'))      return LOGcheckboxClick(e.target);
   if(e.target.closest('.LOGclearBulk'))           return LOGresetBulk();
   if(e.target.closest('.LOGcancelBulk'))          return LOGcancelBulkClicked();
   if(e.target.closest('.LOGbulkConfirm'))         return BLKconfirm();
   if(e.target.closest('.LOGfixDiff'))             return LOGfillInGaps(e.target);
}

function LOGcheckboxClick(target){
   if(target.closest("input")) return;
   let inp = target.querySelector("input");
   if(!inp) return;
   if(inp.checked) inp.checked = false;
   else            inp.checked = true;
   LOGinputControl();
   // LOGchangeHandler(inp);
}

function LOGclearSearchFiltersClicked(){
   LOGresetBulk();
   LOGclearInput("LOGsearchInput");
   LOGclearInput("LOGtagsInput");
   let Args = {};
   Args.component = PAGEhashParameter("component");
   window.location.hash = `logs${PAGEformatParams(Args)}`;
}

function LOGentryClicked(target){
   if(!target) return;
   let Log = target.closest(".LOGlogEntry");
   LOGfocusOnLog(Log);
   // LOGhashSearch(target);
   if(!PAGEhashParameter("search") && !PAGEhashParameter("tags")) return;
   if( PAGEhashParameter("filter")) {
      let SelectedIdx = LOGcurrentlySelectedIndex();
      LOGsearchIndex = SelectedIdx;
      LOGfilterResultCount();
   }
   else{
      let Result = LOGclosestSearchIndexToSelected();
      if(!Result || !Result.exact) LOGsearchIndex = -1;
      else                         LOGsearchIndex = Result.index;
      LOGsearchResultCount();
   }
   if(LOGsearchIndex == -1) return;
   LOGupdateSearchSelect(true);
}

function LOGdblclickHandler(e){
   if(e.target.closest('.LOGlogEntry')) return LOGlogEntrySelected(e);
}

function LOGchangeHandlerWrapper(e){
   LOGchangeHandler(e.target);
}

function LOGchangeHandler(target){
   // if(target.closest('.LOGfilterCriteriaDiv'))     LOGinputControl();
   if(target.closest('.LOGcomponentSelect'))       return LOGcomponentSelected();
   // if(target.closest('.LOGexactCheckbox')) return LOGwholewordResultsSelected();
   // if(target.closest('.LOGfilterAllCheckbox')) return LOGfilterResultsSelected();
   // if(target.closest('.LOGshowOnlyMessages'))      return LOGshowOnlyMessagesSelected();
   // if(target.closest('.LOGshowOnlyErrors'))        return LOGshowOnlyErrorsSelected();
   // if(target.closest(".LOGstartTime"))             return LOGstartTimeChanged();
   // if(target.closest(".LOGendTime"))               return LOGendTimeChanged();
   if(target.closest(".LOGsearchIndexInput"))      return LOGsearchIndexInput(target);
}

function LOGinputHandler(e){
   if(e.target.closest('.LOGfilterCriteriaDiv')) LOGinputControl();
   // if(e.target.closest('.LOGsearchInput')) return LOGsearchInput();
   // if(e.target.closest('.LOGtagsInput'))   return LOGsearchInput();
}

function LOGkeydownHandler(e){
   // if(e.target.closest('.LOGfilterCriteriaDiv')) LOGinputControl();
   if(e.target.closest('.LOGsearchIndexInput')) return LOGsearchIndexInput(e, e.target);
   if(e.target.closest('.LOGsearchInput2') && e.key === "Enter") return LOGsearchInput();
}

let LOGlastInputVal;

function LOGsearchIndexInput(e, input) {
   // global state (last committed value, and whether user edited since then)
   globalThis.LOGlastInputVal = globalThis.LOGlastInputVal ?? null; // 1-based
   globalThis.LOGinputDirty   = globalThis.LOGinputDirty   ?? false;
 
   const raw = input?.value ?? "";
   const num = parseInt(raw, 10);
 
   // For non-Enter keys, mark as dirty if they likely changed the input
   if (e.key !== "Enter") {
     const isDigit     = e.key?.length === 1 && e.key >= "0" && e.key <= "9";
     const editKeys    = e.key === "Backspace" || e.key === "Delete";
     if (isDigit || editKeys) globalThis.LOGinputDirty = true;
     return;
   }
 
   // --- Enter pressed ---
   if (isNaN(num) || num < 1) return;
 
   const usingFilter = !!PAGEhashParameter("filter");
   const maxIndex    = usingFilter ? LOGshowListData.length : LOGsearchResultsData.length;
 
   let value        = Math.min(num, maxIndex); // 1-based, clamped
   const typedIndex = value - 1;               // 0-based
   const currentIdx = LOGsearchIndex;          // 0-based
 
   // Iterate only if:
   // - user did NOT edit since last commit (not dirty)
   // - typed value equals last committed value
   // - typed value equals current index (i.e., they're on that item)
   const shouldIterate =
     !globalThis.LOGinputDirty &&
     globalThis.LOGlastInputVal === value &&
     currentIdx === typedIndex;
 
   if (shouldIterate) {
     value += e.shiftKey ? -1 : 1;
     if (value < 1) value = 1;
     if (value > maxIndex) value = maxIndex;
   }
 
   LOGsearchIndex = value - 1;
 
   if (usingFilter) {
     LOGupdateFilterSelect();
     LOGfilterResultCount();
   } else {
     LOGupdateSearchSelect();
     LOGsearchResultCount();
   }
 
   // Commit state for the next round
   globalThis.LOGlastInputVal = value;     // 1-based
   globalThis.LOGinputDirty   = false;
 
   // Keep the box in sync with clamps/iteration
   if (input && typeof input.value !== "undefined") input.value = String(value);
 }
 

function LOGcomponentSelected(){
   let C = document.querySelector(".LOGcomponentSelect");
   STORElocalCreate("PRJlastSelectedComponent", C.value);
   LOGscrollToTop();
   window.location.hash = LOGformatUrl(C.value);
}

function LOGtargetToItemData(target){
   if(!target) return;
   let ListData = LOGshowListData;
   const LogEntry = target.closest('.LOGlogEntry');
   if(!LogEntry) { return; }
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.LOGlogEntry'));
   const Index = logEntries.indexOf(LogEntry);
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}

function LOGcalculateTime(scrollPosition) {
   let Scrollbar = document.querySelector(".LOGscrollbarBox");
   let Height = Scrollbar.offsetHeight;
   const startTime = LOGscrollbarTopIndex;
   const endTime = LOGscrollbarBottomIndex;
   const totalHeight = Height;
   const time = startTime + ((scrollPosition / totalHeight) * (endTime - startTime));
   return time;
}

function LOGscrollbarMouseMove(e){
   let scrollContainer = document.querySelector(".LOGscrollbarBox");
   let scrollContainerPopout = document.querySelector(".LOGscrollbarPopout");
   let scrollContainerTooltip = document.querySelector(".POPtooltip");
   let scrollLine =  document.querySelector(".LOGscrollbarHoverLine");
   const mouseY = e.clientY - scrollContainer.getBoundingClientRect().top;
   if (mouseY >= 0 && mouseY <= scrollContainer.clientHeight) {
      LOGupdateTooltipPosition(mouseY)
      const time = LOGcalculateTime(mouseY);
      if(!scrollContainerTooltip) return;
      if(isNaN(time)) return scrollContainerTooltip.classList.add("LOGhidden");
      if(scrollContainerTooltip) scrollContainerTooltip.classList.remove("LOGhidden");
      let html = LOGyearMonthDateTime({
         epoch: time,
         includeTime: true,
         simpleTime: true,
         shortDayLabel: true
      });
      if(scrollContainerPopout) scrollContainerPopout.innerHTML = html;
      scrollLine.classList.remove("LOGhidden");
      LOGupdateLinePosition(scrollLine, time, 0, false);
   }
}

function LOGupdateTooltipPosition(mouseY){
   let Div = document.querySelector(".LOGscrollbarPopout");
   if(!Div) return;
   let Popout = Div.closest(".POPtooltip");
   if(!Popout) return;
   Popout.style.top = mouseY + 45 + "px";
}

function LOGscrollbarMouseLeave(){
   const hoverLine = document.querySelector('.LOGscrollbarHoverLine');
   hoverLine.classList.add("LOGhidden");
}

function LOGscrollToPositionClicked(e){
   let scrollContainer = document.querySelector(".LOGscrollbarBox");
   const mouseY = e.clientY - scrollContainer.getBoundingClientRect().top;
   let time;
   if (mouseY >= 0 && mouseY <= scrollContainer.clientHeight) {
      time = Math.round(LOGcalculateTime(mouseY)) + "";

   } else{
      return;
   }

   if     (LOGstartDateTimeInFocus)     LOGupdateStartTimeUrl(time);
   else if(LOGendDateTimeInFocus)       LOGupdateEndTimeUrl(time);
   else if(PAGEhashParameter("filter")) LOGfilterScrollbarClicked(time);
   else                                 LOGupdateTimeUrl(time);
}

function LOGstartTimeBarFocus(){
   let Div = document.querySelector(".LOGlineSelected");
   if(Div) Div.classList.remove("LOGlineSelected");
   Div = document.querySelector(".LOGscrollbarLineTop");
   if(Div) Div.classList.add("LOGlineSelected");
   LOGstartDateTimeInFocus = true;
}

function LOGendTimeBarFocus()  {
   let Div = document.querySelector(".LOGlineSelected");
   if(Div) Div.classList.remove("LOGlineSelected");
   Div = document.querySelector(".LOGscrollbarLineBottom");
   if(Div) Div.classList.add("LOGlineSelected");
   LOGendDateTimeInFocus = true;
}

function LOGstartTimeBarBlur(){
   let Div = document.querySelector(".LOGlineSelected");
   if(Div) Div.classList.remove("LOGlineSelected");
   setTimeout(function(){ LOGstartDateTimeInFocus = false; }, 200);
}
function LOGendTimeBarBlur()  {
   let Div = document.querySelector(".LOGlineSelected");
   if(Div) Div.classList.remove("LOGlineSelected");
   setTimeout(function(){ LOGendDateTimeInFocus   = false; }, 200);
}

function LOGtagClicked(target){
   let hashtag = target.getAttribute('word');
   if(hashtag == null) return;
   PAGEsetHashParameter("tags", hashtag);
   let tagInp = document.querySelector('.LOGtagsInput');
   if(tagInp) tagInp.value = hashtag;
}
