function TRGTconvertToFileDiffName(inputString){
   const parts = inputString.split(" -> ");
   if (parts.length <= 1) return inputString;
   return parts[1];
}


function TRGTconvertToGitName(inputString){
   const parts = inputString.split(" -&gt; ");
   if (parts.length <= 1) return inputString;
   return parts[1];
}

function TRGTconvertToGitPreviousName(inputString){
   const parts = inputString.split(" -&gt; ");
   if (parts.length <= 1) return "";
   return parts[0];
}

function TRGTstatus(FilePath){
   let Data = TRGTuncommittedChanges();
   if(!Data) return;
   for(Lib in Data){
      for(file of Data[Lib]){
         let Path = file?.path || "";
         let Status = file?.status.trim() || "";
         if(Path == FilePath) return Status;
         const parts = Path.split(" -> ");
         if(parts.length <= 1) continue;
         if(parts[1] == FilePath) return Status;
      }
   }
}
