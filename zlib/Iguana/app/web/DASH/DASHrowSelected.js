function DASHgetSelectedComponents(){
   let SelectedRows = document.querySelectorAll('.DASHrowSelected');
   let SelectedComponents = [];
   for (let i = 0; i < SelectedRows.length; i++) {
      let guid = DASHrowGuid( DASHrowIndex(SelectedRows[i]) );
      SelectedComponents.push(guid);
   }
   return SelectedComponents;
}

function DASHclearSelectedRows(){ DASHsetRowsSelected(DASHrenderedRows, false); }

function DASHsetRowsSelected(ComponentGuids, Flag){
   for( let i = 0; i < ComponentGuids.length; i++){
      let row = DASHrowSelector(ComponentGuids[i]);
      if(!row) continue;
      if(Flag) row.classList.add('DASHrowSelected');
      else     row.classList.remove('DASHrowSelected');
   }
}

function DASHrowSelector(guid){
   let Idx = DASHrenderedRows.indexOf(guid);
   return document.querySelectorAll('.DASHcomponentRow')[Idx-1];
}

function DASHcheckboxSelector(guid){
   let Idx = DASHrenderedRows.indexOf(guid);
   return document.querySelectorAll('.DASHselectRow')[Idx-1];
}

function DASHjumpToRow(guid){
   let Row = DASHrowSelector(guid);
   let container = document.querySelector(".DASHtableContainer");
   if(Row) LOGscrollIfNeeded(Row, container);
}
function LOGscrollIfNeeded(el, container, { fadeTop = 52, fadeBottom = 40, behavior = "smooth" } = {}) {
   if (!el || !container) return;
 
   const cRect = container.getBoundingClientRect();
   const eRect = el.getBoundingClientRect();
 
   // Effective visible region (exclude the faded edges)
   const topEdge = cRect.top + fadeTop;
   const bottomEdge = cRect.bottom - fadeBottom;
 
   // If the element intrudes into the fade (or beyond), adjust minimally
   if (eRect.top < topEdge) {
     container.scrollTo({ top: container.scrollTop + (eRect.top - topEdge), behavior });
   } else if (eRect.bottom > bottomEdge) {
     container.scrollTo({ top: container.scrollTop + (eRect.bottom - bottomEdge), behavior });
   }
 }

function DASHjumpToHighestRow(ComponentGuids){
   let LowestIndex, LowestGuid;
   for (let i =0; i < ComponentGuids.length; i++){
      let ComponentGuid = ComponentGuids[i];
      let ComponentIndex = DASHrenderedRows.indexOf(ComponentGuid);
      if(ComponentIndex <= 0) { continue; }
      if(!LowestIndex || ComponentIndex < LowestIndex) {
         LowestIndex = ComponentIndex;
         LowestGuid  = ComponentGuid;
      }
   }
   if(LowestGuid) DASHjumpToRow(LowestGuid);
}
