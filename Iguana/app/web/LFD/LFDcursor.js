function LFDcursorInCrlf(cursor, lineContent){
   return cursor.ch > 0 && 
      cursor.ch < lineContent.length &&
      lineContent[cursor.ch - 1] === LFDcr &&
      lineContent[cursor.ch]     === LFDlf;
}

function LFDcursorBeforeCrlf(cursor, lineContent){
   return cursor.ch >= 0 &&
      cursor.ch < lineContent.length - 1 &&
      lineContent[cursor.ch]     === LFDcr &&
      lineContent[cursor.ch + 1] === LFDlf;
}

function LFDcursorAfterCrlf(cursor, lineContent){
   if (cursor.ch < 2) return false;
   const contentBeforeCursor = lineContent.slice(cursor.ch - 2, cursor.ch);
   return contentBeforeCursor === LFDcrlf;
}
