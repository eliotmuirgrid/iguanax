// New home for all things to do with the block data structure used for annotations.


function TRANSfindBlockAtLine(FunctionStartLine){
   const Blocks = TRANSifware.AnnotationBlockDataSet;
   if (Blocks) {
      for (let i = 0; i < Blocks.length; i++) {
         if (Blocks[i].scope.s == FunctionStartLine) {
            COL_TRC("Found block at line", FunctionStartLine, Blocks[i]);
            return Blocks[i];
         }
      }
   }
   COL_TRC("Unable to find block at line", FunctionStartLine);
   return null;
}