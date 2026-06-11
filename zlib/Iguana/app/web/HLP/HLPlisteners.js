function HLPsetListeners() {
   const HLPmodal = document.querySelector('.HLPmodal');
   HLPmodal?.removeEventListener("click", HLPtoolbarClick, true);
   HLPmodal?.addEventListener("click", HLPtoolbarClick);
}

function HLPtoolbarClick(event) { if(event.target?.closest('.HLPhelpButton')) { HLPtoolbarHelpClick(); } }

function HLPtoolbarHelpClick() {
   HLPisWindowOpen() ? HLPcloseWindow() : HLPlistAll();
   MODELforcePoll('TRANSmodel');
   TRANSsetCode();
}

function HLPregisterViewListeners(FunctionHelp) {
   document.querySelector('.HLPcontainer')?.addEventListener('click', function(event) {
      if(event.target.classList.contains('HLPeditButton')) { HLPeditFunction(FunctionHelp); }
   });
   document.querySelector(".HLPdescriptionContent")?.addEventListener('click', HLPlinkClick);
}

function HLPlinkClick(e){
   if (e.target.pathname){
      let PathList = e.target.pathname.split('@');
      if (PathList.length === 2){
         e.preventDefault();
         HLPlistAll(PathList[1]);
      }
   }
}

function HLPregisterEditListeners() {
   const HLPtable = document.querySelector('.HLPcontainer');
   HLPtable?.addEventListener('click', HLPhandleTableClick);
   HLPtable?.addEventListener('focusin', HLPhandleFocusIn);
   HLPtable?.addEventListener('focusout', HLPhandleFocusOut);
   HLPtable?.addEventListener('dragstart', HLPhandleDragStart);
   HLPtable?.addEventListener('dragend', HLPhandleDragEnd);
   HLPtable?.addEventListener('dragover', HLPhandleDragOver);
}

function HLPhandleTableClick(event) {
   const target = event.target;
   if (target?.classList.contains('HLPcancelButton'))        { return HLPloadFunction(HLPcurrentFile); }
   if (target?.classList.contains('HLPsaveButton'))          { return HLPsave(); }
   if (target?.classList.contains('HLPaddListParameter'))    { return HLPaddParameter('.HLPlistParameterRows', HLPeditListBlankParameterRowHtml); }
   if (target?.classList.contains('HLPaddTableParameter'))   { return HLPaddParameter('.HLPtableParameterRows', HLPeditTableBlankParameterRowHtml); }
   if (target?.classList.contains('HLPisTable'))             { return HLPtoggleTableDisplay(target); }
   if (target?.closest('.HLPdeleteRow'))                     { return target.closest('.HLProw')?.remove() }
}

function HLPaddParameter(containerSelector, blankParameterHtml) {
   const row = document.createElement('div');
   row.innerHTML = blankParameterHtml();
   new Quill(row.children[0].querySelector('.HLPparameterDesc'), HLPparamQuillOptions());
   document.querySelector(containerSelector)?.appendChild(row.children[0]);
}

function HLPtoggleTableDisplay(target) {
   const listDisplay  = target?.checked ? 'none' : 'block';
   const tableDisplay = target?.checked ? 'block' : 'none';
   document.querySelector('.HLPlist').style.display = listDisplay;
   document.querySelector('.HLPtable').style.display = tableDisplay;
}

function HLPhandleFocusIn(event) {
   const input = event.target.closest('.HLPinput');
   const row   = input?.closest('.HLProw');
   if(row) { row.draggable = false; }
}

function HLPhandleFocusOut(event) {
   const input = event.target.closest('.HLPinput');
   const row   = input?.closest('.HLProw');
   if(row) { row.draggable = true; }
}

function HLPhandleDragStart(event) {
   event.stopPropagation();
   const row = event.target.closest('.HLProw');
   if(row) { row.classList.add('HLPdragging'); }
}

function HLPhandleDragEnd(event) {
   const row = event.target.closest('.HLProw');
   if(row) { row.classList.remove('HLPdragging'); }
}

function HLPhandleDragOver(event) {
   event.preventDefault();
   event.stopPropagation();
   const draggedItem  = document.querySelector('.HLPdragging');
   const afterElement = HLPdragSortListGetPosition(event.clientY, draggedItem.parentElement);
   draggedItem.parentElement.insertBefore(draggedItem, afterElement);
}


function HLPdragSortListGetPosition(y, table) {
   const dragSortListItems = Array.from(table.querySelectorAll('.HLProw:not(.HLPdragging)'));
   return dragSortListItems.reduce((closest, listItem) => {
      const offset = y - listItem.getBoundingClientRect().top - listItem.offsetHeight / 2;
      return (offset < 0 && offset > closest.offset) ? {offset, element: listItem} : closest;
   }, {offset: Number.NEGATIVE_INFINITY}).element;
}

function HLPsetupSearchListener(Data) {
   const searchElement = document.querySelector('.HLPsearch');
   searchElement.addEventListener('input', (e) => {
      e.stopPropagation();
      HLPfilter(Data, searchElement.value);
   });
}