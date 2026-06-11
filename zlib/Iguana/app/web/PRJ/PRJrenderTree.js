let PRJselectedFileRendered = "";
let PRJdirTreeRendered = [];
let PRJlibrariesRendered = {};
let PRJsampleIndexRendered = "";
let PRJgitModulesWarning = "";
let PRJgitStatusRendered = {};
let PRJaheadBehindRendered = {};
let PRJconflictsData = {};

function PRJassignAndRenderTree() {
   if(ACTrenameInProgress) return;
   let DirTree = PRJdirTree();
   let TreeChanged         = !DEEPequal(DirTree,   PRJdirTreeRendered);
   let LibraryList         = TRGTlibraryList();
   let LibraryChanged      = !DEEPequal(LibraryList, PRJlibrariesRendered);
   let Changes             = TRGTuncommittedChanges();
   let GitStatusChanged    = !DEEPequal(Changes, PRJgitStatusRendered);
   let AheadBehind         = TRGTaheadBehind();
   let AheadBehindChanged  = !DEEPequal(AheadBehind, PRJaheadBehindRendered);
   let Conflicts           = PRJconflicts();
   let ConflictsChanged    = !DEEPequal(Conflicts, PRJconflictsData);
   let SampleIndexChanged  = PRJsampleIndexRendered !== SMPLindex();
   let ModulesWarning      = PRJgitModulesWarning !== TRANSgitModulesWarning();
   let SelectedFileChanged = PRJselectedFileRendered !== TRANSselectedFile();
   if(TreeChanged || LibraryChanged || GitStatusChanged || AheadBehindChanged || SampleIndexChanged || SelectedFileChanged || ConflictsChanged || ModulesWarning) {
      MENUupdateTooltip();
      let div = document.querySelector('.PRJfileTree');
      if(!div) return;
      div.innerHTML = PRJrenderProjectTree(DirTree);
      PRJdirTreeRendered      = JSON.parse(JSON.stringify(DirTree));
      PRJlibrariesRendered    = JSON.parse(JSON.stringify(LibraryList));
      PRJgitStatusRendered    = JSON.parse(JSON.stringify(Changes));
      PRJaheadBehindRendered  = JSON.parse(JSON.stringify(AheadBehind));
      PRJconflictsData        = JSON.parse(JSON.stringify(Conflicts));
      PRJsampleIndexRendered  = SMPLindex();
      PRJgitModulesWarning = TRANSgitModulesWarning();
      PRJselectedFileRendered = TRANSselectedFile();
   }
}

function PRJmapGitStatus(Item){
   if (LIBisLibraryRoot(Item.filepath)){ return "-library"; }
   let Status = TRGTstatus(Item.path);
   switch(Status) {
      case "M": return "-modified";
      case "R": return "-renamed";
      case "A": return "-untracked";
      case "??": return "-untracked";
      default : return "";
   }
}

function PRJcontainsChanges(Lib, DirPath){
   let Changes = TRGTuncommittedChanges();
   for(file of Changes[Lib]){
      let Path = file?.path || "";
      if(Path.startsWith(DirPath)) return true;
   }
   return false;
}

function PRJdirChanges(DirPath){
   let Changes = TRGTuncommittedChanges();
   for(Lib in Changes){
      if(PRJcontainsChanges(Lib, DirPath)) return true;
   }
   return false;
}

function PRJrenderProjectTree(DirTree) {
   let Tree = [];
   DirTree.map(function(Item, Index) {
      Item.type === 'dir' ? Tree.push(PRJdirHtml(Item)) : Tree.push(PRJfileHtml(Item, Index));
   });
   return Tree.join('');
}

function PRJlockLibFlag(isLibRoot, Path) {
   if(!isLibRoot) { return ''; }
   return LIBlibraryUnlocked(Path) ? '-unlocked' : '-locked';
}

function PRJlocked(Path) {
   if(!LIBisLibraryRoot(Path) || !TRANSdevelopment()){ return false }
   return !LIBlibraryUnlocked(Path);
}

function PRJinsideLockedLib(Path){ return LIBisInLibrary(Path) && !LIBlibraryUnlocked(Path)}

function PRJdirHtml(Item) {
   const isOpen     = Item.state === 'open';
   const dirIcon    = isOpen ? 'PRJicon-openDir' : 'PRJicon-closedDir';
   const lockIcon   = PRJlockLibFlag(LIBisLibraryRoot(Item.filepath), Item.path);
   const selected   = TRANSselectedFile() == Item.path ? ' PRJfolderSelected' : '';
   const drag       = !PRJinsideLockedLib(Item.filepath);
   const action     = isOpen ? 'PRJaction-openDir' : 'PRJaction-closedDir';
   const changes    = PRJdirChanges(Item.path);
   let Conflict      = PRJdirConflictExists(Item.path);
   let ConflictClass = Conflict ? " PRJconflictExistsFolder" : "";
   return /*html*/`
      <li class='PRJdrop'>
         <div class='${dirIcon + PRJmapGitStatus(Item) + lockIcon}${ConflictClass} ${selected} ${action} ${changes ? 'PRJchanges ' : ''}PRJrow' draggable=${drag}>${PRJfolderHtml(Item)}</div>
         ${isOpen ? `<ul>${PRJrenderProjectTree(Item.contents)}</ul>` : ''}
      </li>`;
}

function PRJfileHtml(Item, Index) {
   const path         = Item['filepath'];
   const selected     = TRANSselectedFile() === path ? ' PRJselected' : '';
   let drag           = !PRJinsideLockedLib(path) && Item.path != 'main.lua' && Item.path != ".gitmodules" && TRANSdevelopment();
   let fileHtml       = [`<li>`];
   let Conflict       = PRJconflictExists(Item.path);
   let ConflictClass  = Conflict ? " PRJconflictExists" : "";
   let ModulesWarning = Item.path == ".gitmodules" && TRANSgitModulesWarning();
   let selectedSample = SMPLindex() > 0 && path.indexOf(SMPLrootFolder()) > 0 && SMPLindex() - 1 === Index ? ' PRJselectedSample' : '';
   if(PRJgetRelativePath(path) === 'config.json') { fileHtml.push(PRJcustomFieldsConfigHtml(Item, selected, selectedSample)); }
   else { fileHtml.push(/*html*/`
   <div class='PRJicon-file${PRJmapGitStatus(Item)}${ConflictClass} PRJaction-file${selected}${selectedSample} PRJrow ${
      path.indexOf(".vmd")  !== -1 ? "EDIvmdFile" : 
      path.indexOf(".vdb")  !== -1 ? "TBLdatabase" : 
      path.indexOf(".help") !== -1 ? "HLPicon" : 
      ""}' draggable=${drag}>
   <div class="PRJitemName">${ESChtmlEscape(Item.name)}</div>`); }
   if(ModulesWarning) { fileHtml.push(`<div class='PRJitemTag PRJmodulesWarning' title="${ModulesWarning}"></div>`); }
   if(PRJneedsMenu(Item)) { fileHtml.push(`<div class='PRJfileMenu' title='File options'></div></div></li>`); }
   return fileHtml.join('');
}

function PRJneedsMenu(Item){
   const Filepath      = Item.filepath;
   const IsLibraryFile = !LIBisInLibrary(Filepath);
   const NeedsMenu     = LIBlibraryUnlocked(Filepath) || IsLibraryFile || LIBisLibraryRoot(Filepath);
   return NeedsMenu;
}

function PRJconflictExists(Path){
   let Data = PRJconflicts();
   return Data[Path];
}

function PRJdirConflictExists(Path){
   let Data = PRJconflicts();
   for(let i in Data){
      if(i.startsWith(Path)) return true;
   }
   return false;
}

function PRJcustomFieldsConfigHtml(Item, Selected, SelectedSample){
   return /*html*/`<div class='PRJicon-config${PRJmapGitStatus(Item)} PRJaction-file${Selected}${SelectedSample} PRJrow'>${ESChtmlEscape(Item.name)}`;
}

function PRJfolderHtml(Item){
   COL_TRC(Item);
   let FilePath = Item.filepath;
   const IsLibrary = LIBisLibraryRoot(FilePath);
   return /*html*/`
   ${PRJfolderName(Item.name)}
   ${PRJwarningSymbol(IsLibrary, FilePath)}
   ${PRJupdateLibrarySymbol(IsLibrary, FilePath)}
   ${PRJexternalLink(IsLibrary, FilePath)}
   ${PRJshowMenu(IsLibrary, Item)}
   `;
}

function PRJfolderName(Name){
   return /*html*/`<div class="PRJitemName">${ESChtmlEscape(Name)}</div>`;
}

function PRJwarningSymbol(IsLibrary, FilePath){
   const Development = TRANSdevelopment();
   let Initialized = TRGTinitialized(FilePath);
   let ReposSynchronized = TRGTreposInSync(FilePath);
   if(!Initialized) ReposSynchronized = true;
   let RelativePath = LIBremoveBeginning(FilePath, TRANSsourceDirectory);
   let UpstreamSet = LIBlibraryUpstreamSet(RelativePath);
   let Warning = IsLibrary && (!Initialized || !ReposSynchronized || !UpstreamSet) && Development;
   let Html = "";
   if(Warning) Html = /*html*/`<div class="PRJitemTag PRJlibraryWarning"></div>`;
   return Html;
}

function PRJupdateLibrarySymbol(IsLibrary, FilePath){
   let Html = "";
   let Behind = TRGTbehind(FilePath) && IsLibrary;
   if(Behind) Html = /*html*/`<div class="PRJitemTag PRJlibraryUpdate"></div>`;
   return Html;
}

function PRJshowMenu(IsLibrary, Item){
   let ShowMenu = false;
   let InLibrary =  LIBisInLibrary(Item.filepath);
   let Locked = !LIBlibraryUnlocked(InLibrary);
   const Development = TRANSdevelopment();
   if((!Development && IsLibrary) || Development) { ShowMenu = true; }
   if(InLibrary && Locked && !IsLibrary) ShowMenu = false;
   let NeedsMenu = ShowMenu && PRJneedsMenu(Item);
   let Html = "";
   if(NeedsMenu) Html = /*html*/`<div class='PRJfolderMenu' title="Directory options"></div>`;
   return Html;
}

function PRJexternalLink(IsLibrary, FilePath){
   if(!IsLibrary) return "";
   let Git = LIBlibraryGitData(FilePath);
   let Set  = Git?.set;
   if(!Set) return "";
   let Web  = Git?.web || "";
   return /*html*/`
   <a class="PRJlibLink BUTTONdefault" href="${Web}" title="${Set}" target="_blank">
      <div class="PRJlibLinkIcon"></div>
   </a>`;
}