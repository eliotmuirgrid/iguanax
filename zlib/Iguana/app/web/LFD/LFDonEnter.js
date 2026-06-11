function LFDonEnter(Editor, inCrlf, cursor, lineContent){
   if(!inCrlf) LFDaddNewLine(Editor, cursor, lineContent);
   if( inCrlf) LFDaddNewLineAfterCrlf(Editor, cursor, lineContent);
}

function LFDaddNewLine(Editor, cursor, lineContent){
   let LineFeed = LFDselectedLineFeed();
   const content = lineContent.slice(0, cursor.ch);
   let addContent = "\n";
   if(!content.endsWith(LFDcr) && !content.endsWith(LFDlf)) addContent = LineFeed + addContent;
   Editor.replaceSelection(addContent);
}

function LFDaddNewLineAfterCrlf(Editor, cursor, lineContent){
   Editor.setCursor({
      line: cursor.line,
      ch: cursor.ch + 1
   });
   LFDaddNewLine(Editor, cursor, lineContent);
}