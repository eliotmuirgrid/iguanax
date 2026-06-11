let ACTsourcePath  = ''
let ACTsourceName  = ''
let ACTdestination = ''

function ACTafterMove(Response, OldPath, Command, NewPath){
   COL_TRC(Response);
   // maybe it shouldn't be the destination folder that's too much for if you move a folder
   // - if it's a folder being moved only want to target inners
   
   TGITopenBranchTerminal(function() {TERMexecuteCommand([Command]);});
   if (!Response.success){
      if(Response.error.includes('exists')) return SNCKsnackbar('error', Response.error);
      return SNCKsnackbar('error', Response.error + ' Cannot move file or folder to the specified location.');
   }

   COL_TRC("Update annotations");
   TRANSideRequestCompile( function(){ TRANSupdateAnnotations(); } );
   if      (TRANSfile() === OldPath)       TRANSfileChangeWithoutSaving(NewPath);
   else if (TRANSfile().includes(OldPath)) TRANSfileChangeWithoutSaving(TRANSfile().replace(OldPath, NewPath));
   else                                    MODELforcePoll('TRANSmodel');
}

function ACTmove(OldPath = ACTsourcePath, Name = ACTsourceName, Dest = ACTdestination, Dir){
   const NewPath = Dest ? Dest + FILsep() + Name : Name;
   if(OldPath === Dest || OldPath === NewPath) { return; }
   let data = {
      old_path    : OldPath,
      new_path    : NewPath,
      component   : TRANScomponentGuid(),
      development : TRANSdevelopment()
   };
   COL_TRC("Moving file", data);
   let Command = ACTmoveGitCommand(Dir, NewPath, Name);
   APIcall('component/file/rename', data, function(R) { ACTafterMove(R, OldPath, Command, NewPath); });
   ACTsourcePath  = ACTsourceName  = ACTdestination = ''
}

function ACTmoveGitCommand(Dir, NewPath, Name){
   let Command;
   let ParentFolder = ACTparentFolder(NewPath);
   let ParentDiv = ACTremoveLastFileSep(ParentFolder);
   if(Dir) Command = `git -C "${NewPath}" add -N .`;
   else    Command = `git${ParentDiv !== "" ? ` -C "${ParentDiv}"` : ""} add -N "${Name}"`;
   return Command;
}