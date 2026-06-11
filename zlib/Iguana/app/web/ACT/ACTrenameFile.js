function ACTremoveLastFileSep(inputString){
   let endSubstring = FILsep();
   if (inputString.endsWith(endSubstring)) {
         return inputString.slice(0, -endSubstring.length);
   } else {
         return inputString;
   }
}

function ACTparentFolder       (FilePath) { return FilePath.substring(0, FilePath.lastIndexOf(FILsep()) + 1); }
function ACTluaFileContainsDot (Filename) { return (Filename.match(/\./g) || []).length > 1 && Filename.endsWith(".lua"); }

function ACTrenameFile(OldPath, Filename) {
   if (Filename.trim() === "")          return SNCKsnackbar('error', "A filename must be provided.")
   if (Filename[0] == ".")              return SNCKsnackbar('error', "Filename cannot start with a '.' because it hides the file.")
   if (ACTluaFileContainsDot(Filename)) return SNCKsnackbar('error', "Lua files cannot contain '.' in their name.")
   let ParentFolder = ACTparentFolder(OldPath);
   let NewPath = (ParentFolder ? ParentFolder : "") + Filename;
   if(OldPath === NewPath) return;
   let data = {
      old_path    : OldPath,
      new_path    : NewPath,
      component   : TRANScomponentGuid(),
      development : TRANSdevelopment()
   };
   APIcall('component/file/rename', data, function(Response){
      if (!Response.success) return SNCKsnackbar('error', Response["error"] + ' Cannot rename file.');
      let ParentDirPath = ACTremoveLastFileSep(ParentFolder);
      let Command = `git${ParentFolder !== "" ? ` -C "${ParentDirPath}"` : ""} add -N "${Filename}"`;
      TGITopenBranchTerminal(function() {TERMexecuteCommand([Command]);});
      if (TRANSfile() == OldPath) TRANSfileChangeWithoutSaving(NewPath);
      else                        MODELforcePoll('TRANSmodel');
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}