function ACTrenameDir(OldPath, DirName) {
   if (DirName.trim() === "") return SNCKsnackbar('error', "A folder name must be provided.");
   if (DirName[0] === ".")    return SNCKsnackbar('error', "Folder name cannot start with a '.' because it hides the folder.");
   let ParentFolder = ACTparentFolder(OldPath);
   let NewPath = (ParentFolder ? ParentFolder : "") + DirName;
   if(OldPath === NewPath) return;
   let data = {
      old_path    : OldPath,
      new_path    : NewPath,
      component   : TRANScomponentGuid(),
      development : TRANSdevelopment()
   };
   APIcall('component/dir/rename', data, function(Response){
      if(!Response.success) return SNCKsnackbar('error', Response["error"] + ' Cannot rename folder.');
      let Command = `git -C "${NewPath}" add -N .`;
      TGITopenBranchTerminal(function() {TERMexecuteCommand([Command]);});
      if (TRANSfile().includes(OldPath)) {
         const CurrentFile = TRANSfile().replace(OldPath, NewPath);
         TRANSfileChangeWithoutSaving(CurrentFile);
      }
      else MODELforcePoll('TRANSmodel');
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}