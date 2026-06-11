let MENUitem = {};
let MENUrow = {};

function MENUrender(Item, Target) {
   TRANSsaveCurrentModuleCursorPosition();
   MENUremoveTooltip(); 
   MENUitem = Item;
   MENUrow  = Target.closest('.PRJrow');
   let html = MENUhtml(Item);
   if(!html || !html.trim()) {return;}
   MENUrenderTooltip(html);
   MENUpositionVertically(Target.getBoundingClientRect().top);
   MENUrow.classList.add('PRJmenuBox');
}

function MENUremoveTooltip(){
   let tooltip = MENUgetTooltip();
   if(tooltip) {
      tooltip.remove();
      MENUrow.classList.remove('PRJmenuBox');
      MENUitem = {};
      MENUrow = {};
   }
}

function MENUgetTooltip() { return document.querySelector('.MENUtooltip'); }

function MENUupdateTooltip() {
   let tooltip = MENUgetTooltip();
   tooltip && MENUitem ? MENUrenderTooltip(MENUhtml(MENUitem)) : MENUremoveTooltip();
}

function MENUrenderTooltip(Content){
   let tooltip = MENUgetTooltip();
   if(tooltip) { return tooltip.innerHTML = MENUpopoutHtml(Content) }
   const newNode     = document.createElement('div');
   newNode.className = "MENUtooltip";
   newNode.innerHTML = MENUpopoutHtml(Content);
   document.querySelector('.PRJprojectTreeScroller').appendChild(newNode);
}

function MENUpositionVertically(top){
   let Height = document.querySelector('.TRANSswitchDiv').offsetHeight;
   Height    += document.querySelector('.PRJtabsSection').offsetHeight;
   Height    += document.querySelector('.PRJheaderTitle').offsetHeight;
   Height    -= document.querySelector('.PRJprojectTreeScroller').scrollTop;
   document.querySelector('.MENUtooltip').style.top = top - Height + "px";
}

function MENUpositionHorizontally(Left){
   let Nav       = document.querySelector('.CORheaderNav')
   let Menu      = document.querySelector('.MENUtooltip')
   let Container = document.querySelector('.PRJprojectTreeScroller')
   let Offset = Nav.offsetWidth + Container.offsetWidth - Left - Menu.offsetWidth;
   if (Offset < 0) { Offset = 5; }
   Menu.style.right = Offset + "px"
}

function MENUhtml(Item) {
   if(Item.path === 'Samples')         { return MENUsampleHtml(); }
   if(LIBisLibraryRoot(Item.filepath)) { return MENUlibraryRenderedHtml(Item); }
   let InLibrary =  LIBisInLibrary(Item.filepath);
   let Locked = !LIBlibraryUnlocked(InLibrary);
   if(Item.type === 'dir')             { 
      if(InLibrary) return MENUlibraryFolder(Locked);
      return  MENUfolderHtml(Item);
   }
   if(Item.type === 'file')            { 
      return MENUfileHtml(Item, InLibrary && Locked); 
   }
}

function MENUfileHtml(Item, InLockedLibrary){
   return /*html*/`   
   ${ PRJisStructuredFile(Item.path)         ? /*html*/`<p class='MENUsource'>     Show Source </p>` : ''}
   ${(PRJmapGitStatus(Item) === "-modified") ? /*html*/`<p class='MENUviewChanges'>View Changes</p>` : ''}
   <p class='MENUcopy'     >Copy to Clipboard</p>
   <p class='MENUdownload '>Download</p>
   ${TRANSdevelopment() && !InLockedLibrary? "<p class='MENUduplicate'>Duplicate</p>" : "" }
   ${PRJisImportantFile(Item.path) || !TRANSdevelopment() ? "" : /*html*/`
   ${!InLockedLibrary ? "<p class='MENUrename'>Rename File</p>" : ""}
   ${!InLockedLibrary ? "<p class='MENUdelete'>Delete File</p>" : ""}`}`;
}

function MENUpopoutHtml(Content){
   return /*html*/`
   <div class='MENUtooltipContents'>${Content}</div>`;
}

function MENUsampleHtml(){
   return /*html*/`
   <p class='MENUaddSampleData'>Add Sample Data</p>
   <p class='MENUcopySampleContents'>Copy Sample Data</p>
   ${ ACTcopy.Path ? /*html*/`<p class='MENUpaste'>Paste from Clipboard</p>` : ''}
   <p class='MENUdeleteDir'    >Delete Folder</p>`;
}

function MENUlibraryRenderedHtml(Item){ 
   let LibraryList = TRGTlibraryList();
   if (LibraryList.hasOwnProperty(Item.name)) { return MENUlibraryHtml(Item.name, Item.path, Item.filepath); } 
}

function MENUlibraryHtml(LibName, Path, LibPath) {
   const Unlocked    = LIBlibraryUnlocked(Path);
   const UpstreamSet = LIBlibraryUpstreamSet(Path);
   const LibUrl      = TRGTlibraryList()[LibName].url;
   if(TRANSdevelopment() && !TRANSplainFolder()) {
      return MENUlibraryDevelopmentHtml(Unlocked, UpstreamSet, LibUrl, LibPath);
   } else {
      return MENUlibraryNonDevelopmentHtml(LibUrl, UpstreamSet);
   }
}

function MENUlibraryDevelopmentHtml(Unlocked, UpstreamSet, LibUrl, LibPath){
   let Behind = TRGTbehind(LibPath);
   let Initialized = TRGTinitialized(LibPath);
   let ReposSynchronized = TRGTreposInSync(LibPath);
   if(!Initialized) ReposSynchronized = true;
   let Write = true; // however we want to check for writable 
   return /*html*/`
   ${!ReposSynchronized       ? /*html*/`<p class='MENUlibraryOutOfSync MENUwarning'>Upstream Error</p>` : ''}
   ${!Initialized             ? /*html*/`<p class='MENUlibraryInitialize MENUwarning'>Initialize Library</p>` : ''}
   ${Unlocked && UpstreamSet  ? /*html*/`<p class='MENUlock'>Lock Library</p>`                            : ''}
   ${Write && !Unlocked       ? /*html*/`<p class='MENUunlock'>Unlock Library</p>`                        : ''}
   ${UpstreamSet ? `<p class='MENUlibraryUpdate${Behind ? " MENUwarning" : ""}'>Update Library</p>` : ''}
   ${MENUlibraryNonDevelopmentHtml(LibUrl, UpstreamSet)}
   ${Unlocked                 ? /*html*/`<p class='MENUaddFile'>New File</p>`                  : ''}
   ${Unlocked                 ? /*html*/`<p class='MENUaddDir' >New Folder</p>`                : ''}
   ${Unlocked && ACTcopy.Path ? /*html*/`<p class='MENUpaste'  >Paste from Clipboard</p>`         : ''}
   ${Unlocked                 ? /*html*/`<p class='MENUlibraryRename'>Rename Library</p>`         : ''}
   ${Unlocked                 ? /*html*/`<p class='MENUlibraryRenameTool'>Rename Inner Files</p>`: ''}
   ${Unlocked                 ? /*html*/`<p class='MENUlibraryReset' >Reset to Latest Commit</p>` : ''}
   <p class='MENUlibraryRemove'>Delete Library</p>`;
}

function MENUlibraryNonDevelopmentHtml(LibUrl, UpstreamSet){
   let WarningClass = UpstreamSet ? "" : " MENUwarning";
   return /*html*/`<p class='MENUviewRepo${WarningClass}'>Configure Repository</p>
   ${MENUcollectionHtml(LibUrl)}`;
}

function MENUcollectionHtml(LibUrl){
   if(LibUrl === "") return "";
   if(TRGTisLibraryInCollection(LibUrl)) return /*html*/`<p class='MENUseeInCollection' >See in Collection</p>`;
   return                                       /*html*/`<p class='MENUaddToCollection' >Add to Collection</p>`;
}

function MENUfolderHtml(Item){
   let ItemRoot = Item['project_root'];
   let PlainFolder = TRANSplainFolder();
   let ResetOption = TRGTgitChanges() && ItemRoot;
   return /*html*/`
   <p class='MENUaddFile'>New File</p>
   <p class='MENUaddDir' >New Folder</p>
   ${ItemRoot ?     /*html*/`<p class='MENUcopyProjectContents'>Copy Source Code</p>` : ''}
   ${ResetOption  ? /*html*/`<p class='MENUresetComponent'>Reset to Latest Commit</p>`: ''}
   ${ACTcopy.Path ? /*html*/`<p class='MENUpaste'         >Paste from Clipboard</p>`  : ''}
   ${!ItemRoot    ? /*html*/`<p class='MENUrenameDir'     >Rename Folder</p>`         : ''}
   ${!ItemRoot && !PlainFolder ? /*html*/`<p class='MENUlibraryPublish'>New Library</p>` : ''}
   ${!ItemRoot    ? /*html*/`<p class='MENUdeleteDir'     >Delete Folder</p>`         : ''}`;
}

function MENUlibraryFolder(Locked){
   if(Locked) return "";
   return /*html*/`
      <p class='MENUaddFile'>New File</p>
      <p class='MENUaddDir'>New Folder</p>
      ${ ACTcopy.Path ? /*html*/`<p class='MENUpaste'>Paste from Clipboard</p>` : ''}
      <p class='MENUrenameDir'>Rename Folder</p>
      <p class='MENUdeleteDir'>Delete Folder</p>`;
}