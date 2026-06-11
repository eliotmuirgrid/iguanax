let RSBeditor;

function RSBeditorInit(){
   RSBsetCodeEditor();
   RSBsetEditorListeners();
   RSBsetContent();
}

function RSBsetCodeEditor(){
   let EditorConfig = {
      lineNumbers    : true,
      lineWrapping   : true,
      autofocus      : true,
      indentUnit     : 3,
      specialChars: LFDspecialChars(),
      specialCharPlaceholder: LFDspecialCharPlaceholder,
      tabSize        : 3,
      viewportMargin : 20
   };
   let TextArea = document.querySelector('.RSBmessage');
   RSBeditor = CodeMirror.fromTextArea(TextArea, EditorConfig);
}

function RSBsetEditorListeners(){
   RSBset("keydown",  SMPLonKeyDown);
   RSBset("keypress", SMPLonKeyDown);
   RSBset("cursorActivity", SMPLonCursorAct);
   let div = document.querySelector(".RSBbuttons");
   div.removeEventListener("click", LFDclick);
   div.addEventListener   ("click", LFDclick);
   div.removeEventListener("change", LFDchange);
   div.addEventListener   ("change", LFDchange);
}

function RSBsetContent(){
   let Data = DETrawData()?.content || "  "; 
   RSBeditor.setValue(Data);
   let Type = SMPLautoDetectFileType(Data);
   RSBeditor.setOption("mode", Type);
   RSBsetFocus();
}

function RSBset(action, func){
   RSBeditor.off(action, func);
   RSBeditor.on(action, func);
}


function RSBsetFocus(){
   const doc = RSBeditor.getDoc();
   const lastLine = doc.lineCount() - 1;
   const lastChar = doc.getLine(lastLine).length;
   doc.setCursor({ line: lastLine, ch: lastChar });
   RSBeditor.focus(); // optional: ensure focus
}