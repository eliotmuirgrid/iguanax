function LFDonBackspace(Event, Editor, inCrlf, afterCrlf, cursor, lineNumber, lineContent){
   if(afterCrlf) return LFDdeleteCrlfRightside(Event, Editor, cursor, lineNumber, lineContent);
   if(inCrlf)    return LFDdeleteCrlfMiddle(Event, Editor, cursor, lineNumber, lineContent);
}

function LFDdeleteCrlfRightside(Event, Editor, cursor, lineNumber, lineContent){
   const updatedLineContent = lineContent.slice(0, cursor.ch - 2) + lineContent.slice(cursor.ch);
   Editor.replaceRange(updatedLineContent, { line: lineNumber, ch: 0 }, { line: lineNumber, ch: lineContent.length });
   Editor.setCursor({ line: lineNumber, ch: cursor.ch - 2 });
   Event.preventDefault();
}

function LFDdeleteCrlfMiddle(Event, Editor, cursor, lineNumber, lineContent){
   const updatedLineContent = lineContent.slice(0, cursor.ch - 1) + lineContent.slice(cursor.ch + 1);
   Editor.replaceRange(updatedLineContent, { line: lineNumber, ch: 0 }, { line: lineNumber, ch: lineContent.length });
   Editor.setCursor({ line: lineNumber, ch: cursor.ch - 1 });
   Event.preventDefault();
}