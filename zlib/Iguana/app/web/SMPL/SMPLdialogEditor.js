function SMPLdialogSetUpEditor() {
   let EditorConfig = {
      lineNumbers    : true,
      autofocus      : true,
      indentUnit     : 3,
      specialChars: LFDspecialChars(),
      specialCharPlaceholder: LFDspecialCharPlaceholder,
      tabSize        : 3,
      viewportMargin : 70
   };
   SMPLeditor = CodeMirror.fromTextArea(document.querySelector('.SMPLimportData'), EditorConfig);
   SMPLset("keydown",  SMPLonKeyDown);
   SMPLset("keypress", SMPLonKeyDown);
   SMPLset("cursorActivity", SMPLonCursorAct);
}

function SMPLset(action, func){
   SMPLeditor.off(action, func);
   SMPLeditor.on(action, func);
}

function SMPLonKeyDown(Editor, Event){
   LFDonKeyDown(Editor, Event, false);
}

function SMPLonCursorAct(Editor){
   LFDrenderLineFeeds(Editor);
}