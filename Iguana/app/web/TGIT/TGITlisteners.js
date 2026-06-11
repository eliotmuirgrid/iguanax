function TGITsetupListeners() {
   let div = document.querySelector('.TGITgitWindow');
   div.addEventListener('click',   TGITclick);
   div.addEventListener('change',  TGITchange);
   div.addEventListener('keydown', TGITkeydown);
   div.addEventListener('keyup',   TGITkeyup);
   SPLTsetupSplitter('.TGITdiffView', '.TGITresizeControl', '.TGITselectFileToCommit', 'TGITratio');
}

function TGITclick(e){
   if(e.target.closest('.TGITcloseCommitModal')) return TGITremoveCommitWindow();
   if(e.target.closest('.TGITfileSelectRow'))    return TGITresolveFileDiff(e.target.closest('.TGITfileSelectRow'));
   if(e.target.closest('.TGITmodalCommit') && TGITenableCommit()) return TGITexecuteCommit();
   if(e.target.closest('.TGITunselectAll')) return TGITunselectAll();
}

function TGITchange(e){
   if(e.target.closest('.TGITselectFileToCommit')) return TGITenableCommit();
}

function TGITkeydown(e){
   if(e.target.closest('.TGITcommitMsg') && e.key == 'Enter' && !e.shiftKey) {
      e.preventDefault();
      if(TGITenableCommit()) TGITexecuteCommit();
   }
}

function TGITkeyup(e){
   if(e.target.closest('.TGITcommitMsg')) return TGITenableCommit();
}