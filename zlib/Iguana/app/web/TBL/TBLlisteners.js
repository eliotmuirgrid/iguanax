function TBLlisteners(e){
   if(e.target.closest('.TBLimport')){
      let WindowId = WNDeventId(e);
      let FileName = TBLlistWindows[WindowId];
      DBMrender(FileName);
   }
   if(e.target.closest('.TBLnodeValue')){
      let NodeName = e.target.closest(".TBLnodeValue").getAttribute('value');
      let WindowId = WNDeventId(e);
      let FileName = TBLlistWindows[WindowId];
      TBLopenNodeWindow(NodeName, FileName);
      return;
   }
   if(e.target.closest('.TBLexport')){
      let WindowId = WNDeventId(e);
      let FileName =  TBLlistWindows[WindowId];
      EXPrender(FileName);
      return;
   }
   if(e.target.closest('.TBLeditListWindow')){
      let WindowId = WNDeventId(e);
      WNDcontentSelector(WindowId).innerHTML = "";
      TBLeditListWindows[WindowId] = true;
      MODELforcePoll('TBLmodel');
   }
   if(e.target.closest('.TBLdoneEditList')){
      let WindowId = WNDeventId(e);
      WNDcontentSelector(WindowId).innerHTML = "";
      delete TBLeditListWindows[WindowId];
      MODELforcePoll('TBLmodel');
   }
   if(e.target.closest('.TBLeditNodeWindow')){
      let WindowId = WNDeventId(e);
      let NodeName = TBLnodeWindows[WindowId].name;
      let FileName = TBLnodeWindows[WindowId].file;
      if(WNDisWindow(WindowId)) TBLchangeDimensions(e.target, NodeName, FileName);
      WNDupdateContent(WindowId, TBLeditNodeHtml(NodeName, FileName) );
      TBLeditColumnsHtml(NodeName,FileName, WindowId);
   }
   if(e.target.closest('.TBLcancelEdit')){
      let WindowId = WNDeventId(e);
      WNDcontentSelector(WindowId).innerHTML = "";
      delete TBLeditList[WindowId];
      MODELforcePoll('TBLmodel');
   }
   if(e.target.closest(".TBLdeleteRow")){
      let Id = DRGidFromRow(e.target);
      e.target.closest('.DRGdragSortListItem').remove();
      if(DRGcallbackDiv[Id]) DRGcallbackDiv[Id](Id);
   }
   if(e.target.closest('.TBLaddColumn')){
      let WindowId = WNDeventId(e);
      TBLaddBlankRow(WindowId);
   }
   if(e.target.closest('.TBLsaveEdit')){
      let WindowId = WNDeventId(e);
      let NodeName = TBLnodeWindows[WindowId].name;
      let FileName = TBLnodeWindows[WindowId].file;
      if(!TBLsaveNodeData(WindowId, NodeName, FileName)) return;
      WNDcontentSelector(WindowId).innerHTML = "";
      delete TBLeditList[WindowId];
      MODELforcePoll('TBLmodel');
   }
   if(e.target.closest('.TBLaddListItem')){
      let WindowId = WNDeventId(e);
      let FileName =  TBLlistWindows[WindowId];
      TBLaddNew(TBLgenerateUniqueName("Table" , FileName), FileName);
   }
   if(e.target.closest('.TBLlistRemove')){
      let Name = e.target.closest('.TBLlistRow').querySelector(".TBLnodeValue").getAttribute('value').trim();
      let WindowId = WNDeventId(e);
      let FileName = TBLlistWindows[WindowId];
      return TBLremoveListItem(Name, FileName);
   }
}

function TBLgenerateUniqueName(Type, FileName){
   let FileNumber = 1;
   let FileData = TBLfileData();
   while (FileData[FileName]["tables"][Type + FileNumber] && FileNumber++ <= 10000) {}
   return Type + FileNumber;
}

function TBLsimulateEditClick(){
   let EditButton = document.querySelector('.TBLeditNodeWindow');
   if (EditButton) EditButton.click();
}

function TBLaddNew(NodeName, FileName){
   let FileData = TBLfileData();
   let NodeData = {};
   NodeData.desc = "";
   FileData[FileName]["tables"][NodeName] = NodeData;
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/db/set', ApiData, function(R){ 
      if(!R.success) { SNCKsnackbar("error", R.error); }
      TBLopenNodeWindow(NodeName, FileName);
      console.warn("after open node window, before poll");
      MODELforcePoll('TBLmodel', TBLsimulateEditClick);
   });
}

function TBLremoveListItem(Name, FileName){
   console.log("remove: " + Name);
   let FileData = TBLfileData();
   delete FileData[FileName]["tables"][Name];
   let ApiData = {};
   ApiData.path = FileName;
   ApiData.component   = TRANScomponentGuid();
   ApiData.content     = JSON.stringify(FileData[FileName]);
   ApiData.development = TRANSdevelopment();
   APIcall('component/db/set', ApiData, function(R){ 
      if(!R.success) { SNCKsnackbar("error", R.error); }
      MODELforcePoll('TBLmodel'); 
   });
}

function TBLchangeDimensions(Target, NodeName, FileName){
   let FileData = TBLfileData();
   let FileContent = FileData[FileName]["tables"][NodeName];
   let Height = 200;
   if(FileContent.columns) Height = 200 + FileContent.columns.length * 32;
   if(Height < 200) Height = 200;
   if(Height > 800) Height = 800;
   if(Target.closest('.WNDwindow')){
      // Table edit window is set to fit contents. The actual table inside it can overflow and has a max height of 600, so our window should 
      // never exceed ~650px.
      Target.closest('.WNDwindow').style.height = "fit-content";
      Target.closest('.WNDwindow').style.maxHeight = "800 px !important";
   }
}