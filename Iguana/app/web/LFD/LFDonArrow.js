function LFDonLeft(Event, Editor, afterCrlf, cursor){
   if(afterCrlf) return LFDmoveDoubleLeft(Event, Editor, cursor);
}

function LFDmoveDoubleLeft(Event, Editor, cursor){
   const newCh = Math.max(cursor.ch - 2, 0);
   Editor.setCursor({
     line: cursor.line,
     ch: newCh
   });
   Event.preventDefault();
}

function LFDonRight(Event, Editor, inCrlf, beforeCrlf, cursor){
   if(beforeCrlf) return LFDmoveDoubleRight(Event, Editor, cursor, true);
   if(inCrlf)     return LFDmoveDoubleRight(Event, Editor, cursor, false);
}

function LFDmoveDoubleRight(Event, Editor, cursor, preventDefault){
   const newCh = cursor.ch + 2;
   Editor.setCursor({
      line: cursor.line,
      ch: newCh
    });
   if(preventDefault) Event.preventDefault();
}