let WNDmaxZ = 15;
let WNDPosX = '380px';
let WNDPosY = '60px';
const WNDconfigs = {};

function WNDwindow(WindowId, Content, Header = "", DockDirection = "", iconClass = ""){
  if(WNDisWindow(WindowId)){
    WNDforward(WindowId);
    WNDhighlight(WindowId);
    WNDsetContent(WindowId, Content);
    return false;
  }
  else if(document.querySelector(`#${WindowId}docked`)){
    WNDtabSelected(WindowId);
    document.querySelector(`#content-_${WindowId}docked .WNDcontent`).innerHTML = Content;
    return false;
  }
  else {
    WNDcreateWindow(WindowId, Content, Header, iconClass);
    if(DockDirection) { WNDaddToDock(WindowId, DockDirection); }
    return true;
  }
}

function WNDfocusOnWindow(WindowId){
  if(WNDisWindow(WindowId)){
    WNDforward(WindowId);
    WNDhighlight(WindowId);
    return true;
  }
  else if(document.querySelector(`#${WindowId}docked`)){
     WNDtabSelected(WindowId); 
     return true;
   }
   return false;
}

function WNDupdateContent(WindowId, Content){
  if(WNDisWindow(WindowId)){
    const windowContent = document.getElementById(WindowId).children[1];
    if(windowContent.innerHTML !== Content) windowContent.innerHTML = Content;
  }
  const dockedContent = document.getElementById(WindowId + 'docked');
  if(dockedContent) {
    const dockedWindow = document.querySelector(`#content-_${WindowId}docked .WNDcontent`);
    if(dockedWindow.innerHTML !== Content) dockedWindow.innerHTML = Content;
  }
}

function WNDcreateWindow(id, content, title = "", iconClass = "", PosX = "", PosY = "", height = "400px", width = "500px") {
  WNDcreateWindowWithOptions({
    ID        : id,
    Title     : ESCstripHtml(title),
    Content   : content,
    PositionX : PosX,
    PositionY : PosY,
    Height    : height,
    Width     : width,
    iconClass : iconClass
  });
}

function WNDdockRightButtonHtml(Win){
  if(!Win) { Win = ""; }
  return /*html*/`
  <div class='WNDdockWinRight${Win}' title="Dock to the Right"><img src="/WND/images/dock-to-right.svg" alt="Dock to the Right"></div>
  `;
}

function WNDdockBottomButtonHtml(Win){
  if(!Win) { Win = ""; }
  return /*html*/`
  <div class='WNDdockWinBottom${Win}' title="Dock to the Bottom"><img src="/WND/images/dock-to-bottom.svg" alt="Dock to the Bottom"></div>
  `;
}

function WNDwindowHtml(config){
  let id        = config.ID        ? config.ID        : "";
  let Height    = config.Height    ? config.Height    : "";
  let Width     = config.Width     ? config.Width     : "";
  let PositionY = config.PositionY ? config.PositionY : "";
  let PositionX = config.PositionX ? config.PositionX : "";
  let IconClass = config.iconClass ? config.iconClass : "";
  let Title     = config.Title     ? config.Title     : "";
  let Content   = config.Content   ? config.Content   : "";
  return /*html*/`
  <div id='${id}' class='WNDwindow' style='height: ${Height}; width: ${Width}; top: ${PositionY}; left: ${PositionX};'>
    <div class='WNDmoveXY'>
      <div class="WNDtitleButtons">
          <div class='WNDcloseWin'      title="Close">    <img src="/WND/images/close-window.svg"    alt="Close"></div>
          ${WNDdockRightButtonHtml("Win")}
          ${WNDdockBottomButtonHtml("Win")}
      </div>      
      <div class="WNDtitleGroup">
          ${IconClass}
          <div class="WNDtitle">${Title}</div>
      </div>
    </div>
    <div class='WNDcontent'>${Content}</div>
    <div class='WNDresizeX WNDresizeE WNDe'></div>
    <div class='WNDresizeX WNDresizeW WNDw'></div>
    <div class='WNDresizeY WNDresizeN WNDn'></div>
    <div class='WNDresizeY WNDresizeS WNDs'></div>
    <div class='WNDcorner WNDresizeES WNDes'></div>
    <div class='WNDcorner WNDresizeEN WNDen'></div>
    <div class='WNDcorner WNDresizeWS WNDws'></div>
    <div class='WNDcorner WNDresizeWN WNDwn'></div>
  </div>`;
}

function WNDcreateWindowWithOptions(config) {
  config.iconClass = WNDtitleIconHtml(config.iconClass); 
  if(!config.PositionX || !config.PositionY){ WNDupPosition(); }
  config.PositionX = (config.PositionX ? config.PositionX : WNDPosX);
  config.PositionY = (config.PositionY ? config.PositionY : WNDPosY);
  let WindowHtml = WNDwindowHtml(config);
  let Translator = document.querySelector('.TRANSwindowEdit');
  if(!Translator) return;
  Translator.insertAdjacentHTML('beforeend', WindowHtml);
  let id = config.ID;
  WNDconfigs[id] = config;
  ANNaddToLocalStorageOnOpen(id);
  WNDsetListeners();
  WNDforward(id);
  WNDhighlight(id);
  return id;
}

function WNDsetContent(id, content){ document.getElementById(id).children[1].innerHTML = content; }

function WNDtitleIconHtml(iconClass){ return /*html*/`<div class='WNDtitleIcon ${iconClass}'></div>`; }

function WNDsetTitle(id, title, iconClass = ""){
  document.querySelector(`#${id} .WNDtitle`).innerText = title;
  if(iconClass){
    let TitleIcon = document.querySelector(`#${id} .WNDtitleIcon`);
    if(TitleIcon) TitleIcon.remove();
    let IconHtml = WNDtitleIconHtml(iconClass);
    document.querySelector(`#${id} .WNDtitleGroup`).insertAdjacentHTML('afterbegin', IconHtml);
  }
}

// cascade popup windows
function WNDupPosition(){
  WNDPosX = ( (parseInt(WNDPosX, 10) + 30) < 500 ? (parseInt(WNDPosX, 10) + 30) : 350 ) + "px";
  WNDPosY = ( (parseInt(WNDPosY, 10) + 30) < 500 ? (parseInt(WNDPosY, 10) + 30) : 100 ) + "px";
}

function WNDsetListeners(){
  WNDresize(["E","W","N","S","ES","EN","WS","WN"]);
  WNDlistener("mousedown", "dockWinBottomWin", WNDdockWinBottom);
  WNDlistener("mousedown", "dockWinRightWin",  WNDdockWinRight);
  WNDlistener("mousedown", "closeWin",      WNDcloseWinEvent);
  WNDlistener("mousedown", "window",        WNDforwardEv);
  WNDlistener("mousedown", "moveXY",        WNDmoveXY, WNDdragToDock);
  WNDlistener("mousedown", "titleDockAll",  WNDdockAllWindows);
}

function WNDresize(ParamArray){ for(let i of ParamArray){ WNDlistener("mousedown","resize"+ i, window["WNDresize"+ i], WNDnull); } }

function WNDlistener(action, target, callback, callback2 = ""){
  document.querySelectorAll('.WND' + target).forEach(element => {
    element.removeEventListener(action, element.listener);
    element.listener = function(event) {
      if(action == "mousedown" && event.button != 0) { return; }
      if(callback2){ WNDhandler(callback, event, callback2) }
      else         { callback(event); }
    };
    element.addEventListener(action, element.listener);
  });
}

function WNDhandler(callback, e, callback2, elmnt = ""){
  if(!elmnt){ elmnt = e.currentTarget.parentNode; }
   let pos3 = e.clientX, pos4 = e.clientY;
   const pos3b = e.clientX, pos4b = e.clientY;
   e = e || window.event;
  e.preventDefault();
  document.onmouseup = function (e) { callback2(e, elmnt, pos3b, pos4b); WNDescape();};
  document.onmousemove =  function (e) {
    if(WNDoffCanvas(e)) return;
    callback(e, elmnt, pos3, pos4);
    pos3 = e.clientX; pos4 = e.clientY;
  };
}

function WNDoffCanvas(e){ return !WNDitemBelowCursor(e,"TRANSfullScreen").length && !WNDitemBelowCursor(e,"WNDdock").length; }

function WNDcloseWinEvent(event){ WNDcloseWin(WNDeventId(event)); }

function WNDcloseWin(id){
  if (WNDconfigs[id] === undefined) return;
  WNDdestroyContent(id);
  ANNclearFromLocalStorageOnClose(id);
  document.getElementById(id).remove();
  delete WNDconfigs[id];
  MODELupdate('TRANSmodel');
  if(id !== "TGITterminal" ) setTimeout(TRANSsetCode, 200);
}

function WNDdockWinBottom(event){ WNDaddToDock(WNDeventId(event),"WNDdockBottom"); }
function WNDdockWinRight(event) { WNDaddToDock(WNDeventId(event),"WNDdockRight");  }

function WNDmoveXY(e, elmnt, pos3, pos4){ WNDmoveX(e, elmnt, pos3, pos4); WNDmoveY(e, elmnt, pos3, pos4); WNDcontrolPhantoms(e);}

function WNDmoveX(e, elmnt, pos3, pos4){ elmnt.style.left = ( (elmnt.offsetLeft - pos3 + e.clientX) > 0 ? (elmnt.offsetLeft - pos3 + e.clientX) : 0) + "px"; }
function WNDmoveY(e, elmnt, pos3, pos4){ elmnt.style.top  = ( (elmnt.offsetTop  - pos4 + e.clientY) > 0 ? (elmnt.offsetTop  - pos4 + e.clientY) : 0) + "px"; }

function WNDresizeES(e, elmnt, pos3, pos4){ WNDresizeE(e, elmnt, pos3, pos4); WNDresizeS(e, elmnt, pos3, pos4); }
function WNDresizeEN(e, elmnt, pos3, pos4){ WNDresizeE(e, elmnt, pos3, pos4); WNDresizeN(e, elmnt, pos3, pos4); }
function WNDresizeWS(e, elmnt, pos3, pos4){ WNDresizeW(e, elmnt, pos3, pos4); WNDresizeS(e, elmnt, pos3, pos4); }
function WNDresizeWN(e, elmnt, pos3, pos4){ WNDresizeW(e, elmnt, pos3, pos4); WNDresizeN(e, elmnt, pos3, pos4); }

function WNDresizeE(e, elmnt, pos3, pos4){ elmnt.style.width  = parseInt(elmnt.style.width, 10) - pos3 + e.clientX + "px"; }
function WNDresizeS(e, elmnt, pos3, pos4){ elmnt.style.height = parseInt(elmnt.style.height,10) - pos4 + e.clientY + "px"; }

function WNDresizeW(e, elmnt, pos3, pos4){
  elmnt.style.width  = parseInt(elmnt.style.width, 10) + pos3 - e.clientX + "px";
  if(parseInt(elmnt.style.width, 10) > 301) WNDmoveX(e, elmnt, pos3, pos4);
}
function WNDresizeN(e, elmnt, pos3, pos4){
  elmnt.style.height = (parseInt(elmnt.style.height,10) + pos4 - e.clientY) + "px";
  if(parseInt(elmnt.style.height,10) > 201) WNDmoveY(e, elmnt, pos3, pos4);
}

function WNDforwardEv(event){ WNDforward(event.currentTarget.id);  }

function WNDforward(id){
  const elem = document.getElementById(id);
  if(!elem) return;
  if(elem.style.zIndex && elem.style.zIndex == WNDmaxZ){ return; }
  elem.style.zIndex = ++WNDmaxZ;
}

function WNDhighlight(id){
  document.querySelectorAll('.WNDwindow').forEach(el => el.classList.remove('WNDhighlight'));
  window.setTimeout(function () {
    document.getElementById(id)?.classList.add('WNDhighlight');
    window.setTimeout(function () { document.getElementById(id)?.classList.remove('WNDhighlight'); }, 1000);
  }, 1);
}

function WNDescape(){
  document.onmouseup = null;
  document.onmousemove = null;
  document.querySelectorAll('.WNDdockPhantom').forEach(el => el.style.display = 'none');
}

function WNDnull(){ }
