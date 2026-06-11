// Factoring out code for a callee jump

function TRANScalleeJump(FileLabel, FunctionStartLine, GoToLine, TargetCallee){
   COL_TRC("Callee jump - let's separate those concerns!");
   COL_TRC("Filelabel =", FileLabel, "FunctionStartLine =", FunctionStartLine, "TargetCallee =",TargetCallee, "GotoLine =", GoToLine);
   if (TRANScompareToRenderedFile(FileLabel)) {
      COL_TRC("Jumping in the local file.");
      TRANSscrollToLine(GoToLine);
      TRANSsetEditorScrollLeft(0); // scroll to the leftmost of the page since otherwise the scroll down might be too far right to see the function definition.
      TRANShighlightLine(GoToLine);
      TRANScalleeAnnotationCall(FunctionStartLine, TargetCallee);
   } else if (FileLabel) {
      COL_TRC("We have to change file, then jump to the line:", GoToLine);
      TRANSscrollToLineCallback = function() {
         TRANSonFunctionJumpToLine(GoToLine);
      }
      TRANSrequestFunctionalBlock= {};
      TRANSrequestFunctionalBlock.start_line = FunctionStartLine;
      TRANSrequestFunctionalBlock.target_callee = TargetCallee;
      COL_TRC("RequestFunctionBlock =", TRANSrequestFunctionalBlock);
      TRANSfileChange(FileLabel);
   } else {
      TRANSwarning('Source not available for this function.');
   }
}

function TRANScalleeAnnotationCall(FunctionStartLine, TargetCallee) {
   let Vars = TRANSprepareLuaPostInfo();  // Use common helper function to pack in Lua information
   COL_TRC("We have a callee iteration change");
   COL_TRC("TargetCallee =", TargetCallee);
   COL_TRC("FunctionStartLine =", FunctionStartLine);
   Vars.filename = TRANSrenderedFileName;
   Vars.start_line = FunctionStartLine;
   Vars.target_callee = TargetCallee;
   TRANSideOnIterationChange(Vars);
}