function GITTclickListeners(e){
   if(e.target.closest('.GITTdrawerHeader')) return GITTtoggleDrawer(e);
   if(e.target.closest('.GITTactionCommit')) return TGITshowGitCommitModal();
   if(e.target.closest('.GITTactionRepo')) return CPRrender();
   if(e.target.closest('.GITTactionPull')) return TGITgitPull(e);
   if(e.target.closest('.GITTactionPush')) return TGITgitPush();
   if(e.target.closest('.GITTactionLibraryUsage')) return LBUrender();
   if(e.target.closest('.GITTactionTerminal')) return TRGTtoggleGitTerminal(e);
   if(e.target.closest('.GITTactionIguanaXLuaAssistant')) return GITTactionIguanaXLuaAssistant(e);
   if(e.target.closest('.GITTactionCopyUrl')) return GITTcopyUrl(e);
}

function GITTtoggleDrawer(e){
   let div = document.querySelector('.GITTdrawer');
   div.classList.toggle('is-open');
}

function GITTcopyUrl(e){
   let url = document.querySelector('.GITTactionCopyUrl').href;
   navigator.clipboard.writeText(url);
   let tooltip = POPtooltip({
      target: e.target,
      content: 'Copied to clipboard',
      position: POPtooltipPosition.BOTTOM,
   });
}

function GITTactionIguanaXLuaAssistant(e){
   if(e.target.closest('.GITTchatGptLink')) return;
   if(e.target.closest('.TRANSaiButton')) return;
   let url = document.querySelector('.GITTchatGptLink').href;
   window.open(url, '_blank');
}