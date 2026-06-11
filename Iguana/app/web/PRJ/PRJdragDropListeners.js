let PRJfolderOpenTimeout;

function PRJhandleDragStart(e){
   const T = PRJgetEventDetails(e).treeNode;
   if(!T) return;
   if(PRJinsideLockedLib(T.filepath)){ return }
   e.target.classList.add('PRJdragging')
   ACTsourceName = T.name;
   ACTsourcePath = T.path;
   ACTsourceDir = T.type === "dir";
}

function PRJhandleDragEnd (e){ e.target.classList.remove('PRJdragging')}
function PRJhandleDragOver(e){ e.preventDefault()}

function PRJhandleDrop(e){
   clearTimeout(PRJfolderOpenTimeout);
   for (let DropZone of document.querySelectorAll('.PRJdropZone')) DropZone.classList.remove('PRJdropZone');
   if(!e.target.closest('.PRJdrop')) return;
   let RowArray = PRJgetRowArray(e.target.closest('.PRJdrop').querySelector('.PRJrow'));
   let DirTree = PRJdirTree();
   let T = PRJreturnTreeNode(DirTree, RowArray);
   let Path = T.filepath;
   if(PRJlocked(Path) || PRJinsideLockedLib(Path)) { return SNCKsnackbar('error', 'Library needs to be unlocked to move files into it.')};
   ACTdestination = T.path;
   ACTmove(ACTsourcePath, ACTsourceName, ACTdestination, ACTsourceDir);
}

function PRJopenFolder(Folder){
   Folder.state = "open"; 
   MODELforcePoll('TRANSmodel');
}

function PRJhandleDragEnter(e){
   clearTimeout(PRJfolderOpenTimeout);
   let DropZone = document.querySelector('.PRJdropZone');
   if(DropZone) DropZone.classList.remove('PRJdropZone');
   const DropRow = e.target.closest('.PRJdrop');
   if(!DropRow) return;
   DropRow.classList.add('PRJdropZone');
   const ClosedDir = e.target.closest('.PRJaction-closedDir');
   let T = PRJgetEventDetails(e).treeNode;
   if(ClosedDir){ 
      if(T.path.includes(ACTsourcePath) || PRJinsideLockedLib(T.filepath)) return;
      PRJfolderOpenTimeout = setTimeout(function() { PRJopenFolder(PRJgetEventDetails(e).treeNode)}, 500);
   }
}