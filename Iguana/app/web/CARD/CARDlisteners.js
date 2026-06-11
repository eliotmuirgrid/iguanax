let CARDredirectToRemote = false;

function CARDlisteners(){
   let card = document.querySelector('.CARDdetailedCardId');
   card.removeEventListener('click', CARDclickListeners);
   card.removeEventListener('keydown', CARDkeydownListeners);
   card.addEventListener('click',   CARDclickListeners);
   card.addEventListener('keydown', CARDkeydownListeners);
}

function CARDclickListeners(e){
   if(e.target.closest(".CARDdisableLink") || e.target.closest(".CARDdisable")) return;
   let T = CARDgetEventDetails(e);
   CARDnameListener(T);
   CARDdescriptionListener(T);
   CARDcurrentCommitListener(T);
   CARDfieldListeners(T);
   if(T.targetArea('CARDaction'))            { CARDactionClicked(e); }
   if(T.targetArea('CARDswitchTarget'))      { CARDswitchClicked(e); }
   if(T.targetArea('CARDstatusClearError'))  { CARDclearErrorModal([CARDdata().guid]); }
   if(T.targetArea('CARDstatusClearQueue'))  { CARDclearQueueModal([CARDdata().guid]); }
   if(T.targetArea('CARDstatusRepositionQueue'))  { CARDrepositionQueueClicked(); }
   if(T.targetArea('CARDgitEditCommitButton')){ CARDenableGitCommitEdit(); }
   if(T.targetArea('CARDgitEditCurrentCommitButton')){ CARDenableCurrentCommitEdit(); }
   if(T.targetArea('CARDqueuePosition'))     { CARDredirectToLogs('message'); }
   if(T.targetArea('CARDerrorCount'))        { CARDredirectToLogs('error'); }
   if(T.targetArea('CARDlastActivityValue')) { CARDredirectToLogs(); }
  // if(T.targetArea('CARDviewDetails'))       { CARDviewDetails(e); }
   if(e.target.closest('.CARDremoveUpstream')) { MKEmakeEditablePopout(CARDmakeEditableArgs()); }
   if(e.target.closest('.CARDaddToCollection')) { CARDaddToCollectionClick(); }
   if(e.target.closest('.CARDseeInCollection')) { CARDseeInCollectionClick(); }
   if(e.target.closest('.CARDeditUpstream'))  { CARDeditUpstreamClicked(); }
   if(e.target.closest('.CARDchangeBranch'))  { CARDchangeBranchClicked(); }
   if(e.target.closest('.CARDrefreshCurrentCommit'))  { CARDrefreshCommits(); }
   if(e.target.closest('.CARDtagRow .CARDaddTags'))  { CARDeditTags(); }
   if(e.target.closest('.CARDtagRow .CARDeditBtn'))  { CARDeditTags(); }
   if(e.target.closest('.CARDtagRow .BUTTONcancel')) { CARDcancelTags(); }
   if(e.target.closest('.CARDtagRow .BUTTONaction'))  { CARDconfirmTagSave(); }
   if(e.target.closest('.CARDtagValue .CARDtagClickable')) { CARDtagClick(e.target); }
   if(e.target.closest('.CARDsourceActions .RPOturnToRepo')) { CARDturnToRepo(); }
   if(e.target.closest('.CARDshowMore'))               { CARDshowMore(); }
   if(e.target.closest('.CARDshowLess'))               { CARDshowLess(); }
   if(e.target.closest('.CARDcommitIdHelp'))          { CMTrender(); }
}

function CARDmakeEditableArgs(){
   let Args = {};
   Args.component = CARDcomponentId();
   Args.url       = CARDgitRepo();
   Args.callback  = CARDmakeEditableCallback;
   Args.message   = "To edit this component, you must make it editable.";
   return Args;
}

function CARDmakeEditableCallback(){
   MODELforcePoll('CARDmodel');
   location.hash = "translator?component=" + CARDcomponentId() + "&development=true";
}

function CARDeditUpstreamCallback(){
   if(CARDredirectToRemote) CARDeditUpstreamClicked();
   else                     MODELforcePoll('CARDmodel');
}

function CARDchangeBranchClicked(){
   let Url = "#dashboard/set_branch?component=" + CARDcomponentId();
   let Filter = PAGEhashParameter("filter");
   if(Filter) Url = Url + "&filter=" + Filter;
   window.location.hash = Url;
}

function CARDeditUpstreamClicked(){
   let Url = "#dashboard/set_repo?component=" + CARDcomponentId();
   let Filter = PAGEhashParameter("filter");
   if(Filter) Url = Url + "&filter=" + Filter;
   window.location.hash = Url;
}

function CARDrepositionQueueClicked(){
   let Url = "#dashboard/reposition_queue?component=" + CARDcomponentId();
   let Filter = PAGEhashParameter("filter");
   if(Filter) Url = Url + "&filter=" + Filter;
   window.location.hash = Url;
}

function CARDactionClicked(e){
   const Writable = CARDwritable();
   if( !Writable ) CARDmakeEditable();
}

function CARDaddToCollectionClick(){
   let Description = CARDdescription();
   let Data = {};
   Data.name        = CARDdata().name;
   Data.description = Description;
   Data.git         = CARDgitRepo();
   Data.type        = "component";
   Data.branch      = CARDgitBranch();
   ADCrender(Data);
}

function CARDseeInCollectionClick(){
   let Collection = CARDurlInTemplatesCollection();
   if(!Collection) return;
   EDCinit(Collection);
}

function CARDkeydownListeners(e){
   let T = CARDgetEventDetails(e);
   if(e.key === "Escape" ) return CARDdisableOpenEdit();
   if(e.key !== "Enter" ) return;
   if(e.target.value.trim() === '') return;
   if(T.targetArea('CARDnameEditInput')) CARDsaveName();
   if(T.targetArea('CARDdescriptionEditInput')) if( !T.shiftKey ) CARDsaveDescription();
   if(T.targetArea('CARDvalueEdit') && T.target.closest('.CARDeditInline')&& !T.shiftKey) {
      e.preventDefault();
      CARDfieldSave(e.target.closest('.CARDviewFields').querySelector('.CARDsaveInplace'), function(){
         CARDdisableOpenEdit();
         MODELforcePoll("CARDmodel");
      });
   }
}

function CARDgetEventDetails(e){
   // e.stopPropagation();
   let Details = {};
   Details.target = e.target;
   Details.shiftKey = e.shiftKey;
   Details.targetArea  = function(ClassName){ return Details.target.closest(`.${ClassName}`); }
   return Details;
}

function CARDclearQueueModal(ComponentGuidList) {
   let div = document.querySelector(".CARDactionPopup");
   if(div) return;
   POPtooltipRemoveByClass('LGEtooltipWrap');
   let Card = POPpopoutOpen({title: 'Clear Queues', content: CARDclearQueueHtml(ComponentGuidList), className: 'CARDactionPopup', esc_close : true});
   document.querySelector('.CARDcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.CARDsaveTags').addEventListener('click', function() { CARDclearQueue(ComponentGuidList); POPpopoutClose(Card);})
}

function CARDclearErrorModal(ComponentGuidList) {
   let div = document.querySelector(".CARDactionPopup");
   if(div) return;
   POPtooltipRemoveByClass('LGEtooltipWrap');
   let Card = POPpopoutOpen({title: 'Clear Errors', content: CARDclearErrorsHtml(ComponentGuidList), className: 'CARDactionPopup', esc_close : true});
   document.querySelector('.CARDcancelTags').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.CARDsaveTags').addEventListener('click', function() { CARDclearError(ComponentGuidList); POPpopoutClose(Card);})
}
