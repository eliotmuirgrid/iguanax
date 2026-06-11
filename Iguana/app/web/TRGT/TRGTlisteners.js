function TRGTsetListeners(){
   COL_TRC('TRGTsetListeners');
   let Modal = document.querySelector('.TRGTmodal');
   if(!Modal) return;
   COL_TRC('Setting up git actions');
   Modal.removeEventListener("click", TRGTtoolbarGitClick);
   Modal.addEventListener   ("click", TRGTtoolbarGitClick);
   CLDupdate(false);   
   // BRCHupdate();
}

function TRGTtoggleGitTerminal(e){
   // if (e.target.closest('.on')) {
   //    COL_TRC('Hide GIT terminal');
   //    if(WNDisWindow('TGITterminal')) WNDcloseWin('TGITterminal');
   //    let dockedTerminal = document.getElementById('TGITterminaldocked');
   //    if (dockedTerminal) WNDcloseTab('TGITterminaldocked', dockedTerminal.closest('.WNDdock').id);
   //    TRANSsetCode();
   // } else {
      COL_TRC('Show GIT terminal');
      TGITopenBranchTerminal();
   // }
}

function TRGTtoolbarGitClick(e){
   COL_TRC("GitClick", e);
   // if( e.target.closest('.CLDeditButton') )      CPRrender();
   if( e.target.closest('.TRGTrepoButton') )      CLDrender();
   if(TRANSreadOnly() || e.target.closest('.disabled')) return;
   if( e.target.closest('.TRGTcommit') )          TGITshowGitCommitModal();
   if( e.target.closest('.TRGTgitPushRemote') )   TGITgitPush();
   if( e.target.closest('.TRGTgitPullRemote') )   TGITgitPull();
   if( e.target.closest('.TRGTgitTerminalButton') ){ TRGTtoggleGitTerminal(e); }
   if( e.target.closest('.TRGTlibraryUsage') )  { LBUrender(); }
}