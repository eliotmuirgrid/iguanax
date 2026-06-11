function TBLupdateChangedFiles(){
   let FileData = TBLfileData();
   if(!FileData) {
      MODELdata.TBLfileData.data = {};
      FileData = TBLfileData();
   }
   if(!TBLrenderedFileData) TBLrenderedFileData = {};
   TBLchangedFiles = TBLdetermineChangedFiles(JSON.parse(JSON.stringify(FileData)), JSON.parse(JSON.stringify(TBLrenderedFileData)));
   TBLrenderedFileData = JSON.parse(JSON.stringify(FileData));
}

function TBLupdateListWindowsPrimer(){ TBLupdateListWindows([]); }
function TBLupdateNodeWindowsPrimer(){ TBLupdateNodeWindows([]); }

function TBLupdateListWindowsSecond(){ TBLupdateListWindows(TBLchangedFiles); }

function TBLupdateAnnotations(){ if(TBLchangedFiles.length) TRANSrunButtonClicked(); }

function TBLclearWindowGuids(List){ for(Guid in List) if (!WNDselector(Guid)) delete List[Guid]; }

function TBLgetFileList(){
   let FileList = {};
   for(WindowGuid in TBLnodeWindows) FileList[TBLnodeWindows[WindowGuid].file] = true;
   for(WindowGuid in TBLlistWindows) FileList[TBLlistWindows[WindowGuid]] = true;
   return Object.keys(FileList);
}

function TBLdetermineChangedFiles(NewData, OldData){
   let ChangedFiles = [];
   let OpenFiles = TBLgetFileList(); 
   for(FileName of OpenFiles){ if(!DEEPequal(NewData[FileName], OldData[FileName])) ChangedFiles.push(FileName); }
   return ChangedFiles;
}
