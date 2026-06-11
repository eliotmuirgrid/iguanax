function PRJprojectTreeClick(e){
   let T = PRJgetEventDetails(e);
   if(T.off) { // don't think this ever gets called
      TRANSsetCode();
      return MENUremoveTooltip();
   }
   if( e.target.closest('.PRJlibLink'))       return;
   if( T.targetClass('PRJfolderMenu'))        return PRJonMenuClick(T);
   if( T.targetClass('PRJfileMenu'))          return PRJonMenuClick(T);
   if( T.targetClass('PRJlibraryUpdate'))     return PRJonMenuClick(T);
   if( T.targetClass('PRJlibraryWarning'))    return PRJonMenuClick(T);
   if( T.parentClass('.POPpopover') )         return;
   if( T.rowClass   ('PRJaction-closedDir'))  return PRJonClosedDirClick(T.treeNode);
   if( T.rowClass   ('PRJaction-openDir'))    return PRJonOpenDirClick(T.treeNode);
   if( T.rowClass   ('PRJaction-file'))       return PRJonFileClick(T.treeNode.path);
}

function PRJgetRowArray(targetRow){
   let RowArray = [];
   while(targetRow.parentElement.closest('li')) {
      targetRow = targetRow.parentElement.closest('li');
      RowArray.unshift(PRJrowIndex(targetRow));
   }
   return RowArray;
}

function PRJrowIndex(targetRow){ return targetRow ? Array.from(targetRow.closest('ul').children).indexOf(targetRow) : -1; }

function PRJgetEventDetails(e){
   e.stopPropagation();
   let Details = {};
   Details.row = e.target.closest('.PRJrow');
   if(!Details.row) { Details.off = true; return Details; }
   Details.target = e.target;
   Details.targetClass = function(ClassName){ return Details.target.classList.contains(ClassName); }
   Details.rowClass = function(ClassName){ return Details.row.classList.contains(ClassName); }
   Details.parentClass = function(ClassName){ return Details.target.closest(ClassName); }
   let RowArray = PRJgetRowArray(Details.row);
   Details.rowArray = RowArray;
   let DirTree = PRJdirTree();
   Details.treeNode = PRJreturnTreeNode(DirTree, RowArray);
   return Details;
}

function PRJonClosedDirClick(CurrentTreeNode){
   CurrentTreeNode.state = "open";
   MENUremoveTooltip();
   MODELforcePoll('TRANSmodel');
   TRANSsetCode();
}

function PRJonOpenDirClick(CurrentTreeNode) {
   CurrentTreeNode.state = "closed";
   MENUremoveTooltip();
   MODELforcePoll('TRANSmodel');
   TRANSsetCode();
}

function PRJonFileClick(FilePath) {
   MENUremoveTooltip();
   const FileCheck = FilePath.toLowerCase();
   if(FileCheck.endsWith(".vmd"))       { EDIopenListWindow(FilePath); }
   else if(FileCheck.endsWith(".vdb"))  { TBLopenListWindow(FilePath); }
   else if(FileCheck === "config.json") { FLDview(FilePath); }
   else if(FileCheck.endsWith(".help")) {
      HLPlistAll(HLPnameResolve(FilePath));
      HLPopenListWindow(FilePath);
   }
   else if(FileCheck.endsWith(".pdf")) { ACTviewPdfFile(FilePath); }
   else                                { DBMcheckSqlite(FilePath) }
}

function PRJonMenuClick(T){
   if(T.treeNode.path === MENUitem.path && MENUgetTooltip()) {
      TRANSsetCode();
      return MENUremoveTooltip(); 
   }// for every second click on menu ellipsis
   MENUrender(T.treeNode, T.target);
}

function PRJgetRelativePath(LibraryPath){ return LibraryPath.replace(TRANSsourceDirectory, ""); }

function PRJhandleRightClick(e){
   MENUremoveTooltip()
   if(e.target.closest('.PRJrow')){
      e.preventDefault()
      let T = PRJgetEventDetails(e);
      if(!PRJneedsMenu(T.treeNode)) return
      MENUrender(T.treeNode, T.target);
      if(document.querySelector('.MENUtooltip')){ 
         MENUpositionVertically  (e.clientY - 14)
         MENUpositionHorizontally(e.clientX)
      }  
   }
}
