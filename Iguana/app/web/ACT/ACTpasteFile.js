function ACTpasteFile(CurrentTreeNode){
   let Filename = ACTcopy.Name;
   const Filepath = CurrentTreeNode.path;
   MENUremoveTooltip();
   const Data = { path: Filepath + FILsep(), component: TRANScomponentGuid(), development : TRANSdevelopment()};
   APIcall('component/dir', Data, function(Response){
      if (ACTbadResponse(Response)) return SNCKsnackbar('error', Response.error + ' Cannot paste file.');
      let ExistingFiles = ACTexistingFiles(Response.data.content);
      if(ExistingFiles.includes(Filename)) Filename = ACTgenerateCopyName(Filename, ExistingFiles);
      const Destination = Filepath ? Filepath + FILsep() + Filename : Filename;
      if(ExistingFiles.includes(Filename)) return ACTreplaceFileWindow(Filename, Destination, ExistingFiles);
      ACTpasteApiCall(Destination, function(){ MODELforcePoll('TRANSmodel'); });
   }) 
}

function ACTpasteApiCall(Destination, Callback = '') {
   const Data = { path: Destination, content: ACTcopy.Content, component: TRANScomponentGuid(), development : TRANSdevelopment() };
   APIcall('component/file/add', Data, function(Response){
      if (!Response.success) return SNCKsnackbar('error', Response.error + ' Cannot paste file.');
      SNCKsnackbar('success', Destination + ' pasted successfully.', 20000);
      if(Callback) Callback();
      TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   });
}
