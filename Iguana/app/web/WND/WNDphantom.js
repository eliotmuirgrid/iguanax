function WNDdragToDock(e,elmnt,pos3,pos4){
   if(((e.clientX - pos3) == 0) && ((e.clientY - pos4) == 0 )){ return; }
   WNDupdatePhantomDimensions();
   const pos = WNDtabNumber(WNDidBelowCursor(e, 'WNDtab'), WNDinWhichDock(e));
   if( WNDinWhichDock(e) || WNDdockShowing()) { WNDaddToDock(elmnt.id, WNDdockShowing(),true, pos); }
}
// These native function that gets the height of an element even if it is hidden   
// It used to be that jQuery could get the height of a hidden element, but no longer using jQuery
function WNDgetHiddenElementHeight(E) {
   COL_TRC("originalDisplay=", E.style.display);
   if (E.style.display != 'none') {
      COL_TRC("height=", E.offsetHeight);
      return E.offsetHeight;
   }
   COL_TRC("Hidden so we temporarily make it visible");
   const originalDisplay = E.style.display;
   // Temporarily set display to a visible state to measure height
   E.style.display = 'block';
   const height = E.offsetHeight;
   // Revert to the original display style
   E.style.display = originalDisplay;
   COL_TRC("height=", height);
   return height;
}

function WNDgetHiddenElementWidth(E) {
   COL_TRC("originalDisplay=", E.style.display);
   if (E.style.display != 'none') {
      COL_TRC("width=", E.offsetWidth);
      return E.offsetWidth;
   }
   COL_TRC("Hidden so we temporarily make it visible");
   const originalDisplay = E.style.display;
   // Temporarily set display to a visible state to measure width
   E.style.display = 'block';
   const width = E.offsetWidth;
   // Revert to the original display style
   E.style.display = originalDisplay;
   COL_TRC("width=", width);
   return width;
}

function WNDinWhichDock(e){ 
   if(e.clientY > (document.body.offsetHeight - WNDgetHiddenElementHeight(document.getElementById('WNDdockBottomPhantom')))){
      COL_TRC("WNDdockBottom");
      return "WNDdockBottom";
   }
   if(e.clientX > (document.body.offsetWidth  - (WNDgetHiddenElementWidth(document.getElementById('WNDdockRightPhantom'))/2))){
      COL_TRC("WNDdockRight");
      return "WNDdockRight";
   }
   COL_TRC("No dock found");
   return;
}

function WNDdockShowing(){
   let bottomPhantom = document.getElementById('WNDdockBottomPhantom');
   let rightPhantom = document.getElementById('WNDdockRightPhantom');
   if(window.getComputedStyle(bottomPhantom).display !== 'none') return "WNDdockBottom";
   if(window.getComputedStyle(rightPhantom) .display !== 'none') return "WNDdockRight";
   return "";
}

function WNDcontrolPhantoms(e){
   let DockPhantoms = document.querySelectorAll('.WNDdockPhantom');
   DockPhantoms.forEach(phantom => phantom.style.display = 'none');
   WNDupdatePhantomDimensions();
   let Id = WNDinWhichDock(e)+"Phantom";
   let Phantom = document.getElementById(Id);
   if(Phantom) Phantom.style.display = 'block';
}

function WNDupdatePhantomDimensions(){
   let BottomPhantom = document.getElementById('WNDdockBottomPhantom');
   let RightPhantom = document.getElementById('WNDdockRightPhantom');
   let BottomDock = document.getElementById('WNDdockBottom');
   let RightDock = document.getElementById('WNDdockRight');
   let TRANSwindowEdit = document.querySelector('.TRANSwindowEdit');
   BottomPhantom.style.height = (BottomDock ? BottomDock.offsetHeight : WNDdockHeightDefault() ) + 'px';
   RightPhantom.style.width   = (RightDock  ? RightDock.offsetWidth  : WNDdockWidthDefault() ) + 'px';
   BottomPhantom.style.width  = (RightDock  ? TRANSwindowEdit.offsetWidth - RightDock.offsetWidth : TRANSwindowEdit.offsetWidth) + 'px';
}

function WNDdockHeightDefault(){ 
   let TRANSmainContainer = document.querySelector('.TRANSmainContainer');
   let height = TRANSmainContainer ? TRANSmainContainer.offsetHeight*.33 : 240;
   return ( height > 240 ? height : 240); 
}

function WNDdockWidthDefault(){  
   let body = document.querySelector('body');
   let width = body ? body.offsetWidth*.33 : 300;
   return ( width > 300 ? width : 300); 
}
