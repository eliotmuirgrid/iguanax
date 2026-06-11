function LFDsetListeners(){
   let div = document.querySelector(".LFDmodal");
   if(!div) return;
   div.removeEventListener("click", LFDclick);
   div.addEventListener   ("click", LFDclick);
   div.removeEventListener("change", LFDchange);
   div.addEventListener   ("change", LFDchange);
}

function LFDclick(e){
   if(e.target.closest(".LFDshowHide")) return LFDshowHideClick();
   if(e.target.closest(".LFDconvert"))  return LFDconvertClick(true);
}

function LFDchange(e){
   if(e.target.closest(".LFDlineFeedSelect")) return LFDconvertClick(false);
}

function LFDshowHideClick(){
   let div = BLKsingleResubmit() ? document.querySelector(".RSBeditor") : document.querySelector(".TRANSwindowEdit");
   if(!div) return;
   div.classList.toggle("LFDhide");
}

function LFDconvertClick(defaultConvert) {
   const doc = BLKsingleResubmit() ? RSBeditor.getDoc() : TRANSeditor.getDoc();
   const selectionStartIdx = doc.indexFromPos(doc.getCursor('start'));
   const selectionEndIdx   = doc.indexFromPos(doc.getCursor('end'));
   const selectedText = doc.getSelection();
   if (selectedText && selectedText.length > 0) {
     const newSelectedText = LFDconvertFeeds(selectedText);
     doc.replaceRange(
       newSelectedText,
       doc.posFromIndex(selectionStartIdx),
       doc.posFromIndex(selectionEndIdx)
     );
     doc.setSelection(
       doc.posFromIndex(selectionStartIdx),
       doc.posFromIndex(selectionStartIdx + newSelectedText.length)
     );
   } else if (defaultConvert) {
     const content = doc.getValue();
     const newContent = LFDconvertFeeds(content);
     doc.setValue(newContent);
     doc.setCursor(doc.posFromIndex(selectionStartIdx));
   } else {
     doc.setCursor(doc.posFromIndex(selectionStartIdx));
   }
}

function LFDscrambledChar(){ return "LFD_scrambl_4321asf"; }

function LFDconvertFeeds(Content){
   let lineFeed = LFDselectedLineFeed();
   let scr = LFDscrambledChar();
   let newContent = Content;
   newContent = newContent.replaceAll(LFDcrlf, scr);
   newContent = newContent.replaceAll(LFDcr, scr);
   newContent = newContent.replaceAll(LFDlf, scr);
   newContent = newContent.replaceAll(scr, lineFeed);
   return newContent;
}

function LFDonKeyDown(Editor, Event, Check) {
   if (Check && !SMPLeditingSampleData()) { return; }
   const cursor = Editor.getCursor();
   const lineNumber = cursor.line;
   const lineContent = Editor.getLine(lineNumber);
   const afterCrlf  = LFDcursorAfterCrlf (cursor, lineContent);
   const inCrlf     = LFDcursorInCrlf    (cursor, lineContent);
   const beforeCrlf = LFDcursorBeforeCrlf(cursor, lineContent);
   if (Event.keyCode == 13) { // 'Enter' key
      LFDonEnter(Editor, inCrlf, cursor, lineContent);
      Event.preventDefault();
   }
   if (Event.keyCode == 8) { // 'Backspace' key
      LFDonBackspace(Event, Editor, inCrlf, afterCrlf, cursor, lineNumber, lineContent);
   }
   if (Event.keyCode == 46) { // 'Delete' key
      LFDonDelete(Event, Editor, inCrlf, beforeCrlf, cursor, lineNumber, lineContent);
   }
   if (Event.keyCode == 37) { // 'Left' key
      LFDonLeft(Event, Editor, afterCrlf, cursor);
   }
   if (Event.keyCode == 39) { // 'Right' key
      LFDonRight(Event, Editor, inCrlf, beforeCrlf, cursor);
   }
}  
