let TRANSupdateAnnotationsTimer;
let TRANSmapClickTimer = 0;
let TRANSmapClicks = 0;

function TRANSsetListeners(){
   let Modal = document.querySelector('.TRANSwindow');
   if(!Modal) return;
   Modal.removeEventListener("click", TRANSclick, true);
   Modal.addEventListener   ("click", TRANSclick);
   TRNSautoStartUpdate();   
}

function TRANSdisableSwipeToPrevious(){
   let Editor = document.querySelector('#TRANSeditorDiv');
   if(!Editor) return;
   Editor.addEventListener('wheel', (event) => {
      if (event.deltaX !== 0) event.preventDefault();
   });
}

function TRANSdblClick(e){
   if(e.target.closest('.COMcommitRow')) {
      let div = document.querySelector('.COMtooltip');
      if(div) div.innerHTML = COMspinner();
      let idx = COMtargetToIndex(e.target.closest('.COMcommitRow'));
      return COMpopout(idx);
   }
}

function TRANScommitRowClicked(e){
   if(TRANSmapClicks >= 1 ){
      TRANSdblClick(e);
      clearTimeout(TRANSmapClickTimer);
      TRANSmapClicks = 0;
   }
   else{
      TRANSmapClicks++;
      TRANSmapClickTimer = setTimeout(function(){ 
         TRANSmapClicks = 0;
         TRANScommitRowSingleClick(e);
      }, 300);
   }
}

function TRANScommitRowSingleClick(e){
   return COMcommitListPreview(e.target);
}

function TRANScommitRowDoubleClick(e){
   let div = document.querySelector('.COMtooltip');
   div.innerHTML = COMspinner();
   let idx = COMtargetToIndex(e.target.closest('.COMcommitRow'));
   return COMpopout(idx);
}

function TRANSclick(e){
   if( e.target.closest(".COMcommitRow")) { return TRANScommitRowClicked(e); }
   if( e.target.closest(".TRANSpromotionReview") ) return TRANSpromotionResume();
   if( e.target.closest(".TRANSpromotionAbort") )  return TRANSpromotionAbort();
   if( e.target.closest(".TRANSpromotionPushReturn") ) return TRANSpromotionPushReturnFromBanner();
   if( e.target.closest(".TRANSpromotionDismiss") ) return TRANSpromotionDismissPendingPush();
   if(e.target.closest(".TRANSwindowEdit"))         COMclearPreviews();
   if(e.target.closest(".TRANSgitHistorySplitter")) COMclearPreviews();
   if( e.target.closest('.TRANSshowStackTrace') )  TRANSshowStackTraceClicked();
   if( e.target.closest(".TRANSresizeButtonContainer") ) TRANScollapseButton();
   if( e.target.closest(".PRJscriptSwitch .TOGbutton") ) PRJswitchClick();
   if( e.target.closest(".PRJsourcesReceiversTarget") ) PRJsourcesReceiversTargetClicked();
   if( e.target.closest('.TRANSscriptingWindow') ) TRANSsaveCurrentModuleCursorPosition();
   if( e.target.closest('.TRANSscriptingWindow') ) TRANSsaveCurrentModuleCursorPosition();
   if( e.target.closest('.TRANSautoRunButton') )   TRANSautoRunClick();
   if( e.target.closest('.TRANSstopButton') )      TRANSstopButtonClicked();
   if( e.target.closest(".ITMbannerContainer .ITMbanner2") ) TRANSlineJumpBannerClicked(e);
   if( e.target.closest('.TRANSrunButton') )       TRANSrunButtonClicked();
   if( e.target.closest('.ITMcopyContent') )       return ITMcopyContent(e);
   if( e.target.closest('.ITMnoDataErrorToggle') ) ITMnoDataErrorClicked(e);
   if( e.target.closest('.TRANStoolbarWrapper') && 
      !e.target.closest('.TRANStoolbarButton')  &&
      !e.target.closest('input')  &&
      !e.target.closest('select') ) {
      TRANSsetCode();
   }
}

function TRANScollapseButton(){
   $('.TRANSwindow .SPLTcollapse').length
   ? (TRANSexpandedView(),  STORElocalCreate('TRANSgitHistoryCollapsed', 'expanded')) 
   : (TRANScollapsedView(), STORElocalCreate('TRANSgitHistoryCollapsed', 'collapsed'));
   MODELforcePoll("TCMmodel");
   WNDupdateDockDimensions();
}

function TRANSexpandedView(){
   $('.SPLTcollapse').removeClass('SPLTcollapse');
}

function TRANScollapsedView(){
   $('.TRANSgitHistorySplitter').addClass("SPLTcollapse"); 
}

function TRANSkeyDownListeners(e){
   if(!e.target.closest(".TRANSeditorDiv")) return;
   if ((e.metaKey || e.ctrlKey) && e.key === 'v') {
      TRANSupdateAnnotationsTimer = setTimeout(function(){ TRANSunmarkSaveAndRequestAnnotations(); }, 100);
   }
}

function TRANSinputListeners(e){
   if(!e.target.closest(".TRANSeditorDiv")) return;
   if (e.inputType === 'insertFromPaste') {
      TRANSupdateAnnotationsTimer = setTimeout(function(){ TRANSunmarkSaveAndRequestAnnotations(); }, 100);
   }
}

function TRANSautoRunClick(){
   TRANSsetAnnotationsEnabled(!TRANSannotationsEnabled()); // seems complicated
   COL_TRC("Auto run play pressed");
   TRANSupdateAnnotations();
   MODELupdate('TRANSmodel');
   TRANSsetCode();
}

function TRANSshowStackTraceClicked(){
   TRANSerrorStackDialog(TRANSstackTraceError);
}

function TRANSstopButtonClicked(){
   COL_TRC('Stop button click!');
   TRANSabortAll();
   TRANSannotationStatus("Cancelled");
   TRANSannotationRequestButtons(false);
   TRANSsetCode();
}
function TRANSlineJumpBannerClicked(e){
   TRANSonFunctionCallClick(e, e.target);
}

function TRANSplayButton(Enable){
   let Button = document.querySelector('.TRANSrunButton');
   if(!Button) return;
   if(Enable){
      if(Button.classList.contains('disabled')) Button.classList.remove('disabled');
      if(Button.classList.contains('on'))       Button.classList.remove('on');
   }
   else{
      if(!Button.classList.contains('disabled')) Button.classList.add('disabled');
      if(!Button.classList.contains('on'))       Button.classList.add('on');
   }
}

function TRANSstopButton(Enable){
   let Button = document.querySelector('.TRANSstopButton');
   if(!Button) return;
   if(Enable){
      if(Button.classList.contains('disabled')) Button.classList.remove('disabled');
      if(Button.classList.contains('on'))       Button.classList.remove('on');
   }
   else{
      if(!Button.classList.contains('disabled')) Button.classList.add('disabled');
   }
}

function TRANSrunButtonClicked(){
   COL_TRC("Play button pressed");
   TRANSretrieveCodeBlockData();
   TRANSupdateAnnotations({force: true});
   TRANSsetCode();
}

function TRANSrunIfAutoRunEnabled(){
   if(!TRANSannotationsEnabled()) return;
   TRANSrunButtonClicked();
}