function WNDselector(WindowId){
   if(document.querySelector(`#${WindowId}`))       return document.querySelector(`#${WindowId}`);
   if(document.querySelector(`#${WindowId}docked`)) return document.querySelector(`#${WindowId}docked`);
}

function WNDcontentSelector(WindowId){
   if(document.querySelector(`#${WindowId}`))       return document.querySelector(`#${WindowId} .WNDcontent`);
   if(document.querySelector(`#${WindowId}docked`)) return document.querySelector(`#content-_${WindowId}docked .WNDcontent`);
}

function WNDcloseWindow(WindowId){
   if(document.querySelector(`#${WindowId}`)) return document.querySelector(`#${WindowId}`).remove();
   if(document.querySelector(`#${WindowId}docked`)){
      let dockId = document.querySelector(`#${WindowId}docked`).closest('.WNDdock').id;
      WNDcloseTab(WindowId+"docked", dockId);
   }
}

// TODO - These helper functions are very reliant on jQuery and also just kind of gross
function WNDdockExists(dockId){ return document.getElementById(dockId) !== null; }

function WNDdockedKey (){ return "docked"; }
function WNDcontentKey(){ return "content-_"; }
function WNDrootKey()   { return "root-_"; }

function WNDtabId    (id){ return WNDwindowId(id) + WNDdockedKey(); }
function WNDcontentId(id){ return WNDcontentKey() + WNDwindowId(id) + WNDdockedKey(); }
function WNDwindowId (id){ return ((id.replace(WNDdockedKey(),"")).replace(WNDcontentKey(),"")).replace(WNDrootKey(),""); }

function WNDtabTitle(id){ 
   return document.querySelector(`#${WNDtabId(id)}`)?.querySelector(".WNDtitle")?.innerHTML || ""; 
}
function WNDtabIcon(id){ 
   return document.querySelector(`#${WNDtabId(id)}`)?.querySelector(".WNDtitleIcon")?.classList[1] || ""; 
}

function WNDnumberTabId(number, dockId){ return WNDtabsListChildren(dockId)[number].id; }
function WNDdockContent(id){ return document.querySelector(`#${WNDcontentId(id)}`); }

function WNDtabs(dockId){ return [...WNDtabsListChildren(dockId)].map(item => item.id); }

function WNDwindows(){
   const windowNames = [];
   document.querySelectorAll('.WNDwindow').forEach(window => windowNames.push(window.id));
   return windowNames;
}

function WNDtabNumber(id, dockId){
   COL_TRC("dockId=", dockId);
   let tabs = WNDtabsListChildren(dockId);
   return (tabs.length) ? [...tabs].findIndex(item => item.id == id) : -1; 
}

function WNDtabsListChildren(dockId) { 
   let tabs = WNDtabsList(dockId);
   COL_TRC("tabs=", tabs);
   let Result = (tabs) ? tabs.children : [] ; 
   Result = Result ? Result : [];
   COL_TRC("Result=", Result);
   return Result;
}

function WNDtabsList(dockId){
   COL_TRC("dockId=", dockId);
   let tabs = document.querySelector(`#${dockId}`)?.querySelector("#WNDdockTabsList");
   let Result = (tabs) ? tabs : [] ; 
   COL_TRC("Result=", Result);
   return Result;
}
function WNDdockContentTarget(dockId){ return document.querySelector(`#${dockId}`).querySelector("#WNDdockContent"); }

function WNDelementsUnderCursor(e){ return document.elementsFromPoint(e.clientX, e.clientY); }

function WNDidBelowCursor(e,classname){   return (WNDelementsUnderCursor(e).filter(item => item.classList[0] == classname).length) ? WNDelementsUnderCursor(e).filter(item => item.classList[0] == classname)[0].id : ""; }

function WNDitemBelowCursor(e,classname){ return (WNDelementsUnderCursor(e).filter(item => item.classList[0] == classname).length) ? WNDelementsUnderCursor(e).filter(item => item.classList[0] == classname) : ""; }

function WNDremoveOuterPopouts(){ 
   document.querySelectorAll('.ANNouterPopup').forEach(popup => {
      const Title = popup.querySelector('.title').innerText;
      if(Title !== "Refreshing…"){
         const WindowId = WNDgetIdDecremented(popup.querySelector('.ui-dialog-title').id);
         const Icon = popup.querySelector('.title').classList[2];
         if(WNDisWindow(WindowId)){ WNDsetTitle(WNDinnerIdToWindowId(WindowId),Title,Icon); }
         else if(WNDisTab(WNDuiIdToWindowId(WindowId)) ){ 
            WNDupdateTabTitle(WNDwindowIdToTabId(WNDuiIdToWindowId(WindowId)), Title); 
            WNDupdateTabIcon(WNDwindowIdToTabId(WNDuiIdToWindowId(WindowId)), Icon);
         }
         popup.remove();
      }
   });
}

function WNDgetIdDecremented(id){
   PopId = id.split("-");
   const PopIdNum = Number(PopId.pop()) - 1;
   PopId.push(PopIdNum);
   PopId = PopId.join('-');
   return PopId;
}

function WNDisTab(id)    { return document.querySelector(`#${WNDwindowIdToTabId(id)}`) ? true : false; }

function WNDisWindow(id) { return document.querySelector(`#${id}`)?.closest(".WNDwindow") ? true : false; }

function WNDuiIdToWindowId(id) { return document.querySelector(`#${id}`)?.querySelector(".link_id")?.value || ""; }

function WNDwindowIdToTabId(id){ return "WND" + id + "docked"; }

function WNDupdateTabTitle(id, Title){ 
   let tab = document.querySelector(`#${id}`)?.querySelector(".WNDtitle");
   if(tab) tab.innerText = Title; 
}

function WNDupdateTabIcon(id, Icon){ 
   const TitleIcon = document.querySelector(`#${id}`)?.querySelector(".WNDtitleIcon");
   if(TitleIcon) TitleIcon.remove();
   const TitleGroup = document.querySelector(`#${id}`)?.querySelector(".WNDtitleGroup");
   const iconDiv = document.createElement('div');
   iconDiv.className = `WNDtitleIcon ${Icon}`;
   TitleGroup?.prepend(iconDiv);
}

function WNDannotationToWindowId(Id){
   return "WND" + Id;
}

function WNDdestroyDockContent(dockId){ 
   document.querySelector(`#${dockId}`)?.querySelectorAll('.ANNpopup').forEach(popup => {
      const id = WNDannotationToWindowId(popup.querySelector('input.link_id').value);
      ANNclearFromLocalStorageOnClose(id);
      popup.remove();
   });
}

function WNDdestroyContent(id){  
   const div = document.querySelector(`#${id}`)?.querySelector(".ANNpopup");
   if(div){
      const itemid = WNDwindowId(id);
      TRANSremoveFromPopupStack(itemid);
      ANNclearFromLocalStorageOnClose(itemid);
      div.remove();
   } 
}

function WNDinnerIdToWindowId(id){ 
   const window = document.querySelector(`#${id}`)?.closest(".WNDwindow");
   return window?.id || "";
}

function WNDdockId(event){ return (event.currentTarget.closest(".WNDdock")) ? event.currentTarget.closest(".WNDdock").id : "" ; }

function WNDeventToId(event, className){
   return event.target.closest("."+className)?.id || ""; 
}

function WNDeventId(event){ 
   if(event.target.closest(".WNDwindow")) { 
      return event.target.closest(".WNDwindow").id; 
   } else if(event.target.closest(".WNDcontent") && event.target.closest(".WNDcontent").parentNode){ 
      return WNDwindowId(event.target.closest(".WNDcontent").parentNode.id); 
   }
}

function WNDidToDockId(id){ 
   const content = document.querySelector(`#${WNDcontentId(id)}`);
   return content?.closest(".WNDdock")?.id || "";
}

function WNDdocksOpen(){
   const Docks = [];
   document.querySelectorAll('.WNDdock').forEach(dock => {
      Docks.push(dock.id);
   });
   return Docks;
}

function WNDpickDock(DockId = ""){
   if(DockId) return DockId;
   const Docks = WNDdocksOpen();
   if(Docks.length == 1){ return Docks[0]; }
   else                 { return "WNDdockBottom"; }
}
