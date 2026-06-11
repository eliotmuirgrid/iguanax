function TBLupdateListWindows(ChangedFiles){
   let FileData = TBLfileData();
   for(WindowId in TBLlistWindows){ 
      if(!WNDselector(WindowId)) continue;
      let FileName = TBLlistWindows[WindowId];
      if(!FileData || !FileData[FileName] || !FileData[FileName]["tables"]) {
         COL_TRC("Closing " + WindowId + " because no file data, or filename not in file data.");
         return WNDcloseWindow(WindowId);
      }
      let Data = FileData[FileName]["tables"] ? JSON.parse(JSON.stringify(FileData[FileName]["tables"])) : {};
      if(WNDcontentSelector(WindowId).innerHTML !== "" && ChangedFiles.indexOf(FileName) === -1 ) continue;
      let Content = !TBLeditListWindows[WindowId] ? TBLlistHtml(Data, FileName) : TBLeditListHtml(Data, FileName);
      WNDupdateContent(WindowId, Content); 
   }
}

function TBLupdateNodeWindows(ChangedFiles){
   for(WindowId in TBLnodeWindows) TBLupdateNodeWindow(WindowId, ChangedFiles);
}


function TBLupdateNodeWindow(WindowId, ChangedFiles){
   let NodeName = TBLnodeWindows[WindowId].name;
   let OldNodeName = TBLnodeWindows[WindowId].oldName;
   let FileName = TBLnodeWindows[WindowId].file;
   let FileData = TBLfileData();
   if(!FileData || !FileData[FileName] || !FileData[FileName]["tables"]) { 
      COL_TRC("Closing " + WindowId + " because no file data, or filename not in file data.");
      return WNDcloseWindow(WindowId);
   }
   if(!FileData[FileName]["tables"][NodeName] && !FileData[FileName]["tables"][OldNodeName]) { 
      COL_TRC("Closing " + WindowId + " because node not found.");
      return WNDcloseWindow(WindowId);
   }
   if(OldNodeName === NodeName) delete TBLnodeWindows[WindowId].oldName;
   if(!WNDselector(WindowId)) return;
   if(WNDcontentSelector(WindowId).innerHTML !== "" && ChangedFiles.indexOf(FileName) === -1){ return; }
   let Content = TBLnodeHtml(NodeName, FileName);
   WNDupdateContent(WindowId, Content);
   TBLupdateTitle(WindowId, NodeName, FileName);
}

function TBLupdateTitle(WindowId, NodeName, FileName){
   let Title = `${NodeName} - ${FileName}`;
   if(WNDselector(WindowId).querySelector('.WNDtitle').innerHTML !== Title) WNDselector(WindowId).querySelector('.WNDtitle').innerHTML = Title;
}