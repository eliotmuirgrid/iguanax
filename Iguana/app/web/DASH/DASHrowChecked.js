let DASHcheckedComponents = {};
let DASHscrollToComponent = [];

function DASHlistOfCheckedComponents(){
   let A =[];
   for (let K in DASHcheckedComponents) {
      A.push(K);
   }
   return A;
}

function DASHsyncCheckedRowsDom() {
   for(let i = 1; i < DASHrenderedRows.length; i++) {
      let guid = DASHrenderedRows[i];
      let box  = DASHcheckboxSelector(guid);
      if(box) { box.checked = !!DASHcheckedComponents[guid]; }
   }
}

function DASHsetCheckedComponents(ComponentGuids = [], {emitToCyto = true} = {}) {
   DASHcheckedComponents = {};
   for(let i = 0; i < ComponentGuids.length; i++) {
      DASHcheckedComponents[ComponentGuids[i]] = true;
   }
   DASHsyncCheckedRowsDom();
   DASHrefreshCheckedRows({emitToCyto});
}

function DASHupdateCheckedComponents(RequestComponents = [], {emitToCyto = true} = {}){
   let checkedRows = document.querySelectorAll('.DASHselectRow:checked'); //.closest('.DASHcomponentRow');
   DASHcheckedComponents = {};
   for (let i=0; i < RequestComponents.length; i++){
      DASHcheckedComponents[RequestComponents[i]] = true;
   }
   for (let i = 0; i < checkedRows.length; i++) {
      let guid = DASHrowGuid( DASHrowIndex(checkedRows[i].closest('.DASHcomponentRow')) );
      DASHcheckedComponents[guid] = true;
   }
   DASHrefreshCheckedRows({emitToCyto});
}

function DASHselectAll() { DASHsetRowsChecked(DASHrenderedRows, document.querySelector('.DASHselectAllRow').checked ); }

function DASHsetRowsChecked(ComponentGuids, flag, {emitToCyto = true} = {}) {
   let RequestComponents = [];
   for( let i = 0; i < ComponentGuids.length; i++){
      let guid = ComponentGuids[i];
      if(guid == "DASHtableHeader") continue;
      let box = DASHcheckboxSelector(ComponentGuids[i]);
      if(box) box.checked = flag;
      else    RequestComponents.push(ComponentGuids[i]);
   }
   DASHupdateCheckedComponents(RequestComponents, {emitToCyto});
}

function DASHhandleCheckedRowHighlight(){
   let Boxes = document.querySelectorAll('.DASHselectRow');
   for (let i =0; i < Boxes.length; i++){
      let Box = Boxes[i];
      let Row = Box.closest('.DASHcomponentRow');
      if(Box.checked) Row.classList.add   ('DASHrowChecked');
      else            Row.classList.remove('DASHrowChecked');
   }
}

function DASHrefreshCheckedRows({emitToCyto = true, forcePoll = DASHshowOnlyCheckedRows()} = {}){
   DASHsyncCheckedRowsDom();
   DASHhandleCheckedRowHighlight();
   DASHhandleSelectToolbar();
   DASHhandleSelectAll();
   if(emitToCyto) { DASHbridgeCheckedSetEmit(DASHlistOfCheckedComponents()); }
   if(forcePoll) { MODELforcePoll('DASHmodel'); }
}

function DASHhandleSelectAll() {
   const selectAllRow = document.querySelector('.DASHselectAllRow');
   if(selectAllRow) {
      const selected       = document.querySelectorAll('.DASHselectRow:checked')?.length;
      selectAllRow.checked = selected !== 0 && selected === DASHlistData().length;
   }
}

function DASHhandleSelectToolbar() {
   let CheckedRowCount = document.querySelectorAll('.DASHselectRow:checked').length;
   document.querySelector('.DASHselectToolbar select').disabled = CheckedRowCount === 0;
   let ShowOnlyCheckedSwitchBox = document.querySelector('.DASHshowOnlyCheckedSwitchBox');
   if (CheckedRowCount !== 0) ShowOnlyCheckedSwitchBox.classList.remove('DASHdisplayNone');
   else {
      let CheckedOnly = document.querySelector('.DASHonlyChecked');
      if (CheckedOnly) { CheckedOnly.checked = false; }
      ShowOnlyCheckedSwitchBox.classList.add('DASHdisplayNone');
   }
}
