let TUNrenderedRows = ["TUNtableHeader"];
let TUNrenderedData = {};

function TUNlistUpdate(){
   TUNremoveStaleRows();
   TUNhandleRowContent();
   TUNreorganizePositions();
}

function TUNresetGlobals(){
   TUNrenderedRows = ["TUNtableHeader"];
   TUNrenderedData = {};
}

function TUNremoveRow(Name){
   const index = TUNrenderedRows.indexOf(Name);
   TUNrenderedRows.splice(index, 1);
   document.querySelectorAll('.TUNtableRow')[index].remove();
}

function TUNremoveStaleRows(){
   for(let i = TUNrenderedRows.length - 1; i > 0 ; i--) {
      let Name = TUNrenderedRows[i];
      if(!TUNpeerServerData().hasOwnProperty(Name)){
         TUNremoveRow(Name);
         delete TUNrenderedData[Name];
      }
   }
}

function TUNhandleRowContent(){
   Object.keys(TUNpeerServerData()).length === 0 ? TUNpeerSetupHtml() : TUNinsertHeadingHtml();
   let ListData = JSON.parse(JSON.stringify(TUNpeerServerData()));
   let OldData  = JSON.parse(JSON.stringify(TUNrenderedData));
   for(let K in ListData) {
      let Name = K;
      let New = ListData[Name]?.["authenticated"];
      let Old = OldData[Name]?.["authenticated"];
      let Same = DEEPequal(New, Old);
      if(Same) continue;
      let RowHtml = TUNlistRowHtml(Name, New);
      if(TUNrowAlreadyRendered(Name)) TUNupdateRowContent(Name, RowHtml);
      else                            TUNaddComponentRow (Name, RowHtml);
      TUNrenderedData[Name] = New;
   }
}

function TUNaddComponentRow(Name, RowHtml) {
   TUNrenderedRows.push(Name);
   const container = document.querySelector('.TUNtableRow:last-of-type');
   if(container) { return container.insertAdjacentHTML('afterend', RowHtml); }
   const listDiv = document.querySelector('.TUNlistDiv');
   if(listDiv) { listDiv.insertAdjacentHTML('beforeend', RowHtml); }
}

function TUNcomponentRowSelector(Name){ return document.querySelectorAll('.TUNtableRow')[TUNrenderedRows.indexOf(Name)]; }

function TUNupdateRowContent(Name, RowHtml){
   let Row = TUNcomponentRowSelector(Name);
   if (Row) { Row.outerHTML = RowHtml; }
}

function TUNrowAlreadyRendered(Name){ return TUNrenderedRows.indexOf(Name) !== -1;  }

function TUNreorganizePositions(){
   let ListData = TUNpeerServerData();
   let Keys = Object.keys(ListData);
   for(let i = 0; i < Keys.length; i++) {
      let Name = Keys[i];
      let AnchorName = TUNrenderedRows[i];
      TUNinsertAfterAnchor(Name, AnchorName);
   }
}

function TUNinsertAfterAnchor(Name, AnchorName){
   let NameIdx   = TUNrenderedRows.indexOf(Name);
   let AnchorIdx = TUNrenderedRows.indexOf(AnchorName);
   if(NameIdx === AnchorIdx + 1) return;
   TUNinsertIndexAfterAnchor(NameIdx, AnchorIdx);
   TUNswapRowPositions(NameIdx, AnchorIdx);
}

function TUNinsertIndexAfterAnchor(CurrentIdx, AnchorIdx){
   let RenderedRow = document.querySelectorAll('.TUNtableRow')[CurrentIdx];
   let AnchorRow   = document.querySelectorAll('.TUNtableRow')[AnchorIdx ];
   TUNinsertAfter(RenderedRow, AnchorRow);
}

function TUNinsertAfter(newNode, referenceNode){ referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling); }

function TUNswapRowPositions(fromIndex, toIndex){
   let element = TUNrenderedRows[fromIndex];
   TUNrenderedRows.splice(fromIndex, 1);
   TUNrenderedRows.splice(toIndex + 1, 0, element);
}