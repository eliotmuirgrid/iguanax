let COMpreviewIdx;
let COMpreviewIdRendered;
let COMpreviewSticky = false;
let COMdiffData;
let COMfileList;
let COMcommitDiffData;
let COMrequest;

function COMsetListeners(div){
   if(!div) return;
   div.addEventListener("click",  COMclick);
   div.addEventListener("change", COMchange);
}

function COMcommitListMouseover(e){
   e.stopPropagation();
   if(e.target.closest('.COMcommitRow')) { return COMcommitListPreview(e.target); }
}

function COMclick(e){
   if(e.target.closest('.HSVdiffToolbarToggle') || e.target.closest('.HSVtoolbarFilename')) return COMhideShowFile(e.target);
   if(e.target.closest('.COMcloseButtonContainer'))  return COMclearPreviews();
   if(e.target.closest('.COMrestore'))               return COMrestore(COMgetCommitId());
}

function COMchange(e){
   if(e.target.closest('.HSVdiffViewSelect')) return COMdiffViewSelectChange(e.target.closest('.HSVdiffViewSelect'), COMcommitDiffData);
}

function COMclearPreviews(){
   POPtooltipRemoveByClass('COMtooltip');
   COMremovePreviousSelected();
   COMpreviewSticky = false;
   if(COMrequest) { COMrequest.abort(); COMrequest = null; }
}

function COMcommitListPreview(target, {sticky = false} = {}){
   let row = target.closest('.COMcommitRow');
   if(!row) { return; }
   let index = COMtargetToIndex(row);
   const nextCommitId = COMcommitList[COMscreen()][index]?.hash || "";
   if(COMpreviewSticky && !sticky && nextCommitId !== COMpreviewIdRendered) { return; }
   COMpreviewIdx = index;
   if(sticky) {
      const hasTooltip = !!document.querySelector('.COMtooltip');
      if(COMgetCommitId() == COMpreviewIdRendered && hasTooltip) {
         COMpreviewSticky = true;
         return;
      }
      COMclearPreviews();
      COMrenderPreview(row);
      COMpreviewSticky = true;
      return;
   }
   if(COMgetCommitId() == COMpreviewIdRendered) { return; }
   COMclearPreviews();
   COMrenderPreview(row);
}

function COMremovePreviousSelected(){
   let prevSelected = document.querySelector('.COMpreviewSelected');
   if(prevSelected){ prevSelected.classList.remove('COMpreviewSelected'); }
   COMpreviewIdRendered = null;
}

function COMspinner(){ return '<div class="CARDloading-spinner"></div>'; }

function COMrenderPreview(row){
   COMpreviewIdRendered = COMgetCommitId();
   row.classList.add('COMpreviewSelected');
   POPtooltip({
      target: row,
      content: COMspinner(),
      position: POPtooltipPosition.RIGHT,
      event: POPtooltipEvent.CLICK,
      style: "COMtooltip",
      callback: function() {
         COMgetCommitDiffs();
      }
   });
}

function COMgetCommitId(){ return COMcommitList[COMscreen()][COMpreviewIdx]?.hash || ""; }

function COMgetRelPath() { return COMcommitList[COMscreen()][COMpreviewIdx]?.path || ""; }

function COMgetCommitDiffs(){
   let data = COMdiffArgs();
   COMrequest = APIcall(COMdiffApi(), data, function(R){
      COMcommitDiffData = R?.data || {};
      COMrenderDiffPopout();
   });
}

function COMdiffArgs(){
   let data = {};
   data.commit = COMgetCommitId() || "";
   data.relative_path = COMgetRelPath();
   if(COMonDashboard()) return data;
   data.component = TRANScomponentGuid();
   data.development = TRANSdevelopment();
   return data;
}

function COMonDashboard(){
   return COMscreen() == "dashboard";
}

function COMdiffApi(){
   return COMonDashboard() ? "instance/commit_diffs" : "component/commit_diffs";
}

function COMfileListArg(){
   let Arr = [];
   for(file in COMfileList)Arr.push(COMfileList[file].name);
   return Arr;
}

function COMrenderDiffPopout(){
   const tooltipElement = document.querySelector('.COMtooltip');
   if (!tooltipElement) return;
   tooltipElement.innerHTML = COMcloseButton() + `<div class="COMtooltipContent">${COMtooltipHtml()}</div>`;
   COMsetListeners(tooltipElement);
}

function COMtooltipHtml(){
   return /*html*/`
   ${COMdiffHtml(COMcommitDiffData, COMcommitList[COMscreen()][COMpreviewIdx])}`;
}

function COMtargetToIndex(target){
   let row = target.closest('.COMcommitRow');
   const parentElement = row.closest(".COMlist");
   const logEntries = Array.from(parentElement.querySelectorAll('.COMcommitRow'));
   const Index = logEntries.indexOf(row);
   return Index;
}

function COMcommitListMouseOut(e){
   POPtooltipRemoveByClass('CLDmodal');
}

function COMdiffViewSelectChange(selectBox, Data){
   const Index = HSVtargetToItemData(selectBox);
   let View = selectBox.value;
   let Div = selectBox.closest('.HSVdiffWrapper');
   STORElocalCreate('COMviewSelected', View);
   COMupdateView(Div, Index, View, Data);
}

function COMhideShowFile(target){
   let Div = target.closest('.HSVdiffWrapper');
   // let div = Div.querySelector(".HSVdiffToolbarToggle");
   if(Div.classList.contains("HSVhide")) Div.classList.remove("HSVhide");
   else Div.classList.add("HSVhide");
}