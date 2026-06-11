let ACTcopy = {Path: '', Name: '', Content: '' };

function ACTcopyFile(CurrentTreeNode){
   ACTcopy.Path = CurrentTreeNode.path;
   ACTcopy.Name = CurrentTreeNode.name;
   MENUremoveTooltip();
   const Data = { path: CurrentTreeNode.path, component: TRANScomponentGuid(), development : TRANSdevelopment() }
   APIcall('component/file', Data, function(Response){
      if (ACTbadResponse(Response)) return ACTcopyError(Response.error);
      ACTcopy.Content = Response.data.content;
      SNCKsnackbar('success', CurrentTreeNode.name + ' copied to internal clipboard.  Use paste on folder to copy it.', 20000);
      TRANSideRequestCompile( function(){ 
         TRANSupdateAnnotations(); 
         TRANSsetCode();
      } );
   })
}

function ACTsplitName(Filename){
   let Name      = Filename;
   let Extension = '';
   if(Filename.includes('.')){
      Name      = Filename.substring(0, Filename.lastIndexOf('.'));
      Extension = Filename.substring(Filename.lastIndexOf('.'));
   }
   return {Name: Name, Ext: Extension};
}

function ACTgenerateCopyName(Filename, ListOfNames = []){
   if(!ListOfNames.includes(Filename)) return Filename;
   let   Name      = ACTsplitName(Filename).Name;
   const Extension = ACTsplitName(Filename).Ext;
   const CopyRegex = /_copy\s?(\d)*$/;
   if(CopyRegex.test(Name)){ Name = Name.replace(CopyRegex, '')};
   let NewName = `${Name}_copy${Extension}`;
   if(!ListOfNames.includes(NewName)) return NewName;
   for(let i = 2; true; i++){ 
      NewName = `${Name}_copy ${i}${Extension}`;
      if(!ListOfNames.includes(NewName)) return NewName;
   }
}

function ACTcopyError(Error){
   console.log('Error: ' + Error);
   return SNCKsnackbar('error', 'Cannot copy file.');
}