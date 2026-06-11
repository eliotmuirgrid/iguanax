let LFDskip = false;

function LFDspecialChars(){
   return /(\u240D\u240A|\u240D|(?<!\u240D)\u240A|[\u240A])/g;
}

function LFDspecialCharPlaceholder(ch) {
   const span = document.createElement("span");
   if(ch === LFDlf && LFDskip) {
      LFDskip = false;
      return span; 
   }
   if (ch === LFDcrlf) {
      span.textContent = "CRLF";
      span.classList.add("LFDcrlf");
      LFDskip = true;
      return span; 
   } else if (ch === LFDcr) {
     span.textContent = "CR";
     span.classList.add("LFDcr");
   } else if (ch === LFDlf) {
      span.textContent = "LF";
      span.classList.add("LFDlf");
   }
   return span;
}

function LFDsetEditorContent(editor, content){
   if(editor.getValue() != content) {
      let cursor = editor.getCursor();
      editor.setValue(content);
      editor.setCursor(cursor);
   }
}

function LFDrenderLineFeeds(editor){
   const content = editor.getValue();
   const newContent = LFDcontentWithLineFeeds(content);
   LFDsetEditorContent(editor, newContent);
}
