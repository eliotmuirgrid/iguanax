function TRANSonCursorActivity(Editor, Event) {
   ITScursorActivity();
   TRANSdefaultOnCursorMove();
}

let TRANSonChangeTimeout   

function TRANSonChangeCallback(Event){
   COL_TRC("On change happening.");
   ITSonChange();
   TRANSdefaultOnChange(Event);
}

function TRANSonChange(Editor, Event) {
   COL_TRC("On change called, setting timeout to prevent double firing on enter");
   clearTimeout(TRANSonChangeTimeout);
   TRANSonChangeTimeout = setTimeout(function () {
      TRANSonChangeCallback(Event);
   }, 30);
}

function TRANSonKeyDown(Editor, Event) {
   LFDonKeyDown(Editor, Event, true);
   ITSonEditorKeyDown(Event);
   SRCHcheckForCommandF(Event);
   LINESlisteners(Event);
   TRANShandleSaveCombo(Event);
}

function TRANSonEditorFocus() {
   TRANSmarkCurrentLine();
   TRANSifware.focusLastFocussedElement = function() { TRANSeditor.focus;};
}

function TRANSonBlur() {
   if(TRANScurrentDirtyLine) TRANSfileCurrentSave();
}

function TRANSclearBrowserEvents(){
   TRANSeditor.off("cursorActivity", TRANSonCursorActivity);
   TRANSeditor.off("scroll",         ITSclosePopup);
   TRANSeditor.off("change",         TRANSonChange);
   TRANSeditor.off("keydown",        TRANSonKeyDown);
   TRANSeditor.off("keypress",       TRANSonKeyDown);
   TRANSeditor.off("focus",          TRANSonEditorFocus);
   TRANSeditor.off("blur",           TRANSonBlur);
}

function TRANSattachBrowserEvents() {
   TRANSclearBrowserEvents();
   TRANSeditor.on("cursorActivity", TRANSonCursorActivity);
   TRANSeditor.on("scroll",         ITSclosePopup);
   TRANSeditor.on("change",         TRANSonChange);
   TRANSeditor.on("keydown",        TRANSonKeyDown);
   TRANSeditor.on("keypress",       TRANSonKeyDown);
   TRANSeditor.on("focus",          TRANSonEditorFocus);
   TRANSeditor.on("blur",           TRANSonBlur);
}
