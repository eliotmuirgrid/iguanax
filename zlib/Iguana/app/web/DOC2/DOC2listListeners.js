let DOCcheckboxActive = [];
let DOCstartDateTimeInFocus;

function DOC2listListners() {
  let Container = document.querySelector('.DOC2tree');
  if (!Container) return;
  Container.querySelector   (".DOC2list").addEventListener('click', DOC2listClick);
  Container.querySelector   (".DOC2listRowHeader").addEventListener('click', DOC2headerClick);
  Container.addEventListener('mousedown', DOCmouseDownHandler);
  Container.addEventListener('mouseup', DOCmouseUpHandler);
  Container.querySelector(".DOC2bulkEditTag").addEventListener    ('click', DOC2bulkEditTags);
  Container.querySelector(".DOC2bulkDelete").addEventListener     ('click', DOC2bulkDelete);

  let Scrollbar = document.querySelector(".DOC2scrollbar");
  Scrollbar.addEventListener('mousemove', DOCscrollbarMouseMove);
  Scrollbar.addEventListener('mouseleave', DOCscrollbarMouseLeave);
  Scrollbar.querySelector   (".DOC2loadNewest").addEventListener("click", DOCloadNewestDocsClicked);
  Scrollbar.querySelector   (".DOC2loadOldest").addEventListener("click", DOCloadOldestDocsClicked);
  Scrollbar.querySelector   (".DOC2scrollbarBox").addEventListener("click", DOCscrollToPositionClicked);

}

function DOC2bulkButtonControl(){
  let BulkButtons = document.querySelectorAll(".DOC2bulkButton");
  if (DOCcheckboxActive.length > 0){
    BulkButtons.forEach(button => {
      button.classList.remove("DOC2buttonDisabled");
    });
    return;
  }
  BulkButtons.forEach(button => {
    button.classList.add("DOC2buttonDisabled");
  });
}

function DOC2headerClick(E){
  const Target = E.target;
  if (Target.closest('.DOC2listRowHeaderCheckBox'))    { DOC2headercheckBoxClicked(E); return; };
}

function DOC2listClick(E){
  const Target = E.target;
  const Row = Target.closest('.DOC2listRow');
  if (Target.closest('.DOC2listRowCheckBox'))          { DOC2checkBoxClicked(E,Row); return; };
  if (Target.closest('.DOC2listRowTitle'))             { DOC2titleClicked(E,Row); return; };
}

function DOC2headercheckBoxClicked(E) {
   const Checked = E.target.checked;
   const List = document.querySelector(".DOC2list");
   const RowChecks = List.querySelectorAll(".DOC2listRowCheckBox input[type='checkbox']");
 
   DOCcheckboxActive = [];
 
   RowChecks.forEach((cb, Idx) => {
     cb.checked = Checked;
     if (Checked) {
       const RowData = DOCshowListData[Idx];
       DOCcheckboxActive.push(RowData);
     }
   });
   DOC2bulkButtonControl();
 }

function DOC2checkBoxClicked(E, Row) {
   const Idx = DOC2listRowIndex(Row);
   const Doc = DOCshowListData[Idx];
   const ExistingIndex = DOCcheckboxActive.findIndex(
     item => item.id === Doc.id
   );
   if (ExistingIndex === -1) {
     DOCcheckboxActive.push(Doc);
   } else {
     DOCcheckboxActive.splice(ExistingIndex, 1);
   }
   DOC2bulkButtonControl();
 }

function DOC2listRowIndex(Row) {
   const AllRows = Array.from(Row.parentElement.querySelectorAll('.DOC2listRow'));
   return AllRows.indexOf(Row);
}

function DOC2titleClicked(E,Row){
  let ExistingSelect = document.querySelector(".DOC2listRowSelected");
   if(ExistingSelect) ExistingSelect.classList.remove("DOC2listRowSelected");
   Row.classList.add("DOC2listRowSelected");
   const Idx = DOC2listRowIndex(Row);
   const RowData = DOCshowListData[Idx]
   DOCscrollbarLineSelectedPosition = parseInt(RowData.created, 16);
   MODELupdate('DOCscrollbarModel');
   DOC2documentModelRefresh();
}

function DOCupdateTooltipPosition(mouseY){
  let Div = document.querySelector(".DOC2scrollbarPopout");
  if(!Div) return;
  let Popout = Div.closest(".POPtooltip");
  if(!Popout) return;
  Popout.style.top = mouseY + 125 + "px";
}

function DOCcalculateTime(scrollPosition) {
  let Scrollbar = document.querySelector(".DOC2scrollbarBox");
  let Height = Scrollbar.offsetHeight;
  const startTime = DOCscrollbarTopIndex;
  const endTime = DOCscrollbarBottomIndex;
  const totalHeight = Height;
  const time = Math.floor(startTime + ((scrollPosition / totalHeight) * (endTime - startTime)));
  return time;
}

function DOCscrollbarMouseMove(E){
  let scrollContainer = document.querySelector(".DOC2scrollbarBox");
  let scrollContainerPopout = document.querySelector(".DOC2scrollbarPopout");
  let scrollContainerTooltip = document.querySelector(".POPtooltip");
  let scrollLine =  document.querySelector(".DOC2scrollbarHoverLine");
  const mouseY = E.clientY - scrollContainer.getBoundingClientRect().top;
  if (mouseY >= 0 && mouseY <= scrollContainer.clientHeight) {
     DOCupdateTooltipPosition(mouseY);
     const time = DOCcalculateTime(mouseY);
     if(!scrollContainerTooltip) return;
     if(isNaN(time)) return scrollContainerTooltip.classList.add("DOC2hidden");
     if(scrollContainerTooltip) scrollContainerTooltip.classList.remove("DOC2hidden");
     if(scrollContainerPopout) scrollContainerPopout.innerHTML = DOC2convertDate(time.toString(16));
     scrollLine.classList.remove("DOC2hidden");
     DOCupdateLinePosition(scrollLine, time, 0, false);
  }
}

function DOCscrollbarMouseLeave(E){
  const hoverLine = document.querySelector('.DOC2scrollbarHoverLine');
  hoverLine.classList.add("DOC2hidden");
}

function DOCmouseDownHandler(e){
  if(e.target.closest('.DOC2loadNewer')) return DOCstartScrollingNewer();
  if(e.target.closest('.DOC2loadOlder')) return DOCstartScrollingOlder();
}

function DOCmouseUpHandler(){
  clearInterval(DOCscrollNewerInterval);
  clearInterval(DOCscrollOlderInterval);
}

let DOCscrollNewerInterval;
let DOCscrollOlderInterval;
let DOCscrollPageNewerInterval;
let DOCscrollPageOlderInterval;

function DOCstartScrollingNewer(){
  DOCloadNewerDocsClicked();
  DOCscrollNewerInterval = setInterval(DOCloadNewerDocsClicked, 200);
  DOCdocumentModelInit();
}

function DOCstartScrollingOlder(){
  DOCloadOlderLogsClicked();
  DOCscrollOlderInterval = setInterval(DOCloadOlderLogsClicked, 200);
  DOCdocumentModelInit();
}

function DOCcurrentSelectedIndex(){
  let SelectedRow = document.querySelector(".DOC2listRowSelected");
  return DOC2listRowIndex(SelectedRow);
}

function DOCloadNewerDocsClicked(Amount = 1){
  let SelectedIdx = DOCcurrentSelectedIndex();
  let NewIdx = SelectedIdx - Amount;
  if(NewIdx < 0) NewIdx = 0;
  let Docs = document.querySelectorAll(".DOC2listRow");
  if(!Docs) return;
  let DocItem = Docs[NewIdx];
  if(!DocItem) return;
  DOCentryClicked(DocItem);
  DocItem.scrollIntoView({behavior : "smooth", block: "center"})
}

function DOCloadOlderLogsClicked(Amount = 1){
  let SelectedIdx = DOCcurrentSelectedIndex();
  let NewIdx = SelectedIdx + Amount;
  if(NewIdx >= DOCshowListData.length ) NewIdx = DOCshowListData.length - 1;
  let Docs = document.querySelectorAll(".DOC2listRow");
  if(!Docs) return;
  let DocItem = Docs[NewIdx];
  if(!DocItem) return;
  DOCentryClicked(DocItem);
  DocItem.scrollIntoView({behavior : "auto", block: "center"})
}

function DOCentryClicked(target){
  if(!target) return;
  let Doc = target.closest(".DOC2listRow");
  DOCfocusOnDoc(Doc);
}

function DOCfocusOnDoc(Doc){
  let ExistingSelect = document.querySelector(".DOC2listRowSelected");
  if(ExistingSelect) ExistingSelect.classList.remove("DOC2listRowSelected");
  if(Doc) Doc.classList.add("DOC2listRowSelected");
  let SelectedIdx = DOC2listRowIndex(Doc);
  let Item = DOCshowListData[SelectedIdx];
  let Time = parseInt(Item?.created,16);
  if(Time) DOCscrollbarLineSelectedPosition = Time;
  DOCscrollbarPositionUpdate();
  MODELupdate("DOCscrollbarModel");
}

function DOCloadNewestDocsClicked(){
  let Args = DOCloadNewestArgs();
  let hash =  `#documents2${PAGEformatParams(Args)}`;
  let oldhash = window.location.hash;
  if(oldhash != hash) window.location.hash = hash;
}

function DOCloadOldestDocsClicked(){
  let Args = DOCloadOldestArgs();
  let hash =  `#documents2${PAGEformatParams(Args)}`;
  let oldhash = window.location.hash;
  if(oldhash != hash) window.location.hash = hash;
}

function DOCscrollToPositionClicked(E){
  let scrollContainer = document.querySelector(".DOC2scrollbarBox");
  const mouseY = E.clientY - scrollContainer.getBoundingClientRect().top;
  let time;
  if (mouseY >= 0 && mouseY <= scrollContainer.clientHeight) {
     time = Math.round(DOCcalculateTime(mouseY)) + "";

  } else{
     return;
  }
  DOCupdateTimeUrl(time);
}

function DOCupdateTimeUrl(time){
  let Args = {};
  Args.component = PAGEhashParameter("component");
  Args.seetags   = PAGEhashParameter("seetags");
  Args.search    = PAGEhashParameter("search");
  Args.tag       = PAGEhashParameter("tag");
  Args.created   = parseInt(time).toString(16);
  let hash =  `#documents2${PAGEformatParams(Args)}`;
  let oldhash = window.location.hash;
  if(oldhash != hash) window.location.hash = hash;
}