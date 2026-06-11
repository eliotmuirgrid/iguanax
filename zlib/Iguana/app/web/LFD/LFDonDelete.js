function LFDonDelete(Event, Editor, inCrlf, beforeCrlf, cursor, lineNumber, lineContent){
   if(beforeCrlf) return LFDdeleteCrlfLeftside(Event, Editor, cursor, lineNumber, lineContent);
   if(inCrlf)     return LFDdeleteCrlfMiddle(Event, Editor, cursor, lineNumber, lineContent);
}

function LFDdeleteCrlfLeftside(Event, Editor, cursor, lineNumber, lineContent){
   if (cursor.ch >= 0 && cursor.ch < lineContent.length - 1) {
      const updatedLineContent = lineContent.slice(0, cursor.ch) + lineContent.slice(cursor.ch + 2);
      Editor.replaceRange(updatedLineContent, { line: lineNumber, ch: 0 }, { line: lineNumber, ch: lineContent.length });
      Editor.setCursor({ line: lineNumber, ch: cursor.ch });
      Event.preventDefault();
   }
}

function LFDdeleteCrlfMiddle(Event, Editor, cursor, lineNumber, lineContent){
   Editor.setCursor({
      line: cursor.line,
      ch: cursor.ch + 1
   });
   let newCursor = Editor.getCursor();
   const beforeCrlf = LFDcursorBeforeCrlf(newCursor, lineContent);
   if(beforeCrlf) LFDdeleteCrlfLeftside(Event, Editor, cursor, lineNumber, lineContent);
}