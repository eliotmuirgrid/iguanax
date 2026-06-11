let COMcard;
let COMpopoutIdx;
let COMpopoutCommitData;
let COMcommitPopDiffData;

function COMpopout(Idx){
   COMpopoutIdx = Idx;
   COMpopoutCommitData = COMcommitList[COMscreen()][Idx];
   COMcard =  POPpopoutOpen({
      title: 'Commit History', 
      height: '100%', 
      width: '90%', 
      minWidth: '800px', 
      content: COMskeletonPopoutHtml(),
      className: 'COMwindow',  
      esc_close : true
   });
   // let Row = document.querySelector(".COMwindow .COMdiffSelected");
   let Rows = document.querySelectorAll(".COMwindow .COMcommitRow");
   let Row = Rows[COMpopoutIdx];
   if(Row) Row.scrollIntoView({ block: 'center', behavior: 'auto' });
   SPLTsetupSplitter('.COMpopoutModal', '.COMresizeControl', '.COMpopoutListSplitter', 'COMratio')
   COMclearPreviews();
   COMrenderDiffs();
   COMselectRow();
   let div = document.querySelector(".COMpopoutModal");
   COMpopoutListeners(div);
}

function COMpopoutListeners(){
   let div = document.querySelector(".COMpopoutModal");
   if(!div) return;
   div.addEventListener("click", COMpopoutClick);
   div.addEventListener("change", COMpopoutChange);
}

function COMpopoutClick(e){
   if(e.target.closest(".COMcommitRow")) return COMpopoutCommitSelected(e.target);
   if(e.target.closest('.HSVdiffToolbarToggle') || e.target.closest('.HSVtoolbarFilename')) return COMhideShowFile(e.target);
   if(e.target.closest('.COMrestore')) return COMrestore(COMpopoutCommitData?.hash);
}
function COMpopoutChange(e){
   if(e.target.closest('.HSVdiffViewSelect')) return COMdiffViewSelectChange(e.target.closest('.HSVdiffViewSelect'), COMcommitPopDiffData);
}

function COMselectRow(){
   let div = document.querySelector(".COMpopoutList");
   if(!div) return;
   let selected = div.querySelector(".COMdiffSelected");
   if(selected) selected.classList.remove("COMdiffSelected");
   let rows = div.querySelectorAll(".COMcommitRow");
   let row = rows[COMpopoutIdx];
   row.classList.add("COMdiffSelected");
}

function COMrenderDiffs(){
   let div = document.querySelector(".COMpopoutDiff");
   if(!div) return;
   div.innerHTML = COMspinner();
   let data = COMpopoutArgs();
   APIcall(COMdiffApi(), data, function(R){
      COMcommitPopDiffData = R?.data || {};
      div.innerHTML = COMpopoutHtml();
   });
}

function COMrollbackHtml(){
   return "";
   if(COMscreen() != "dashboard") return "";
   let commitId = COMpopoutCommitData?.hash || "";
   if(!commitId) return "";
   let html = /*html*/`
      <div class="COMrollback">
         <div class="COMrollbackText">Would you like to rollback to this commit?</div>
         <a class="COMrollbackButton BUTTONdefault BUTTONstandard" href="#settings/instance?rollback=${commitId}">ROLLBACK</a>
      </div>
   `;
   return html;
}

function COMpopoutHtml(){
   let html = /*html*/`
      <div class="COMinnerDiv">
         ${COMdiffHtml(COMcommitPopDiffData, COMcommitList[COMscreen()][COMpopoutIdx])}
      </div>
      ${COMrollbackHtml()}
   `;
   return html;
}

function COMpopoutCommitSelected(target){
   let Idx = COMtargetToIndex(target);
   COMpopoutIdx = Idx;
   COMpopoutCommitData = COMcommitList[COMscreen()][Idx];

   COMrenderDiffs();
   COMselectRow();
}

function COMgetRelPathPopout(){
   return COMpopoutCommitData?.path || "";
}  

function COMpopoutArgs(){
   let data = {};
   data.commit = COMpopoutCommitData?.hash || "";
   data.relative_path = COMgetRelPathPopout();
   if(COMonDashboard()) return data;
   data.component = TRANScomponentGuid();
   data.development = TRANSdevelopment();
   return data;
}