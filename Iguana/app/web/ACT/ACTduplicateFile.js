function ACTduplicateFile(CurrentTreeNode){
   const Filepath = CurrentTreeNode.path;
   const Filename = CurrentTreeNode.name;
   MENUremoveTooltip();
   const Data = { path: Filepath, component: TRANScomponentGuid(), development : TRANSdevelopment()};
   APIcall('component/file', Data, function(Response){
      if (ACTbadResponse(Response)) return ACTduplicateError(Response.error);
      const FileContent = Response["data"]["content"];
      const Data = { path: ACTparentFolder(Filepath), component: TRANScomponentGuid(), development : TRANSdevelopment()};
      APIcall('component/dir', Data, function(Response){
         if (ACTbadResponse(Response)) return ACTduplicateError(Response["error"]);
         let Destination = ACTparentFolder(Filepath) + ACTgenerateCopyName(Filename, ACTexistingFiles(Response.data.content));
         const Data = { path: Destination, content: FileContent, component: TRANScomponentGuid(), development : TRANSdevelopment()};
         APIcall('component/file/add', Data, function(Response){
            if (!Response["success"]) return ACTduplicateError(Response["error"]);
            SNCKsnackbar('success', Destination + ' pasted successfully.', 20000);
            TRANSfileChange(Destination);
            TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
         })
      }) 
   })
}

function ACTexistingFiles(Folder){
   let Files = []
   for (const Item in Folder){ if(!Folder[Item].dir) Files.push(Folder[Item].name); }
   return Files
}

function ACTbadResponse(Response){
   if (!Response["success"] || !Response["data"] || !Response["data"]["exists"]) return true;
   return false;
}

function ACTduplicateError(Error){
   return SNCKsnackbar('error', Error + ' Cannot duplicate file.');
}