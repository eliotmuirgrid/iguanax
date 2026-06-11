let DRGcallbackDiv = {};

function DRGcreateDragSortList(Params = {}){
   const Anchor = Params.anchor || 'body';
   const Id = DRGgenerateWindowGuid();
   if(Params.callback) {
      DRGcallbackDiv[Id] = Params.callback;
   }
   let NewDiv = document.createElement('div');
   NewDiv.classList.add('DRGdragSortList');
   NewDiv.id = Id;
   document.querySelector(Anchor).appendChild(NewDiv);
   DRGdragSortListSetupListeners(NewDiv);
   return Id;
}

function DRGdragSortListAddItem(id, Content, Params = {}){
   const DragHandle = Params.dragHandleSelector;
   if(!DragHandle) return;
   let List = document.querySelector(`#${id}`);
   if(!List) return;
   let NewDiv = document.createElement('div');
   if (DragHandle) {
      NewDiv.classList.add('DRGdragSortListItem--Draggable', 'DRGdragSortListItem');
      NewDiv.removeAttribute('draggable');
   } else {
      NewDiv.setAttribute('draggable', false);
   }
   NewDiv.innerHTML = Content;
   List.appendChild(NewDiv);
   //if(window.navigator.userAgent.includes('Mozilla')) DRGmozillaListener();
   if(Params.dragHandleSelector) DRGattachDragHandle(NewDiv, Params.dragHandleSelector);
   if(DRGcallbackDiv[id]) DRGcallbackDiv[id](id);
}

function DRGattachDragHandle(Row, dragHandleSelector){
   const DragHandle = Row.querySelector(dragHandleSelector);
   if(!DragHandle) return;
   DragHandle.addEventListener('mousedown', function (e) { Row.setAttribute('draggable', true);});
   Row.addEventListener('dragend', function (e) { Row.removeAttribute('draggable');});
   Row.addEventListener('dragcancel', function (e) { Row.removeAttribute('draggable');});
}

function DRGenableDrag(e) {
   e.currentTarget.setAttribute('draggable', true);
}
 
function DRGdisableDrag(e) {
   e.currentTarget.removeAttribute('draggable');
}
 
function DRGremoveDragHandle(Row, dragHandleSelector){
   const DragHandle = Row.querySelector(dragHandleSelector);
   if (!DragHandle) return;
   DragHandle.removeEventListener('mousedown', DRGenableDrag);
   Row.removeEventListener('dragend', DRGdisableDrag);
   Row.removeEventListener('dragcancel', DRGdisableDrag);
}
/*
function DRGonDragFocus(e){
   COL_TRC(e);
   e.currentTarget.closest('.DRGdragSortListItem--Draggable').setAttribute("draggable", false); 
}

function DRGonDragBlur(e){
   COL_TRC(e);
   e.currentTarget.closest('.DRGdragSortListItem--Draggable').setAttribute("draggable", true); 
}

function DRGmozillaListener(){
   COL_TRC($('.DRGdragSortList input, .DRGdragSortList textarea'));
   //$('.DRGdragSortList input, .DRGdragSortList textarea').off('focus').on('focus', DRGonDragFocus);
   //$('.DRGdragSortList input, .DRGdragSortList textarea').off('blur').on('blur',   DRGonDragBlur);
}*/

function DRGidFromRow(row){
   let div = row.closest(".DRGdragSortList");
   if(!div) return;
   return div.id;
}

function DRGonDragEnd(e, ListDiv ){
   let AfterElement = DRGdragSortListGetPostion(e.y);
   if (AfterElement == null){
      COL_TRC("Append to the end");
      ListDiv.append(DRGdraggedItem); 
   }
   else { 
      COL_TRC("Insert before");
      ListDiv.insertBefore(DRGdraggedItem, AfterElement); 
   }
   if(e.target.closest('.DRGdragSortListItem--Draggable')) e.target.closest('.DRGdragSortListItem--Draggable').classList.remove('DRGdragSortListItem--Dragging');
   let Id = DRGidFromRow(e.target);
   if(DRGcallbackDiv[Id]) DRGcallbackDiv[Id](Id);
}

function DRGdragSortListSetupListeners(ListDiv){
   if(!ListDiv) return;
   ListDiv.addEventListener('dragstart', function(e){ DRGdragStart(e, ListDiv); });
   ListDiv.addEventListener("dragend", function(e){ DRGonDragEnd(e, ListDiv); });
   ListDiv.addEventListener("dragover", (e)=>{
      e.preventDefault();
      e.stopPropagation();
   });
}

let DRGdraggedItem;

function DRGdragStart(e, ListDiv){
   if(e.target.closest('.DRGdragSortListItem--Draggable')) {
      e.target.closest('.DRGdragSortListItem--Draggable').classList.add('DRGdragSortListItem--Dragging');
      DRGdraggedItem  = ListDiv.querySelector(".DRGdragSortListItem--Dragging");
   }
};

function DRGgetNonDraggingItems(container) {
   return Array.from(
       container.querySelectorAll('.DRGdragSortListItem:not(.DRGdragSortListItem--Dragging)')
   );
}

function DRGcalculateOffset(y, element) {
   const box = element.getBoundingClientRect();
   return y - box.top - box.height / 2;
}

function DRGdragSortListGetPostion(y) {
   const container = DRGdraggedItem.closest('.DRGdragSortList');
   const items = DRGgetNonDraggingItems(container);
   
   let closestElement = null;
   let closestOffset = Number.NEGATIVE_INFINITY;

   COL_TRC(items);
   for (let item of items) {
       const offset = DRGcalculateOffset(y, item);
       if (offset < 0 && offset > closestOffset) {
           closestOffset = offset;
           closestElement = item;
       }
   }
   COL_TRC("closestElement", closestElement);
   return closestElement;
}

function DRGgenerateWindowGuid(){ return "DRG" + ([1e7]+-1e3+-4e3+-8e3+-1e11).replace(/[018]/g, c => (c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> c / 4).toString(16) ); }
