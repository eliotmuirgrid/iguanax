function DASHnewListeners(){
   let div = document.querySelector(".DASHdashboardGrid");
   // div.removeEventListener("change", DASHchangeEvent);
   // div.addEventListener   ("change", DASHchangeEvent);
   div.addEventListener('click',    DASHnewClick);
   div.addEventListener('dblclick', DASHnewDblClick);
   div.addEventListener('mouseover', function(e) { COMcommitListMouseover(e); });
   // div.addEventListener('mouseout', function(e) { COMcommitListMouseOut(e); });
}

function DASHnewClick(e){
   if(e.target.closest('.COMcommitRow')) { return COMcommitListPreview(e.target, {sticky: true}); }
   if(e.target.closest('.DASHaddComponent')) DASHaddComponent(e);
}  

function DASHnewDblClick(e){
   if(e.target.closest('.COMcommitRow')) {
      let div = document.querySelector('.COMtooltip');
      if(div) { div.innerHTML = COMspinner(); }
      let idx = COMtargetToIndex(e.target.closest('.COMcommitRow'));
      return COMpopout(idx);
   }
}

