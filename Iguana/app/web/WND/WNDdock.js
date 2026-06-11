function WNDdockContentHtml(tab_id){
  return /*html*/`<li class="WNDdockContentListItem" id='${tab_id}'></li>`;
}

function WNDdockHtml(config){
  let dockId        = config.dockId ? config.dockId : "";
  let WNDresizeDock = config.WNDresizeDock ? config.WNDresizeDock : "";
  return /*html*/`
  <div id='${dockId}' class='WNDdock '>
    <div class='${WNDresizeDock}'></div>
    <div id='WNDdockTabs'>
        <ul id='WNDdockTabsList'></ul>
        <div class="WNDtitleButtons">
          <div class='WNDtitleDockAll-${dockId}' title="Dock All Windows"><img src="/WND/images/dock-all.svg" alt="Dock All Windows"></div>
          <div class='WNDundockAll' title="Undock Tabs"><img src="/WND/images/pop-from-bottom.svg" alt="Undock Tabs"></div>
          <div class='WNDcloseDock' title="Close Dock"><img src="/WND/images/close-window.svg"    alt="Close Dock"></div>
        </div>
    </div>
    <div class="WNDdockContent">
        <ul id='WNDdockContent'></ul>
    </div>
  </div>`;
}

function WNDdockInit(dockId){
   if(dockId == "WNDdockBottom") {
         const dock = WNDdockHtml({dockId : dockId, WNDresizeDock : WNDresizeClass(dockId)});
         const phantom = document.querySelector(".WNDdockBottomPhantom");
         if(!phantom) return;
         phantom.insertAdjacentHTML('afterend', dock);
         phantom.style.display = 'block';
   }
   if(dockId == "WNDdockRight")  {
      const dock = WNDdockHtml({dockId : dockId,  WNDresizeDock : WNDresizeClass(dockId)});
      const phantom = document.querySelector(".WNDdockRightPhantom");
      phantom.insertAdjacentHTML('afterend', dock);
      phantom.style.display = 'block';
   }
   WNDsetDockListeners();
}

function WNDresizeClass(dockId){
  if(dockId == "WNDdockBottom"){return "WNDresizeDockY"; }
  if(dockId == "WNDdockRight") {return "WNDresizeDockX"; }
  return "WNDresizeDock";
}

function WNDaddToDock(id, dockId, dragged = false, TabPos = ""){
  let pos;
  if ( !WNDdockExists(dockId) ) WNDdockInit(dockId);
  if(TabPos == "" || TabPos == -1){ pos = WNDtabsListChildren(dockId).length-1; }
  let flag = false;
  if(dragged&&TabPos!=-1){
    pos = TabPos;
    flag = true;
  }
  WNDhideContent(dockId);
  WNDaddTab(WNDtabId(id), document.querySelector(`#${id}`)?.querySelector(".WNDtitleGroup")?.innerHTML, dockId, pos, flag);
  WNDaddContent(id, dockId);
  WNDcloseWin(id);
  ANNaddToLocalStorageOnOpen(id, dockId);
  WNDtabSelected(id);
  WNDsetDockListeners();
  HLPadjustHeaderPosition();
  const phantom = document.querySelector(`#${dockId}Phantom`);
  if (phantom) phantom.style.display = 'none';
  WNDupdateDockDimensions();
  if(id !== "TGITterminal" ) setTimeout(TRANSsetCode, 200);
}

function WNDshowTabNumber(number, dockId){ WNDshowContent(WNDnumberTabId(number,dockId)); }

function WNDhideContent(dockId){ 
   if(WNDdockContentTarget(dockId)) for (let i of WNDdockContentTarget(dockId).children){ 
      i.style.display = 'none';
   } 
}

function WNDshowContent(id){ WNDdockContent(id).style.display = 'block'; }

function WNDtabSelected(id, notClicked = true){
   WNDdeSelectTabs(WNDidToDockId(id));
   if(notClicked){ WNDdockHighlight(WNDidToDockId(id)); }
   document.querySelector(`#${WNDtabId(id)}`)?.classList.add("selected");
   WNDhideContent(WNDidToDockId(id));
   WNDshowContent(id);
}

function WNDtabDeSelected(id){ document.querySelector(`#${WNDtabId(id)}`)?.classList.remove("selected"); }
function WNDdeSelectTabs(dockId){ for (let i of WNDtabs(dockId)){ WNDtabDeSelected(i); } }

function WNDaddContent(id, dockId){
   const content = WNDdockContentHtml( WNDcontentId(id) );
   const contentElement = document.createElement('div');
   contentElement.innerHTML = content;
   WNDdockContentTarget(dockId).appendChild(contentElement.firstChild);
   let div = document.querySelector(`#${id}`).querySelector(".WNDcontent");
   div.parentNode.removeChild(div);
   document.querySelector(`#${WNDcontentId(id)}`).appendChild(div);
}

function WNDtabHtml(dockId, id, title){
   let button;
   if (dockId == "WNDdockBottom") button = WNDdockRightButtonHtml();
   else                           button = WNDdockBottomButtonHtml();
   return /*html*/`
   <li id = ${id} class='WNDtab'>
      <div class="WNDbuttonGroup">
         <div class='WNDcloseTab'  title="Close Tab"> <img src="/WND/images/close-window.svg"    alt="Close Tab" ></div>
         <div class='WNDundockTab' title="Undock Tab"><img src="/WND/images/pop-from-bottom.svg" alt="Undock Tab"></div>
         ${button}
      </div>
      <div class="WNDtitleGroup">
         ${title}
      </div>
   </li>`;
}

function WNDaddTab(id, Title, dockId, pos, flag = false){
   let TabHtml = WNDtabHtml(dockId, id, Title);
   let tabs = WNDtabsList(dockId);
   if(!tabs.childElementCount){
      const tabElement = document.createRange().createContextualFragment(TabHtml).firstElementChild;
      tabs.appendChild(tabElement);
   }
   else if (!flag) { 
      let tab = document.querySelector(`#${WNDtabId(WNDnumberTabId(pos,dockId))}`);
      const tabElement = document.createRange().createContextualFragment(TabHtml).firstElementChild;
      tab.parentNode.appendChild(tabElement);
   }
   else { 
      let tab = document.querySelector(`#${WNDtabId(WNDnumberTabId(pos,dockId))}`);
      const tabElement = document.createRange().createContextualFragment(TabHtml).firstElementChild;
      tab.parentNode.insertBefore(tabElement, tab);
   }
}

function WNDremoveTab(id)    { document.querySelector(`#${WNDtabId(id)}`)?.remove(); }
function WNDremoveContent(id){ document.querySelector(`#${WNDcontentId(id)}`)?.remove(); }

function WNDundockAllTabs(dockId)  { for (let i of WNDtabs(dockId)) { WNDundockTab(i, dockId); } }
function WNDdockAllWindows(DockId = WNDpickDock()) { for (const i of WNDwindows()){ WNDaddToDock(i, DockId); } }
function WNDcloseAllWindows(){ for (const i of WNDwindows()){ WNDcloseWin(i); } }

function WNDundockTab(id, dockId, PosX = "", PosY = "", force=false){
   WNDcreateWindowWithOptions({
      ID        : WNDwindowId(id),
      Title     :  WNDtabTitle(id),
      Content   : "",
      PositionX : PosX,
      PositionY : PosY,
      Height    : "400px",
      Width     : "500px",
      iconClass : WNDtabIcon(id),
    });
    let content = document.querySelector(`#${WNDcontentId(id)}`).querySelector(".WNDcontent").children[0];
    if (!content) return;
    content.parentNode.removeChild(content);
    let undockedWindowContent = document.querySelector(`#${WNDwindowId(id)}`)?.querySelector(".WNDcontent");
    if (!undockedWindowContent) return;
    undockedWindowContent.appendChild(content);
    WNDcloseTab(id, dockId);
    WNDhandleResize(WNDwindowId(id), force);
    HLPadjustHeaderPosition();
}

function WNDcloseTab(id, dockId){
  WNDdestroyContent(WNDcontentKey()+id);
  const tabCount = WNDtabsListChildren(dockId).length - 1;
  if(tabCount == 0){ WNDcloseDock(dockId); return MODELupdate('TRANSmodel'); }
  let position = WNDtabNumber(id, dockId);
  if(position == tabCount){ position--; }
  WNDremoveTab(id);
  WNDremoveContent(id);
  WNDhideContent(dockId);
  WNDshowTabNumber(position,dockId);
  WNDtabSelected( WNDnumberTabId(position,dockId), false );
  MODELupdate('TRANSmodel');
}

function WNDcloseDock(dockId){
  WNDdestroyDockContent(dockId);
  document.querySelector(`#${dockId}`)?.remove();
  TRANSrefreshCodeMirror();
  WNDupdateDockDimensions();
}

function WNDtabChosen(id){
  WNDhideContent( WNDidToDockId(id) );
  WNDshowContent(WNDcontentId(id));
  WNDtabSelected(id, false);
}

function WNDupdateDockDimensions(){
  const dockRight = document.getElementById('WNDdockRight');
  const dockBottom = document.getElementById('WNDdockBottom');
  const windowEdit = document.querySelector('.TRANSwindowEdit');
  if(dockRight && dockRight.offsetWidth > windowEdit.offsetWidth - 200) dockRight.style.width = (windowEdit.offsetWidth - 200) + 'px';
  if(dockRight && dockRight.offsetWidth < 200) dockRight.style.width = 200 + 'px';
  if(dockBottom && dockBottom.offsetHeight > windowEdit.offsetHeight - 120) dockBottom.style.height = (windowEdit.offsetHeight - 120) + 'px';
  if(dockBottom && dockBottom.offsetHeight < 100) dockBottom.style.height = 100 + 'px';
  if(dockRight && dockRight.offsetWidth > windowEdit.offsetWidth - 200) dockRight.style.width = (windowEdit.offsetWidth - 200) + 'px';
  if(!dockBottom) return;
  if(dockRight) dockBottom.style.width = (windowEdit.offsetWidth - dockRight.offsetWidth) + 'px';
  else          dockBottom.style.width = windowEdit.offsetWidth + 'px';
}

function WNDresizeDockY(e, elmnt, pos3, pos4){
  let windowEdit = document.querySelector('.TRANSwindowEdit');
  let newHeight = windowEdit.offsetHeight - e.clientY;
  newHeight = newHeight > 100 ? newHeight : 100;
  let maxHeight = windowEdit.offsetHeight - 120;
  newHeight = newHeight < maxHeight ? newHeight : maxHeight;
  document.getElementById('WNDdockBottom').style.height = newHeight + 'px';
}

function WNDresizeDockX(e, elmnt, pos3, pos4){
   let div = document.querySelector('.TRANSfullScreen');
   let newWidth = div.offsetWidth - e.clientX + 80;
   newWidth = newWidth > 200 ? newWidth : 200;
   let windowEdit = document.querySelector('.TRANSwindowEdit');
   const maxWidth = windowEdit.offsetWidth - 200;
   newWidth = newWidth < maxWidth ? newWidth : maxWidth;
   document.getElementById('WNDdockRight').style.width = newWidth + 'px';
   WNDupdateDockDimensions();
}

function WNDundockTabEvent(event){ WNDundockTab(WNDeventToId(event,"WNDtab"), WNDdockId(event)); }

function WNDtabClickEvent(event){
  WNDtabChosen(WNDeventToId(event,"WNDtab"), WNDdockId(event));
}
function WNDundockAllEvent(event){ 
  WNDundockAllTabs(WNDdockId(event)); 
  TRANSsetCode();
}
function WNDcloseAllEvent(event){  
  WNDcloseDock(WNDdockId(event)); 
  TRANSsetCode();
}
function WNDcloseTabClickEvent(event){ WNDcloseTab(WNDeventToId(event,"WNDtab"),WNDdockId(event)); }

function WNDtabClicked(ev){
  if(ev.which != 1) { return; }
  let e = ev || window.event;
  const DockId = WNDdockId(ev);
  if(DockId == "WNDdockBottom") WNDbottomDockTabClicked(e, ev);
  if(DockId == "WNDdockRight")  WNDrightDockTabClicked(e, ev);
}

function WNDrightDockTabClicked(e, ev){
   const TabOver = WNDidBelowCursor(e, 'WNDtab');
   let pos3 = e.clientX;
   const pos4 = e.clientY, pos3b = e.clientX, pos4b = e.clientY;
   e.preventDefault();
   let dockRight = document.getElementById('WNDdockRight');
   let windowEdit = document.querySelector('.TRANSwindowEdit');
   let projectManager = document.querySelector('.PRJprojectManager');
   document.onmouseup = function (ev) {
      WNDescape();
      let diff = windowEdit.offsetWidth + projectManager.offsetWidth - dockRight.offsetWidth + 30 - pos3;
      if(diff <= 0) WNDtabChosen(TabOver);
   };
   document.onmousemove =  function (e) {
      pos3 = e.clientX;
      let diff = windowEdit.offsetWidth + projectManager.offsetWidth - dockRight.offsetWidth + 30 - pos3;
      if(diff > 0){
         const yPos = ((pos4 - 20) + 'px');
         if(!document.getElementById(WNDwindowId(TabOver))){
            let xNew = ((pos3b - 100) + "px");
            WNDundockTab(TabOver, WNDidToDockId(TabOver), xNew, yPos, true);
            elmnt = document.getElementById(WNDwindowId(TabOver));
            WNDhandler(WNDmoveXY,ev,WNDdragToDock,elmnt);
         }
      }
   };
}

function WNDbottomDockTabClicked(e, ev){
   const TabOver = WNDidBelowCursor(e, 'WNDtab');
   let pos3 = e.clientX;
   let pos4 = e.clientY;
   let pos4b = e.clientY;
   e.preventDefault();
   let windowEdit = document.querySelector('.TRANSwindowEdit');
   let dockBottom = document.getElementById('WNDdockBottom');
   document.onmouseup = function (e) {
      WNDescape();
      let diff = windowEdit.offsetHeight - dockBottom.offsetHeight - pos4;
      if(diff <= 0) WNDtabChosen(TabOver);
   };
   document.onmousemove =  function (e) {
      pos4 = e.clientY;
      let diff = windowEdit.offsetHeight - dockBottom.offsetHeight - pos4;
      if(diff > 0){
         const xPos = ((pos3 - 100) + 'px');
         if(!document.getElementById(WNDwindowId(TabOver))){
         let yNew = ((pos4b - 15) + "px");
         WNDundockTab(TabOver, WNDidToDockId(TabOver), xPos, yNew, true);
         elmnt = document.getElementById(WNDwindowId(TabOver));
         WNDhandler(WNDmoveXY,ev,WNDdragToDock,elmnt);
         }
      }
   };
}

function WNDdockWinBottomFromDock(event) {
   console.log("dockWinBottomFromDock");
   let id     = WNDeventToId(event, "WNDtab");
   let dockId = WNDdockId(event);
   WNDundockTab(id, dockId);
   WNDaddToDock(WNDwindowId(id), "WNDdockBottom");
   HLPadjustHeaderPosition();
}

function WNDdockWinRightFromDock(event) {
   let id     = WNDeventToId(event, "WNDtab");
   let dockId = WNDdockId(event);
   WNDundockTab(id, dockId);
   WNDaddToDock(WNDwindowId(id), "WNDdockRight");
   HLPadjustHeaderPosition();
}

function WNDsetDockListeners(){
  WNDlistener("mousedown","resizeDockY", WNDresizeDockY, WNDnull);
  WNDlistener("mousedown","resizeDockX", WNDresizeDockX, WNDnull);
  WNDlistener("click", "undockAll", WNDundockAllEvent);
  WNDlistener("click", "closeDock", WNDcloseAllEvent) ;
  WNDlistener("click", "undockTab", WNDundockTabEvent);
  WNDlistener("click", "closeTab",  WNDcloseTabClickEvent);
  WNDlistener("click", "dockWinBottom", WNDdockWinBottomFromDock);
  WNDlistener("click", "dockWinRight",  WNDdockWinRightFromDock);
  document.querySelector('.WNDtitleDockAll-WNDdockBottom')?.addEventListener('click', function(){ WNDdockAllWindows("WNDdockBottom"); TRANSsetCode(); });
  document.querySelector('.WNDtitleDockAll-WNDdockRight') ?.addEventListener('click', function(){ WNDdockAllWindows("WNDdockRight"); TRANSsetCode(); });
  document.querySelectorAll('.WNDtab').forEach(tab => tab?.addEventListener('mousedown', WNDtabClicked));
  window.addEventListener('resize', WNDupdateDockDimensions);
}

function WNDdockHighlight(dockId){
  document.querySelectorAll('.WNDdock').forEach(dock => dock.classList.remove('WNDhighlight'));
  window.setTimeout(function () {
      let div =  document.getElementById(dockId);
      if(div) div.classList.add('WNDhighlight');
      window.setTimeout(function () { document.getElementById(dockId)?.classList.remove('WNDhighlight'); }, 1000);
   }, 1);
}

function WNDcloseAllWindowsAndDocks(Cache){
  let CacheObject;
  if(Cache) CacheObject = ANNcachedObject();
  WNDcloseAllWindows();
  WNDcloseDock("WNDdockBottom");
  WNDcloseDock("WNDdockRight");
  if(Cache) ANNsaveToCache(CacheObject);
}
