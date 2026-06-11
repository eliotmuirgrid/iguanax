function LOGindexDownClicked(){
   if(!PAGEhashParameter("filter")) return LOGsearchIndexDownClicked();
   else                             return LOGfilterIndexDownClicked();
}

function LOGsearchIndexDownClicked(){
   if(!LOGsearchResultsData.length){
      LOGsearchIndex = -1;
      LOGupdateSearchSelect();
      LOGsearchResultCount();
      return;
   }
   if(LOGsearchIndex == LOGsearchResultsData.length - 1){
      LOGsearchIndex = 0;
      LOGupdateSearchSelect();
      LOGsearchResultCount();
      return;
   }
   let Selected     = LOGcurrentlySelected();
   if(!Selected) return;
   let ClosestIndex = LOGclosestSearchIndexToSelected().index;
   let Closest = LOGsearchResultsData[ClosestIndex];
   if(Selected.time == Closest.time && Selected.id == Closest.id) LOGsearchIndex = ClosestIndex + 1;
   if(Selected.time == Closest.time && Selected.id >  Closest.id) LOGsearchIndex = ClosestIndex;
   if(Selected.time == Closest.time && Selected.id <  Closest.id) LOGsearchIndex = ClosestIndex + 1;
   if(Selected.time > Closest.time) LOGsearchIndex = ClosestIndex - 1;
   if(Selected.time < Closest.time) LOGsearchIndex = ClosestIndex + 1;
   if(LOGsearchIndex < 0) LOGsearchIndex = 0;
   if(LOGsearchIndex >= LOGsearchResultsData.length) LOGsearchIndex = LOGsearchResultsData.length - 1;
   let NewSelect = LOGsearchResultsData[LOGsearchIndex];
   if(NewSelect.time > Selected.time) LOGsearchIndex = LOGsearchIndex + 1;
   if(LOGsearchIndex >= LOGsearchResultsData.length) LOGsearchIndex = LOGsearchResultsData.length - 1;
   LOGupdateSearchSelect();
   LOGsearchResultCount();
   let div = document.querySelector(".LOGsearchIndexInput");
   FORMfocus(div);
}

function LOGcurrentlySelectedIndex(){
   let LogEntry = document.querySelector(".LOGentrySelected");
   if(!LogEntry) LogEntry = document.querySelector(".LOGtopVisible");
   if(!LogEntry) return;
   const parentElement = LogEntry.parentNode;
   const logEntries = Array.from(parentElement.querySelectorAll('.LOGlogEntry'));
   const Index = logEntries.indexOf(LogEntry);
   return Index;
}

function LOGcurrentlySelected(){
   let Index = LOGcurrentlySelectedIndex();
   if(Index === undefined) return;
   let ListData = LOGshowListData;
   if(Index >= ListData.length) { return; }
   const Item = ListData[Index];
   return Item;
}

function LOGindexUpClicked(){
   if(!PAGEhashParameter("filter")) return LOGsearchIndexUpClicked();
   else                             return LOGfilterIndexUpClicked();
}

function LOGsearchIndexUpClicked(){
   if(LOGsearchIndex == 0){
      LOGsearchIndex = LOGsearchResultsData.length - 1;
      LOGupdateSearchSelect();
      LOGsearchResultCount();
      return;
   }
   let Selected = LOGcurrentlySelected();
   if(!Selected) return;
   let ClosestIndex = LOGclosestSearchIndexToSelected().index;
   let Closest = LOGsearchResultsData[ClosestIndex];
   if(!Closest) return;
   if(Selected.time == Closest.time && Selected.id == Closest.id) LOGsearchIndex = ClosestIndex - 1;
   if(Selected.time == Closest.time && Selected.id <  Closest.id) LOGsearchIndex = ClosestIndex;
   if(Selected.time == Closest.time && Selected.id >  Closest.id) LOGsearchIndex = ClosestIndex - 1;
   if(Selected.time > Closest.time) LOGsearchIndex = ClosestIndex - 1;
   if(Selected.time < Closest.time) LOGsearchIndex = ClosestIndex + 1;
   if(LOGsearchIndex >= LOGsearchResultsData.length) LOGsearchIndex = LOGsearchResultsData.length - 1;
   if(LOGsearchIndex < 0) LOGsearchIndex = 0;
   let NewSelect = LOGsearchResultsData[LOGsearchIndex];
   // if(NewSelect.time < Selected.time)                                LOGsearchIndex = LOGsearchIndex - 1;
   // if(NewSelect.time == Selected.time && NewSelect.id < Selected.id) LOGsearchIndex = LOGsearchIndex - 1;
   LOGupdateSearchSelect();
   LOGsearchResultCount();
}

function LOGbinarySearch(LogTime, LogIndex) {
   let left = 0;
   let right = LOGshowListData.length - 1;
   let count = 0;

   while (left <= right && count < 1000) {
      count = count + 1;
      const mid = Math.floor((left + right) / 2);
      const element = LOGshowListData[mid];
      if (element.time === LogTime && element.id === LogIndex) {
         return mid; // Element found
      } else if (
         parseInt(element.time) > parseInt(LogTime) ||
         (element.time === LogTime && parseInt(element.id) > parseInt(LogIndex))
      ) {
         left = mid + 1; // Search the right half
      } else {
         right = mid - 1; // Search the left half
      }
   }
 
   return -1; // Element not found  
}

function LOGfindClosestIndex(LogTime, LogIndex, Data) {
   let left = 0;
   let right = Data.length - 1;
   let closestIndex = -1; // Initialize to -1
   let count = 0;

   while (left <= right && count < 1000) {
      count = count + 1;
      const mid = Math.floor((left + right) / 2);
      const element = Data[mid];

      if (element.time === LogTime && element.id === LogIndex) {
         return { index : mid, exact : true}; // Element found
      } else if (
         parseInt(element.time) > parseInt(LogTime) ||
         (element.time === LogTime && parseInt(element.id) > parseInt(LogIndex))
      ) {
         left = mid + 1; // Search the right half
      } else {
         right = mid - 1; // Search the left half
      }

      // Update closestIndex when a closer element is found
      if (closestIndex === -1 || Math.abs(LogTime - element.time) < Math.abs(LogTime - Data[closestIndex].time)) {
         closestIndex = mid;
      }
   }

   // If no exact match is found, return the index of the closest element
   return { index : closestIndex, exact : false};
}
