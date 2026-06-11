function PRJprojectPaneComponentBlock() {
   let Hide = TRANScomponentHide();
   return /*html*/`
   <div class='PRJresizeControl'>
      <div class="PRJresizeShadow"></div>
   </div>
   <div class="PRJheaderTitle">
      <div class="PRJheaderTitleText">
         Translator
      </div>
   </div>   
   ${PRJdrawerHtml()}
   <div class="PRJtabsAndButtons${Hide}"> 
      <div class="PRJtabsSection">
         <div class="PRJtab PRJfiles PRJtabActive" title="View Project Files">FILES</div>
         <div class="PRJtab PRJsearch" title="Open Search and Replace">SEARCH</div>
      </div>
      <div class ="PRJbuttonGroup">
         <select class="PRJnewSelect FORMselectBox" title="New">
            <option value="null" disabled selected>New +</option>
            <option value="file">File</option>
            <option value="folder">Folder</option>
            <option value="library">Library</option>
            <option value="sample">Sample Data</option> 
         </select>
      </div>
   </div>
   <div class="PRJprojectTreeBody">
      <div class='PRJprojectTreeScroller'>
         <ul class='PRJfileTree'></ul>
      </div>
   </div>
   <div class="PRJglobalSearch"></div>
   <div class="PRJdrawers${Hide}">
      ${GITTdrawerHtml()}
   </div>
   ${PRJsourcesReceiversHtml()}
   `;
}

function PRJclearModels(){
   PRJselectedFileRendered = "";
   PRJdirTreeRendered = [];
   PRJlibrariesRendered = {};
   PRJsampleIndexRendered = "";
   PRJgitStatusRendered = {};
   PRJgitModulesWarning = "";
}

function PRJreturnTreeNode(DirTree, IndexArray){
   if (!IndexArray.length)      { return DirTree }
   if (IndexArray.length === 1) { return PRJreturnTreeNode(DirTree[IndexArray.shift()], IndexArray); }
   return PRJreturnTreeNode(DirTree[IndexArray.shift()].contents, IndexArray);
}

function PRJfindOpenDirsInTree(DirTree, OpenDirs){
   DirTree.map(function(Item){ // apply this function to each item in DirTree
      if (Item.type === "dir" && Item.state === "open"){
         OpenDirs.push(Item.filepath);
         PRJfindOpenDirsInTree(Item.contents, OpenDirs);
      }
   });
}

function PRJgetFullPath(){
   let CurrentFileInput = document.querySelector(".TRANSbottomBarCurrentFile");
   if(!CurrentFileInput) return;
   let Path = TRANSfullPathModule();
   if(CurrentFileInput.value !== Path) CurrentFileInput.value = Path;
}

function PRJexpandSamplesDir() {
   let DirTree = PRJdirTree();
   const RootChildren = DirTree[0].contents;
   for (let i = 0; i < RootChildren.length; i++) {
      if (RootChildren[i].name == "Samples") return PRJonClosedDirClick( PRJreturnTreeNode(DirTree, [0, i]) );
   }
}
