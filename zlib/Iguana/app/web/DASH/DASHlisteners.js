let DASHminListWidth = 508;

function DASHsetListenersInit(){
   DASHnewListeners();
   DASHsetComponentListListeners();
   DASHstaticListeners();
   DASHsetListeners();
   DASHsetHistoryListeners();
   DASHcollapseState();
}

function DASHaddComponent(e){
   if(e.target.closest(".BUTTONdisabled")) return;
   let div = document.querySelector('.DASHeditConnections');
   if(div) div.checked = false;
   DASHsaveArgs();
   let Url = "#dashboard/new" + PAGEformatParams(DASHpreviousFilter);
   window.location.hash = Url;
}

function DASHaddComponentFromUrl(e){
   if(e.target.closest(".BUTTONdisabled")) return;
   let div = document.querySelector('.DASHeditConnections');
   if(div) div.checked = false;
   DASHsaveArgs();
   let Url = "#dashboard/new/url" + PAGEformatParams(DASHpreviousFilter);
   window.location.hash = Url;
}

function DASHcollapsed(){ 
   return document.querySelector('.SPLTcollapse .DASHresizeControl2') !== null;
}

function DASHcollapseState(){
   let State = STORElocalRead('gitHistoryCollapsed');
   State === 'expanded' ? DASHexpandedView() : DASHcollapsedView();
}

function DASHcollapseButton(){
   document.querySelector('.DASHdashboard .SPLTcollapse') 
   ? (DASHexpandedView(),  STORElocalCreate('gitHistoryCollapsed', 'expanded')) 
   : (DASHcollapsedView(), STORElocalCreate('gitHistoryCollapsed', 'collapsed'));
   MODELforcePoll("COMmodel");
   requestAnimationFrame(() => { CYTOresize(); });
}

function DASHexpandedView() {
   document.querySelector('.DASHdashboard .SPLTcollapse')?.classList.remove('SPLTcollapse');
}

function DASHcollapsedView(){ 
   document.querySelector('.DASHgitHistorySplitter')?.classList.add("SPLTcollapse"); 
}

function DASHsetListeners(){
   let div = document.querySelector(".DASHdashboardGrid");
   div.removeEventListener("click", DASHhandleClick);
   div.addEventListener("click", DASHhandleClick);
}

function DASHhandleClick(e){
   if(e.target.closest(".DASHfilterCheckbox"))        return DASHcheckboxClick(e.target);
   if(e.target.closest(".DASHaddComponentButton"))    return DASHaddComponent(e);
   if(e.target.closest(".DASHaddFromUrlButton"))      return DASHaddComponentFromUrl(e);
   
}
function DASHsetHistoryListeners(){
   let div = document.querySelector(".DASHgitHistorySplitter");
   div.removeEventListener("click", DASHhandleGitHistoryClick);
   div.addEventListener("click", DASHhandleGitHistoryClick);
}

function DASHhandleGitHistoryClick(e){
   if(e.target.closest(".SPLTresizeButtonContainer")) return DASHcollapseButton();
}


function DASHmatchWholeWord(){
   let Div = document.querySelector('.DASHfilterResultsCheckbox');
   if(!Div) return;
   return Div.checked;
}

function DASHcheckboxClick(target){
   let inp = target.closest("input");
   if(!inp) {
      inp = target.querySelector("input");
      if(!inp) return;
      if(inp.checked) inp.checked = false;
      else            inp.checked = true;
   }
   if(!inp) return;
   let MatchWholeWord = DASHmatchWholeWord();
   PAGEsetHashParameter("whole_word", MatchWholeWord);
}

function DASHonlySelectedChanged() {
   MODELupdate('DASHmodel', 'onlySelectedToggle');
   MODELforcePoll('DASHmodel');
}

function DASHstaticListeners(){
   document.querySelector('.DASHselectMenu') .addEventListener('change', DASHbulkActions);
   document.querySelector('.DASHonlyChecked').addEventListener('change', DASHonlySelectedChanged);
   SPLTsetupSplitter('.CORmain', '.DASHresizeControl', '.DASHlistView', 'DASHratio', DASHminListWidth, undefined, DASHupdateNameWidth)
   DASHupdateNameWidth();
   SPLTsetupSplitter('.CORmain', '.DASHresizeControl2', '.DASHgitHistorySplitter', 'DASHratio2', undefined, COMclearPreviews)
   document.querySelector('.DASHclearSearch')    .addEventListener('click', DASHclearSearchText);
   document.querySelector('.EDITfilter')         .addEventListener('input', DASHsearchTextChanged);
}

function DASHupdateNameWidth(){
   let Width = document.querySelector('.DASHlistView').offsetWidth;
   let OtherWidths = document.querySelectorAll('.DASHtableHeader');
   let TotalWidth = 32;
   for(let header of OtherWidths){
      if(!header.classList.contains('DASHnameHeader')){
         let style = window.getComputedStyle(header);
         let width = parseInt(style.width);
         TotalWidth += width + 12;
      }
   }
   let NameWidth = Width - TotalWidth;
   if(NameWidth < 100) NameWidth = 100;
   let NameHeader = document.querySelectorAll('.DASHnameHeader');
   for(let header of NameHeader){
      header.style.maxWidth = NameWidth + 'px';
   }
}
