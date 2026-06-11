function PRJlocateDropTarget(){
   let SelectedItem = document.querySelector('.PRJselected');                      
   let SelectedFolder;
   if(SelectedItem){
      SelectedFolder = SelectedItem.closest('.PRJdrop').querySelector('.PRJrow'); 
   }
   let RowArray = [0];
   if(SelectedFolder){
      RowArray = PRJgetRowArray(SelectedFolder);
   }
   let DirTree = PRJdirTree();
   return PRJreturnTreeNode(DirTree, RowArray)
}

function PRJcreateFile(e){
   let T = PRJlocateDropTarget();  if(!T) return
   if(PRJlocked(T.filepath) || PRJinsideLockedLib(T.filepath)) return SNCKsnackbar('error', 'Library needs to be unlocked to create a file inside it.');
   ACTcreateFileWindow(T);
}

function PRJcreateDir(e){
   let T = PRJlocateDropTarget(e);  if(!T) return
   if(PRJlocked(T.filepath) || PRJinsideLockedLib(T.filepath)) return SNCKsnackbar('error', 'Library needs to be unlocked to create a directory inside it.')
   ACTcreateDirWindow(T)
}
