let EXPeditor;

function EXPinitEditor(){
   let EditorConfig = {
      lineNumbers    : true,
      autofocus      : true,
      indentUnit     : 3,
      tabSize        : 3,
      viewportMargin : 70
   };
   EXPeditor = CodeMirror.fromTextArea(document.querySelector('.EXPsqlEditor'), EditorConfig);
}

function EXPpopulateCode(sqlCode){
   EXPeditor.setValue(sqlCode);
}
