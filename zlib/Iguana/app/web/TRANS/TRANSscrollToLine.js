let TRANSscrollToLineCallback = null;

// This call back is often used to do more just jump to a line
// we are often jumping to a frame.

function TRANSscrollToLine(LineNumber) {
   COL_TRC(LineNumber);
   const LineTop = TRANSgetLineTop(LineNumber);
   // #26835 This used to use $(...).get(0).scrollX like the others but it's not having any effect
   // when jumping from another file via an annotation anymore. Using the CodeMirror function
   // works. I won't update them all though unless there are similar issues.
   TRANSeditor.scrollTo(0, LineTop - (TRANSeditorHeight() / 2));
}

// TRANSscrollToLineCallback is a callback function which will scroll to a desired line after the current file has been changed.
function TRANSscrollToLineCheckCallBack(){
   COL_TRC("TRANSscrollToLineCallback=", TRANSscrollToLineCallback);
   if (TRANSscrollToLineCallback) {
      COL_TRC("A scroll to line function callback exists.");
      TRANSscrollToLineCallback();
      TRANSscrollToLineCallback = undefined;
      COL_TRC("Set it to null");
   }
}

function TRANSscrollToLineSetCallback(Func){
   COL_TRC(Func);
   TRANSscrollToLineCallback = Func;
}