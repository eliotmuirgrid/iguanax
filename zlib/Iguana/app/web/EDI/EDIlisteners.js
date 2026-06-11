// a lot of this could be refactored
// - could determine early on if the click is in a list window or a node window

function EDIlisteners(e){
   let WindowId = WNDeventId(e);
   let FileName = EDIlistWindows[WindowId];
   if(!FileName) FileName = EDInodeWindows[WindowId]?.file;
   if(!FileName) FileName = EDImatchingWindows[WindowId];
   if(e.target.closest('.EDIimport')){
      IMPrender(FileName);
   }
   if(e.target.closest(".EDInodeValue")) {
      let NodeName = e.target.closest(".EDInodeValue").getAttribute('value');
      EDIopenNodeWindow(NodeName, FileName);
      return;
   }
   if(e.target.closest(".EDImatchingOrderLink") || e.target.closest(".EDImessageWarning")){
      EDIopenMatchingWindow(FileName);
      return;
   }
   if(e.target.classList.contains("EDIdoneEditList")){
      WNDcontentSelector(WindowId).innerHTML = "";
      delete EDIeditListWindows[WindowId];
      MODELforcePoll('EDImodel');
   }
   if(e.target.classList.contains("EDIdeleteListItem")){
      let Name = e.target.closest('.EDIlistRow').querySelector(".EDInodeValue").getAttribute('value').trim();
      return EDIremoveListItem(Name, FileName);
   }
   if(e.target.closest('.EDImatchingRulesNodeTable')){
      EDIopenMatchingWindow(FileName);
      return;
   }
   if(e.target.closest(".EDIaddListItem")){
      if(e.target.classList.contains("EDIaddmessage"))      { EDIaddNew(EDIgenerateUniqueName("Message"     , FileName), "message",      FileName);}
      if(e.target.classList.contains("EDIaddsegment_group")){ EDIaddNew(EDIgenerateUniqueName("SegmentGroup", FileName), "segment_group",FileName);}
      if(e.target.classList.contains("EDIaddsegment"))      { EDIaddNew(EDIgenerateUniqueName("Segment"     , FileName), "segment",      FileName);}
      if(e.target.classList.contains("EDIaddcomposite"))    { EDIaddNew(EDIgenerateUniqueName("Composite"   , FileName), "composite",    FileName);}
      return;
   }
   if(e.target.closest(".EDIdeleteRow")){
      let Id = DRGidFromRow(e.target);
      e.target.closest('.DRGdragSortListItem').remove();
      if(DRGcallbackDiv[Id]) DRGcallbackDiv[Id](Id);
   }
   if(e.target.closest(".EDIaddRowMatching")){
      EDIaddBlankMatchingRow(FileName, WindowId);
   }
   if(e.target.closest(".EDIaddChild")){
      let NodeName = EDInodeWindows[WindowId].name;
      EDIaddBlankRow(FileName,NodeName, WindowId);
   }
   if(e.target.classList.contains("EDIcancelMatching")){
      WNDcontentSelector(WindowId).innerHTML = "";
      delete EDIeditList[WindowId];
      MODELforcePoll('EDImodel');
   }
   if(e.target.classList.contains("EDIsaveMatching")){
      EDIsaveMatchingData(FileName, WindowId);
      WNDcontentSelector(WindowId).innerHTML = "";
      delete EDIeditList[WindowId];
   }
   if(e.target.classList.contains("EDIcancelEdit")){
      WNDcontentSelector(WindowId).innerHTML = "";
      delete EDIeditList[WindowId];
      MODELforcePoll('EDImodel');
   }
   if(e.target.classList.contains("EDIsaveEdit")){
      let NodeName = EDInodeWindows[WindowId].name;
      if(!EDIsaveData(FileName, NodeName, WindowId)) return;
      WNDcontentSelector(WindowId).innerHTML = "";
      delete EDIeditList[WindowId];
   }
   if(e.target.classList.contains("EDIeditListWindow")) {
      WNDcontentSelector(WindowId).innerHTML = "";
      EDIeditListWindows[WindowId] = true;
      MODELforcePoll('EDImodel');
   }
   if(e.target.classList.contains("EDIeditMatchingWindow")) {
      WNDupdateContent(WindowId, EDIeditMatchingHtml(FileName) );
      EDIeditMatchingRowsHtml(FileName, WindowId);
   }
   if(e.target.classList.contains("EDIeditNodeWindow")) {
      let NodeName = EDInodeWindows[WindowId].name;
      if(WNDisWindow(WindowId)) EDIchangeDimensions(e.target, NodeName, FileName);
      WNDupdateContent(WindowId, EDIeditNodeHtml(NodeName, FileName) );
      EDIeditChildrenHtml(NodeName,FileName, WindowId);
   }
   if(e.target.closest(".EDImessageFolder"))  { 
      EDItoggleTray(e.target.closest(".EDImessageFolder")); 
      EDItoggleTray(e.target.closest('.EDIaccordian').querySelector('.EDImessageFolderContent')); 
      document.querySelector('.EDImessageFolderContent').scrollIntoView()
   }
   if(e.target.closest(".EDIsegmentgroupFolder"))  { 
      EDItoggleTray(e.target.closest(".EDIsegmentgroupFolder")); 
      EDItoggleTray(e.target.closest('.EDIaccordian').querySelector('.EDIsegmentGroupFolderContent')); 
      document.querySelector('.EDIsegmentGroupFolderContent').scrollIntoView()
   }
   if(e.target.closest(".EDIsegmentFolder"))  { 
      EDItoggleTray(e.target.closest(".EDIsegmentFolder")); 
      EDItoggleTray(e.target.closest('.EDIaccordian').querySelector('.EDIsegmentFolderContent')); 
      document.querySelector('.EDIsegmentFolderContent').scrollIntoView()
   }
   if(e.target.closest(".EDIcompositeFolder")){ 
      EDItoggleTray(e.target.closest(".EDIcompositeFolder")); 
      EDItoggleTray(e.target.closest('.EDIaccordian').querySelector('.EDIcompositeFolderContent')); 
      document.querySelector('.EDIcompositeFolderContent').scrollIntoView()
   }
   let Node = e.target.closest('.EDInodeRowDiv');
   if(Node && e.target.closest(".EDInameNode")){
      if(Node.classList.contains("EDIopen")){
         Node.classList.remove("EDIopen");
         Node.classList.add("EDIclosed");
         EDIrowCollapse(Node);
      }
      else if(Node.classList.contains("EDIclosed")){
         Node.classList.remove("EDIclosed");
         Node.classList.add("EDIopen");
         EDIrowExpand(Node, FileName);
         Node.querySelector(':last-child').scrollIntoView();
      }
      return;
   }
   let Row = e.target.closest('.EDIrowDiv');
   if(!Node && Row){
      if(Row.classList.contains("EDIopen")){
         Row.classList.remove("EDIopen");
         Row.classList.add("EDIclosed");
         EDIcloseAllInnerFolders(Row);
      }
      else if(Row.classList.contains("EDIclosed")){
         Row.classList.remove("EDIclosed");
         Row.classList.add("EDIopen");
         Row.querySelector(':last-child').scrollIntoView();
      }
   }
}

function EDIchangeDimensions(Target, NodeName, FileName){
   let FileData = EDIfileData();
   let FileContent = FileData[FileName]["nodes"][NodeName];
   let Height = 240;
   if(FileContent.children) Height = 240 + FileContent.children.length * 32;
   if(Height < 240) Height = 240;
   if(Height > 800) Height = 800;
   if(Target.closest('.WNDwindow')){
      let CurrentHeight = Target.closest('.WNDwindow').style.height;
      if(CurrentHeight) CurrentHeight = parseInt(CurrentHeight,10);
      if(CurrentHeight && CurrentHeight > Height) Height = CurrentHeight;
      Target.closest('.WNDwindow').style.height = Height + 'px';
   }
}

function EDItoggleTray(target){
   if(target.classList.contains("EDItrayOpen")){
      target.classList.add("EDItrayClosed");
      target.classList.remove("EDItrayOpen");
   }
   else if(target.classList.contains("EDItrayClosed")){
      target.classList.add("EDItrayOpen");
      target.classList.remove("EDItrayClosed");
   }
}

function EDIcloseAllInnerFolders(ParentRow){
   let OpenRows = ParentRow.querySelectorAll('.EDIopen');
   for(let i = 0; i < OpenRows.length; i++){
      OpenRows[i].classList.remove("EDIopen");
      OpenRows[i].classList.add("EDIclosed");
   }
}

function EDIgenerateUniqueName(Type, FileName) {
   let FileData = EDIfileData();
   let FileNumber = 1;
   while (FileData[FileName]["nodes"][Type + FileNumber] && FileNumber++ <= 10000) {}
   return Type + FileNumber;
}
