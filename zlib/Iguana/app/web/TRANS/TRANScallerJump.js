function TRANScallerJump(FileLabel, FunctionStartLine, FunctionCallNumber, GoToLine) {
   COL_TRC("Filelabel =", FileLabel, "FunctionStartLine =", FunctionStartLine, "FunctionCallNumber = ", FunctionCallNumber, "GotoLine =", GoToLine);
   if (TRANScompareToRenderedFile(FileLabel)) {
      COL_TRC("Jumping in the local file.");
      TRANSscrollToLine(GoToLine);
      TRANSsetEditorScrollLeft(0); // scroll to the leftmost of the page since otherwise the scroll down might be too far right to see the function definition.
      TRANShighlightLine(GoToLine);
      TRANScallerOnScopeChange(FunctionStartLine, FunctionCallNumber);
   } else if (FileLabel) {
      COL_TRC("We have to change file, then jump to the line:", GoToLine);
      TRANSscrollToLineCallback = function() {
         TRANSonFunctionJumpToLine(GoToLine);
      }
      TRANSrequestFunctionalBlock = {};
      TRANSrequestFunctionalBlock.start_line = FunctionStartLine;
      TRANSrequestFunctionalBlock.iteration  = FunctionCallNumber;
      COL_TRC("RequestFunctionBlock =", TRANSrequestFunctionalBlock);
      TRANSfileChange(FileLabel);
   } else {
      TRANSwarning('Source not available for this function.');
   }
}

function TRANScallerOnScopeChange(FunctionStartLine, FunctionCallNumber) {
   let Block = TRANSfindBlockAtLine(FunctionStartLine);
   const NewIteration = Math.min(Block.scope.l, FunctionCallNumber);
   if (NewIteration === Block.scope.p){
      COL_TRC("No need to change the scope.");
      return;
   }
   Block.scope.c = [];
   Block.scope.p = NewIteration;

   let Vars = TRANSprepareLuaPostInfo();  // Use common helper function to pack in Lua information
   COL_TRC("We have a scope iteration change");
   Vars.filename = TRANSrenderedFileName;
   Vars.start_line = FunctionStartLine;
   Vars.iteration_tree = TRANSbuildIterationTree(Block.scope);
   COL_TRC("iteration tree=", Vars.iteration_tree);
   TRANSideOnIterationChange(Vars);
}