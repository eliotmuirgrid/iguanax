function EDIgetFileList(){
   let FileList = {};
   for(WindowGuid in EDInodeWindows)     FileList[EDInodeWindows[WindowGuid].file] = true;
   for(WindowGuid in EDIlistWindows)     FileList[EDIlistWindows[WindowGuid]]      = true;
   for(WindowGuid in EDImatchingWindows) FileList[EDImatchingWindows[WindowGuid]]  = true;
   return Object.keys(FileList);
}

function EDIclearWindowGuids(List){ for(Guid in List) if (!WNDselector(Guid)) delete List[Guid]; }

function EDIupdateWindowGuids(){
   EDIclearWindowGuids(EDIlistWindows);
   EDIclearWindowGuids(EDInodeWindows);
   EDIclearWindowGuids(EDImatchingWindows);
   EDIclearWindowGuids(EDIeditList);
}

function EDIupdateChangedFiles(){
   let FileData = EDIfileData();
   if(!FileData) {
      MODELdata.EDIfileData.data = {};
      FileData = EDIfileData();
   }
   if(!EDIrenderedFileData) EDIrenderedFileData = {};
   EDIchangedFiles = EDIdetermineChangedFiles(JSON.parse(JSON.stringify(FileData)), JSON.parse(JSON.stringify(EDIrenderedFileData)));
   EDIrenderedFileData = JSON.parse(JSON.stringify(FileData));
}

function EDIupdateAnnotations(){ 
   if(EDIchangedFiles.length) TRANSrunIfAutoRunEnabled(); 
}

function EDIdetermineChangedFiles(NewData, OldData){
   let ChangedFiles = [];
   let OpenFiles = EDIgetFileList(); 
   for(FileName of OpenFiles){ if(!DEEPequal(NewData[FileName], OldData[FileName])) ChangedFiles.push(FileName); }
   return ChangedFiles;
}
