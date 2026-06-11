function DBMupdateDataBuffers(){
   DBMtoFileDataBuffer   = JSON.parse(JSON.stringify( DBMtoFileData   ));
   DBMfromFileDataBuffer = JSON.parse(JSON.stringify( DBMfromFileData ));
}

function DBMupdateToFileChanged(){
   let Changed = false;
   if(DBMtoFileData       === undefined) DBMtoFileData       = {};
   if(DBMtoFileDataBuffer === undefined) DBMtoFileDataBuffer = {};
   if( !DEEPequal( JSON.parse(JSON.stringify(DBMtoFileData)), JSON.parse(JSON.stringify(DBMtoFileDataBuffer))) ) Changed = true;
   DBMtoFileChanged = Changed;
}

function DBMupdateFromFileChanged(){
   let Changed = false;
   if(DBMfromFileData       === undefined) DBMfromFileData       = {};
   if(DBMfromFileDataBuffer === undefined) DBMfromFileDataBuffer = {};
   if( !DEEPequal( JSON.parse(JSON.stringify(DBMfromFileData)), JSON.parse(JSON.stringify(DBMfromFileDataBuffer))) ) Changed = true;
   DBMfromFileChanged = Changed;
}

function DBMvmdFiles(){
   let Files = [];
   if(DBMtoFile   && DBMtoFile.trim() !== "")   Files.push(DBMtoFile);
   if(DBMfromFile && DBMfromFile.trim() !== "") Files.push(DBMfromFile);
   return Files;
}

function DBMprepareGlobals(){
   let FileData = DBMfileData();
   if( FileData === undefined ) DBMtoFile   = "";
   if( DBMtoFile != "" && FileData[DBMtoFile] === undefined ) DBMtoFile = "";
   if(DBMtoFile && DBMtoFile !== "") DBMtoFileData = FileData[DBMtoFile];
   else DBMtoFileData = {};
   if(DBMfromDatabase === "") {
      if(DBMfromFile && DBMfromFile !== "") DBMfromFileData = FileData[DBMfromFile];
      else DBMfromFileData = {};
   }
}

function DBMupdateToFile(){
   let ForceUpdate = DBMtoFileChanged;
   if(DBMtoFile === undefined) return;
   let SearchField = document.querySelector('.DBMtoFileSearch');
   if(!SearchField) return;
   let Data = DBMfilterFile(DBMtoFileData?.tables, SearchField.value.trim());
   let ToFileContent = document.querySelector('.DBMtoFileContent');
   if(ToFileContent.innerHTML !== "" && !ForceUpdate && DEEPequal(Data,DBMtoFileDataRendered)) return;
   ToFileContent.innerHTML = DBMtoFileHtml(Data);
   DBMtoFileDataRendered = JSON.parse(JSON.stringify(Data));
}

function DBMupdateFromFile(){
   let ForceUpdate = DBMtoFileChanged|| DBMfromFileChanged;
   let SearchField = document.querySelector('.DBMfromFileSearch');
   if(!SearchField) return;
   let Data = DBMfilterFile(DBMfromFileData?.tables, SearchField.value.trim());
   let FromFileContent = document.querySelector('.DBMfromFileContent');
   if(FromFileContent.innerHTML !== "" && !ForceUpdate && DEEPequal(Data,DBMfromFileDataRendered)) return;
   FromFileContent.innerHTML = DBMfromFileHtml(Data);
   DBMfromFileDataRendered = JSON.parse(JSON.stringify(Data));
}

function DBMupdateImportList(){
   let ForceUpdate = DBMtoFileChanged|| DBMfromFileChanged;
   if(ForceUpdate) { DBMimportList = {}; }
   let ImportListContent = document.querySelector('.DBMimportListContent');
   if(!ImportListContent) return;
   let ImportData = DBMimportData(DBMfromFileData?.tables);
   if(ImportListContent.innerHTML !== "" && DEEPequal(ImportData, DBMimportNodeListDataRendered) && !ForceUpdate) return;
   ImportListContent.innerHTML = DBMimportListHtml(ImportData, DBMimportMatchingList);
   DBMimportNodeListDataRendered = JSON.parse(JSON.stringify(ImportData));
}

function DBMfilterFile(FileData, FilterTerm){
   let FilteredData = {};
   for(Item in FileData) {
      if(Item.toLowerCase().includes(FilterTerm.toLowerCase()) || FileData[Item].desc.toLowerCase().includes(FilterTerm.toLowerCase())) FilteredData[Item] = FileData[Item];
   }
   return FilteredData;
}
function DBMimportData(FileData){
   let ImportData = {};
   for(Item in FileData){
      if(!DBMimportList[Item]) continue;
      ImportData[Item] = FileData[Item];
   }
   return ImportData;
}
