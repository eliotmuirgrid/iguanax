function TBLopenListWindow(FileName){
   let WindowGuid = TBLlistWindowGuid(FileName);
   if(WindowGuid) return WNDfocusOnWindow(WindowGuid);
   WindowGuid = TBLgenerateWindowGuid();
   TBLlistWindows[WindowGuid] = FileName;
   WNDcreateWindow(WindowGuid, "", FileName, "TBLdatabase");
   WNDaddToDock(WindowGuid, "WNDdockRight");
   MODELforcePoll('TBLmodel');
}

function TBLopenNodeWindow(NodeName, FileName){
   let WindowGuid = TBLnodeWindowGuid(NodeName, FileName);
   if(WindowGuid) return WNDfocusOnWindow(WindowGuid);
   let WindowData = {};
   WindowData.name = NodeName;
   WindowData.file = FileName;
   WindowGuid = TBLgenerateWindowGuid();
   TBLnodeWindows[WindowGuid] = WindowData;
   let FileData = TBLfileData();
   let Height = TBLdefaultHeight(FileData[FileName]["tables"][NodeName]);
   WNDcreateWindow(WindowGuid, "", `${NodeName} - ${FileName}`, "TBLtable" ,"","",Height);
   MODELforcePoll('TBLmodel');
   return WindowGuid;
}

function TBLlistWindowGuid(FileName){
   for(WindowGuid in TBLlistWindows){ if(TBLlistWindows[WindowGuid] === FileName && WNDselector(WindowGuid)) return WindowGuid; }
}

function TBLnodeWindowGuid(NodeName, FileName){
   for(WindowGuid in TBLnodeWindows){
      if(!TBLnodeWindows[WindowGuid]) continue;
      if(TBLnodeWindows[WindowGuid].name !== NodeName) continue;
      if(TBLnodeWindows[WindowGuid].file !== FileName) continue;
      if(!WNDselector(WindowGuid)) continue; 
      return WindowGuid;
   }
}

function TBLgenerateWindowGuid(){
   return "TBL" + ([1e7]+-1e3+-4e3+-8e3+-1e11).replace(/[018]/g, c => (c ^ crypto.getRandomValues(new Uint8Array(1))[0] & 15 >> c / 4).toString(16) ); 
}

function TBLdefaultHeight(FileData){
   let Height = 200;
   if(FileData.columns) Height = 160 + FileData.columns.length * 24;
   if(Height < 200) Height = 200;
   if(Height > 800) Height = 800;
   return Height + "px";
}

// Used when we create a new .vdb file in translator
function TBLblankFile(){
   return `{"tables": { }}`;
}