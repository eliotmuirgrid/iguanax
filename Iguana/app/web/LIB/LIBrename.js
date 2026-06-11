function LIBrename(library, DirName) {
   if(DirName.trim() === "") { return SNCKsnackbar('error', "A folder name must be provided."); }
   if(DirName[0] === ".")    { return SNCKsnackbar('error', "Folder name cannot start with a '.' because it hides the folder."); }
   let parent = ACTparentFolder(library);
   let rename = (parent ? parent : "") + DirName;
   if(library === rename) { return; }
   let data = {library: library, rename: rename, component: TRANScomponentGuid(), development: TRANSdevelopment()};
   APIcall('component/library/rename', data, function(Response) {
      if(!Response.success) { return SNCKsnackbar('error', Response.error); }
      if(TRANSfile().includes(library)) {
         const CurrentFile = TRANSfile().replace(library, rename);
         TRANSfileChangeWithoutSaving(CurrentFile);
      } else {
         TRANSfileCurrentReload();
         TRANSopenDir = rename;
         MODELforcePoll('TRANSmodel');
      }
      TRANSideRequestCompile(function() { TRANSupdateAnnotations(); });
      RNMrender(rename, library);
   });
}