function LISremoveTranslatorListeners(){
   const TransScreen = document.querySelector('.TRANSfullScreen');
   if(!TransScreen) { return; }
   TransScreen.removeEventListener("click",   LIShandleTranslatorClick, true);
   TransScreen.removeEventListener("keydown", PRJhandleGlobalKeydown,   true);
   TransScreen.removeEventListener("input",   PRJhandleGlobalInput,     true);
   TransScreen.removeEventListener("change",  PRJhandleGlobalChange,    true);
   TransScreen.removeEventListener("contextmenu", PRJhandleRightClick,  true);
   TransScreen.removeEventListener("dragstart", PRJhandleDragStart,  true);
   TransScreen.removeEventListener("dragend",   PRJhandleDragEnd,    true);
   TransScreen.removeEventListener("dragenter", PRJhandleDragEnter,  true);
   TransScreen.removeEventListener("dragover",  PRJhandleDragOver,   true);
   TransScreen.removeEventListener("drop",      PRJhandleDrop,       true);
}

function LISsetTranslatorListeners() {
   LISremoveTranslatorListeners();
   const TransScreen = document.querySelector('.TRANSfullScreen');
   TransScreen.addEventListener("click",   LIShandleTranslatorClick);
   TransScreen.addEventListener("keydown", PRJhandleGlobalKeydown);
   TransScreen.addEventListener("input",   PRJhandleGlobalInput);
   TransScreen.addEventListener("change",  PRJhandleGlobalChange);
   TransScreen.addEventListener("contextmenu", PRJhandleRightClick);
   TransScreen.addEventListener("dragstart", PRJhandleDragStart);
   TransScreen.addEventListener("dragend",   PRJhandleDragEnd);
   TransScreen.addEventListener("dragenter", PRJhandleDragEnter);
   TransScreen.addEventListener("dragover",  PRJhandleDragOver);
   TransScreen.addEventListener("drop",      PRJhandleDrop);
}

function LIShandleTranslatorClick(e) {
   if ( e.target.closest(".PRJdrawers"))        PRJdrawersClickListeners(e);
   if ( e.target.closest(".PRJprojectManager")) return PRJhandleProjectPanelClick(e);
   MENUremoveTooltip();
   if ( e.target.closest(".FLDmodal") )         return FLDlisteners(e);
   if ( e.target.closest(".EDIwindow") )        return EDIlisteners(e);
   if ( e.target.closest(".TBLwindow") )        return TBLlisteners(e);
   PRJwindowClick(e); // used to stop codemirror from overriding the cursor when typing
}

// TODO - move into PRJ library.
function PRJhandleGlobalKeydown(e) {
   return TRANSkeyDownListeners(e);
}

function PRJhandleGlobalInput(e) {
   return TRANSinputListeners(e);
}

function PRJhandleGlobalChange(e) {
   if ( e.target.closest(".TRANSwindowEdit") ){ FLDchangeListeners(e); }
   if ( e.target.closest(".EDIwindow") ){ EDIchangeListeners(e); }
   if ( e.target.closest(".PRJcomponentSelect") ){ PRJcomponentSelectChange(); }
   if ( e.target.closest(".PRJnewSelect") ){ PRJnewSelectChange(e); }
}

function PRJnewSelectChange(e) {
	TRANSsaveCurrentModuleCursorPosition();
   let Target = e.target;
   let Value = Target.value;
   Target.value = "null";
   if(Value === "file")    return PRJcreateFile(e);
   if(Value === "folder")  return PRJcreateDir(e);
   if(Value === "library") return LBRrender();
   if(Value === "sample")  return SMPLaddSampleData();
}

function PRJwindowClick(e) {
   let target = e.target;
   let Parent = target.closest(".CodeMirror-scroll");
}