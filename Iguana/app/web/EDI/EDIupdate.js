function EDIupdateNodeWindows(){ for(WindowId in EDInodeWindows) EDIupdateNodeWindow(WindowId, EDIchangedFiles);  }

function EDIupdateNodeWindow(WindowId, ChangedFiles){
   let NodeName = EDInodeWindows[WindowId].name;
   let OldNodeName = EDInodeWindows[WindowId].oldName;
   let FileName = EDInodeWindows[WindowId]?.file;
   let FileData = EDIfileData();
   if((!FileData || !FileData[FileName] || !FileData[FileName]["nodes"] || FileData[FileName]["error"]) && !EDImidRequest) return WNDcloseWindow(WindowId);
   if(!FileData[FileName]["nodes"][NodeName] && !FileData[FileName]["nodes"][OldNodeName]) return WNDcloseWindow(WindowId);
   if(OldNodeName === NodeName) delete EDInodeWindows[WindowId].oldName;
   let NodeData = JSON.parse(JSON.stringify(FileData[FileName]["nodes"]));
   if(!WNDselector(WindowId)) return;
   if(WNDcontentSelector(WindowId).innerHTML !== "" && ChangedFiles.indexOf(FileName) === -1){ return; }
   let Content = EDInodeHtml(NodeName, FileName, Object.keys(EDIparents(NodeName, NodeData)));
   WNDupdateContent(WindowId, Content);
   EDIupdateTitle(WindowId, NodeName, FileName);
}

function EDIupdateTitle(WindowId, NodeName, FileName){
   if(WNDselector(WindowId).querySelector('.WNDtitle').innerHTML !== EDInodeWindowTitle(NodeName, FileName)) WNDselector(WindowId).querySelector('.WNDtitle').innerHTML = EDInodeWindowTitle(NodeName, FileName);
}

function EDIupdateListWindows(){
   let FileData = EDIfileData();
   for(WindowId in EDIlistWindows){ 
      if(!WNDselector(WindowId)) continue;
      let FileName = EDIlistWindows[WindowId];
      if(EDIerrorOccured(FileData, FileName)) return WNDcloseWindow(WindowId);
      let Data = FileData[FileName] ? JSON.parse(JSON.stringify(FileData[FileName])) : {};
      if(WNDcontentSelector(WindowId).innerHTML !== "" &&  EDIchangedFiles.indexOf(FileName) === -1 ) continue;
      let Content = !EDIeditListWindows[WindowId] ? EDIlistHtml(Data, FileName) : EDIeditListHtml(Data["nodes"], FileName);
      WNDupdateContent(WindowId, Content); 
   }
}

function EDIupdateMatchingWindows(){
   let FileData = EDIfileData();
   for(WindowId in EDImatchingWindows){
      if(!WNDselector(WindowId)) continue;
      let FileName = EDImatchingWindows[WindowId];
      if(EDIerrorOccured(FileData, FileName)) return WNDcloseWindow(WindowId);
      if((WNDcontentSelector(WindowId).innerHTML !== "" && EDIchangedFiles.indexOf(FileName) === -1)) continue;
      WNDupdateContent(WindowId, EDImatchingHtml(FileData[FileName]?.matching, FileName)); 
   }
}

function EDIerrorOccured(FileData, FileName){
   return (!FileData || !FileData[FileName] || FileData[FileName]["error"]) && !EDImidRequest;
}

function EDIparents(DeletedName, Data){
   let NodeNames = {};
   for(NodeName in Data){
      if(!Data[NodeName].children) continue;
      for(let i=0; i < Data[NodeName].children.length; i++) if(Data[NodeName].children[i].type === DeletedName) NodeNames[NodeName] = true; 
   }
   return NodeNames;
}

function EDIcalculateDependencies(Data){
   let DependencyData = {};
   for(NodeName in Data) DependencyData[NodeName] = EDIparents(NodeName, Data); 
   return DependencyData;
}
