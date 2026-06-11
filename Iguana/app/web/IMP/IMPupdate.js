function IMPupdateDataBuffers(){
   IMPtoFileDataBuffer   = JSON.parse(JSON.stringify( IMPtoFileData   ));
   IMPfromFileDataBuffer = JSON.parse(JSON.stringify( IMPfromFileData ));
}

function IMPupdateToFileChanged(){
   let Changed = false;
   if(IMPtoFileData       === undefined) IMPtoFileData       = {};
   if(IMPtoFileDataBuffer === undefined) IMPtoFileDataBuffer = {};
   if( !DEEPequal( JSON.parse(JSON.stringify(IMPtoFileData)), JSON.parse(JSON.stringify(IMPtoFileDataBuffer))) ) Changed = true;
   IMPtoFileChanged = Changed;
}

function IMPupdateFromFileChanged(){
   let Changed = false;
   if(IMPfromFileData       === undefined) IMPfromFileData       = {};
   if(IMPfromFileDataBuffer === undefined) IMPfromFileDataBuffer = {};
   if( !DEEPequal( JSON.parse(JSON.stringify(IMPfromFileData)), JSON.parse(JSON.stringify(IMPfromFileDataBuffer))) ) Changed = true;
   IMPfromFileChanged = Changed;
}

function IMPupdateToFile(){
   let ForceUpdate = IMPtoFileChanged;
   if(IMPtoFile === undefined) return;
   let SearchField = document.querySelector('.IMPtoFileSearch')?.value.trim();
   let NodeData     = IMPfilterFile    (IMPtoFileData?.nodes,    SearchField);
   let MatchingData = IMPfilterMatching(IMPtoFileData?.matching, SearchField);
   let ToFileContent = document.querySelector('.IMPtoFileContent');
   if(ToFileContent.innerHTML !== "" && !ForceUpdate
      && DEEPequal(NodeData,     IMPtoFileNodeDataRendered) 
      && DEEPequal(MatchingData, IMPtoFileMatchingDataRendered) ) return;
      ToFileContent.innerHTML = IMPtoFileHtml(NodeData, MatchingData);
   IMPtoFileNodeDataRendered     = JSON.parse(JSON.stringify(NodeData));
   IMPtoFileMatchingDataRendered = JSON.parse(JSON.stringify(MatchingData));
}

function IMPupdateFromFile(){
   let ForceUpdate = IMPtoFileChanged || IMPfromFileChanged;
   let SearchField = document.querySelector('.IMPfromFileSearch')?.value.trim();
   let NodeData     = IMPfilterFile    (IMPfromFileData?.nodes,    SearchField);
   let MatchingData = IMPfilterMatching(IMPfromFileData?.matching, SearchField);
   let FromFileContent = document.querySelector('.IMPfromFileContent');
   if(FromFileContent.innerHTML !== "" && !ForceUpdate
      && DEEPequal(NodeData,     IMPfromFileNodeDataRendered) 
      && DEEPequal(MatchingData, IMPfromFileMatchingDataRendered) ) return; 
   FromFileContent.innerHTML = IMPfromFileHtml(NodeData, MatchingData);
   IMPfromFileNodeDataRendered     = JSON.parse(JSON.stringify(NodeData));
   IMPfromFileMatchingDataRendered = JSON.parse(JSON.stringify(MatchingData));
}

function IMPupdateImportList(){
   let ForceUpdate = IMPtoFileChanged || IMPfromFileChanged;
   if(ForceUpdate) { IMPimportNodeList = {}; IMPimportMatchingList = []; }
   let ImportListContent = document.querySelector('.IMPimportListContent');
   let ImportData = IMPimportData(IMPfromFileData?.nodes);
   if(ImportListContent.innerHTML !== "" 
      && DEEPequal(ImportData,            IMPimportNodeListDataRendered)
      && DEEPequal(IMPimportMatchingList, IMPimportMatchingListRendered) 
      && !ForceUpdate) return;
   ImportListContent.innerHTML = IMPimportListHtml(ImportData, IMPimportMatchingList);
   IMPimportMatchingListRendered = JSON.parse(JSON.stringify(IMPimportMatchingList));
   IMPimportNodeListDataRendered = JSON.parse(JSON.stringify(ImportData));
}

function IMPfilterFile(FileData, FilterTerm){
   let FilteredData = {};
   for(Item in FileData) {
      if(Item.toLowerCase().includes(FilterTerm.toLowerCase()) || FileData[Item].desc.toLowerCase().includes(FilterTerm.toLowerCase())) FilteredData[Item] = FileData[Item];
   }
   return FilteredData;
}

function IMPfilterMatching(MatchingData, FilterTerm){
   let FilteredData = [];
   if(MatchingData === undefined) return FilteredData;
   for(Item of MatchingData) {
      if(Item.name.toLowerCase().includes(FilterTerm.toLowerCase()) || Item.rule.toLowerCase().includes(FilterTerm.toLowerCase())) FilteredData.push(Item);
   }
   return FilteredData;
}

function IMPimportData(FileData){
   let ImportData = {};
   for(Item in FileData){
      if(!IMPimportNodeList[Item]) continue;
      ImportData[Item] = FileData[Item];
   }
   return ImportData;
}
