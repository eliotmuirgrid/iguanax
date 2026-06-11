let DASHrenderedAllPermData = {};
let DASHrenderedComponentData = {};
let DASHrenderedFilter;
let DASHrenderedRows = ["DASHtableHeader"];
let DASHlistChanged;

function DASHfocusedComponentIdFromCard() {
   const componentId = PAGEhashParameter('component');
   if(!componentId) { return ''; }
   return DASHcontainsComponent(DASHlistData(), componentId) ? componentId : '';
}

function DASHrenderList(){
   COL_TRC("Rendering list.");
   DASHlistChanged = false;
   let SelectedComponents = DASHgetSelectedComponents();
   const focusedComponent = DASHfocusedComponentIdFromCard();
   if(focusedComponent) {
      DASHclearSelectedRows();
      SelectedComponents = [];
   }
   DASHremoveStaleRows();
   DASHhandleRowContent();
   DASHreorganizePositions();
   DASHresultsCount();
   DASHhandleSelectToolbar();
   DASHhandleSelectAll();
   DASHscrollToRender();
   DASHrenderedFilter = DASHsearchText();
   if(DASHlistChanged) POPtooltipRemoveByClass('LGEtooltipWrap');
   DASHsetRowsSelected(SelectedComponents, true);
   if(focusedComponent) { DASHjumpToRow(focusedComponent); }
   document.querySelector('.DASHtableContainer').dispatchEvent(new Event('mouseover'));
}
function DASHscrollToRender() {
   if(DASHscrollToComponent.length) DASHjumpToHighestRow(DASHscrollToComponent);
   DASHscrollToComponent = [];
}

function DASHcontainsComponent(DashboardData, ComponentId){
   if(!DashboardData) { return false; }
   for (let i = 0; i < DashboardData.length; i++){ if(DashboardData[i].guid === ComponentId) return true; }
   return false;
}

function DASHremoveStaleRows(){
   for(let i = DASHrenderedRows.length - 1; i > 0 ; i--) {
      let ComponentId = DASHrenderedRows[i];
      if(!DASHcontainsComponent(DASHlistData(), ComponentId)){
         DASHremoveRow(ComponentId);
         DASHlistChanged = true;
         delete DASHrenderedComponentData[ComponentId];
      }
   }
}

function DASHserverNameChanged(ComponentData){
   if(!DASHrenderedComponentData[ComponentData.guid]) {
      COL_TRC("Could not find rendered component data for component");
      return true;
   }
   return false;
}

function DASHhandleRowContent(){
   let ListData = DASHlistData();
   let AllPermData = DASHuserPermissionAll();
   let RunningComponents = DASHrunningComponentsData();
   for(let i = 0; i < ListData.length; i++) {
      let ComponentData = ListData[i];
      let ComponentId = ComponentData.guid;
      let PermData = AllPermData[ComponentId];
      let Different = !DEEPequal(DASHrenderedComponentData[ComponentId], ComponentData);
      COL_TRC("Different ", Different);
      Different = Different || DASHserverNameChanged(ComponentData);
      COL_TRC("Different ", Different);
      Different = Different || DASHsearchedChanged();
      COL_TRC("Different ", Different);
      Different = Different || !DEEPequal(DASHrenderedAllPermData[ComponentId], PermData);
      COL_TRC("Different ", Different);
      if (Different || DASHlastOnStatus[ComponentId] !== RunningComponents[ComponentId]) { // check to see if the running status is different
         let ComponentRowHTML = DASHcomponentRowHtml(ComponentData, RunningComponents[ComponentId]);
         if(DASHrowAlreadyRendered(ComponentId)) { DASHupdateRowContent(ComponentId, ComponentRowHTML); }
         else                                    { DASHaddComponentRow (ComponentId, ComponentRowHTML); }
         DASHlistChanged = true;
         DASHrenderedComponentData[ComponentId] = ComponentData;
         DASHrenderedAllPermData[ComponentId]   = PermData;
      }
   }
   DASHlastOnStatus = RunningComponents;  // Store the last on status.
}

function DASHsearchedChanged(){
   if(DASHrenderedFilter !== DASHsearchText()) return true;
   return false;
}

function DASHupdateRowContent(ComponentId, HTMLContent){
   let Row = DASHcomponentRowSelector(ComponentId);
   if (Row) { Row.outerHTML = HTMLContent; }
}

function DASHcomponentRowSelector(ComponentId){ return document.querySelectorAll('.DASHtableRow')[DASHrenderedRows.indexOf(ComponentId)]; }

function DASHrowAlreadyRendered(ComponentId){ return DASHrenderedRows.indexOf(ComponentId) !== -1;  }

function DASHaddComponentRow(ComponentId, HTMLContent) {
   const rows = document.querySelectorAll('.DASHtableRow');
   if (rows.length > 0) {
      rows[rows.length - 1].insertAdjacentHTML('afterend', HTMLContent); // Insert below the last row
   } else {
      const table = document.querySelector('.DASHtableRow').parentNode; // Get the parent container
      table.insertAdjacentHTML('beforeend', HTMLContent); // Insert at the end if no rows exist
   }
   DASHrenderedRows.push(ComponentId);
}

function DASHreorganizePositions(){
   let ListData = DASHlistData();
   for(let i = 0; i < ListData.length; i++) {
      let ComponentGuid = ListData[i].guid;
      let AnchorGuid    = DASHrenderedRows[i];
      DASHinsertComponentAfterAnchor(ComponentGuid, AnchorGuid);
   }
}

function DASHinsertAfter(newNode, referenceNode){ referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling); }

function DASHinsertIndexAfterAnchor(CurrentIdx, AnchorIdx){
   let RenderedRow = document.querySelectorAll('.DASHtableRow')[CurrentIdx];
   let AnchorRow   = document.querySelectorAll('.DASHtableRow')[AnchorIdx ];
   DASHinsertAfter(RenderedRow, AnchorRow);
}

function DASHinsertComponentAfterAnchor(ComponentGuid, AnchorGuid){
   let ComponentIdx = DASHrenderedRows.indexOf(ComponentGuid);
   let AnchorIdx    = DASHrenderedRows.indexOf(AnchorGuid);
   if(ComponentIdx === AnchorIdx + 1) return;
   DASHinsertIndexAfterAnchor(ComponentIdx, AnchorIdx);
   DASHswapRowPositions(ComponentIdx, AnchorIdx);
   DASHlistChanged = true;
}

function DASHswapRowPositions(fromIndex, toIndex){
   let element = DASHrenderedRows[fromIndex];
   DASHrenderedRows.splice(fromIndex, 1);
   DASHrenderedRows.splice(toIndex + 1, 0, element);
}

function DASHremoveRow(ComponentGuid){
   const index = DASHrenderedRows.indexOf(ComponentGuid);
   DASHrenderedRows.splice(index, 1);
   document.querySelectorAll('.DASHtableRow')[index].remove();
}

function DASHresultsCount() {
   let Results = '';
   let TotalCount = DASHcomponentCount();
   if(!TotalCount) Results = "There are 0 components to display";
   else{
      let componentPlural = TotalCount === 1 ? 'component' : 'components';
      Results = `Displaying ${DASHlistData().length} of ${TotalCount} ${componentPlural}`
   }
   let div = document.querySelector('.DASHresultCount');
   if (Results != div.innerHTML) div.innerHTML = Results;
}

function DASHupdateCardArrows(){
   CARDupdateNext();
   CARDupdatePrev();
}
