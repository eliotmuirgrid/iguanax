function EDIopenListWindow(FileName){
   let WindowGuid = EDIlistWindowGuid(FileName);
   if(WindowGuid) return WNDfocusOnWindow(WindowGuid);
   WindowGuid = EDIgenerateWindowGuid();
   EDIlistWindows[WindowGuid] = FileName;
   WNDcreateWindow(WindowGuid, "", FileName, "EDIvmdFile");
   WNDaddToDock(WindowGuid, "WNDdockRight");
   MODELforcePoll('EDImodel');
}

function EDIopenMatchingWindow(FileName){
   let WindowGuid = EDImatchingWindowGuid(FileName);
   if(WindowGuid) return WNDfocusOnWindow(WindowGuid);
   WindowGuid = EDIgenerateWindowGuid();
   EDImatchingWindows[WindowGuid] = FileName;
   WNDcreateWindow(WindowGuid, "", "Matching Rules - "+FileName,"EDImatchingRule");
   MODELforcePoll('EDImodel');
}

function EDIopenNodeWindow(NodeName, FileName){
   let FileData = EDIfileData();
   let WindowGuid = EDInodeWindowGuid(NodeName, FileName);
   if(WindowGuid) return WNDfocusOnWindow(WindowGuid);
   let WindowData = {};
   WindowData.name = NodeName;
   WindowData.file = FileName;
   WindowData.type = FileData[FileName]["nodes"][NodeName].type;
   WindowGuid = EDIgenerateWindowGuid();
   EDInodeWindows[WindowGuid] = WindowData;
   let Height = EDIdefaultHeight(FileData[FileName]["nodes"][NodeName]);
   WNDcreateWindow(WindowGuid, "", EDInodeWindowTitle(NodeName, FileName), `${EDIiconClass( FileData[FileName]["nodes"][NodeName].type )}` ,"","",Height);
   MODELforcePoll('EDImodel');
   return WindowGuid;
}

function EDIdefaultHeight(FileData){
   let Height = 200;
   if(FileData.children) Height = 160 + FileData.children.length * 24;
   if(Height < 200) Height = 200;
   if(Height > 800) Height = 800;
   return Height + "px";
}

function EDInodeWindowTitle(NodeName, FileName){ return `${NodeName} - ${FileName}`; }

function EDInodeWindowGuid(NodeName, FileName){
   for(WindowGuid in EDInodeWindows){
      if(!EDInodeWindows[WindowGuid]) continue;
      if(EDInodeWindows[WindowGuid].name !== NodeName) continue;
      if(EDInodeWindows[WindowGuid].file !== FileName) continue;
      if(!WNDselector(WindowGuid)) continue; 
      return WindowGuid;
   }
}

function EDIlistWindowGuid(FileName){
   for(WindowGuid in EDIlistWindows){ if(EDIlistWindows[WindowGuid] === FileName && WNDselector(WindowGuid)) return WindowGuid; }
}

function EDImatchingWindowGuid(FileName){
   for(WindowGuid in EDImatchingWindows){ if(EDImatchingWindows[WindowGuid] === FileName && WNDselector(WindowGuid)) return WindowGuid; }
}

// Used when we create a new .vmd file in translator
function EDIblankVmd(){
   return `{
      "nodes": {},
      "matching": []
   }`
}