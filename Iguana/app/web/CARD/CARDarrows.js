function CARDarrowsHtml(){
   return /*html*/`
   <a class="CARDarrowPrev CARDdisabledArrow BUTTONstandard BUTTONdefault"><div class="DETarrowFont">▲</div></a>
   <a class="CARDarrowNext CARDdisabledArrow BUTTONstandard BUTTONdefault"><div class="DETarrowFont">▼</div></a>`;
}

function CARDdetermineArrow(Component, Offset){
   let Data = DASHlistData();
   let ComponentIdx = Data.findIndex(item => item.guid === Component);
   let ArrowIdx = ComponentIdx + Offset;
   if(ArrowIdx < 0) return null;
   if(ArrowIdx >= Data.length) return null;
   return Data[ArrowIdx].guid;
}

function CARDupdatePrev(){
   CARDupdateArrow(document.querySelector(".CARDarrowPrev"), CARDcomponentId(), -1);
}

function CARDupdateNext(){
   CARDupdateArrow(document.querySelector(".CARDarrowNext"), CARDcomponentId(), 1);
}

function CARDupdateArrow(div, Guid, Offset){
   let NextGuid = CARDdetermineArrow(Guid, Offset);
   if(!div) return;
   if(!NextGuid) { div.classList.add("CARDdisabledArrow"); }
   else          { div.classList.remove("CARDdisabledArrow"); }
   if(!NextGuid) NextGuid = Guid;
   let Args = DASHpreviousFilter;
   Args.component = NextGuid;
   let href = `#dashboard/details${PAGEformatParams(Args)}`;
   if(div.href !== href) div.href = href;
}