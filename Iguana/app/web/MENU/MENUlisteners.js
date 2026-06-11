function MENUlisteners(e){
   TRANSsaveCurrentModuleCursorPosition();
   if(e.target.classList.contains('MENUaddFile'))        { return ACTcreateFileWindow(MENUitem);      }
   if(e.target.classList.contains('MENUcopy'))           { return ACTcopyFile(MENUitem);              }
   if(e.target.classList.contains('MENUpaste'))          { return ACTpasteFile(MENUitem);             }
   if(e.target.classList.contains('MENUduplicate'))      { return ACTduplicateFile(MENUitem);         }
   if(e.target.classList.contains('MENUdownload'))       { return ACTdownloadFile(MENUitem);         }
   if(e.target.classList.contains('MENUrename'))         { return ACTrename(MENUitem, ACTrenameFile);            }
   if(e.target.classList.contains('MENUsource'))         { return PRJshowSource(MENUitem.path);       }
   if(e.target.classList.contains('MENUdelete'))         { return ACTdeleteFileWindow(MENUitem.path); }
   if(e.target.classList.contains('MENUdeleteDir'))      { return ACTdeleteDirWindow(MENUitem.path);  }
   if(e.target.classList.contains('MENUaddDir'))         { return ACTcreateDirWindow(MENUitem);       }
   if(e.target.classList.contains('MENUrenameDir'))      { return ACTrename(MENUitem, ACTrenameDir);  }
   if(e.target.classList.contains('MENUunlock'))         { return LIBunlock(MENUitem.path, MENUitem.name); }
   if(e.target.classList.contains('MENUlock'))           { return LIBlock(MENUitem);                }
   if(e.target.classList.contains('MENUlibraryRemove'))  { return LIBdeleteModal(MENUitem);           }
   if(e.target.classList.contains('MENUlibraryRename'))  { return ACTrename(MENUitem, LIBrename); }
   if(e.target.classList.contains('MENUlibraryRenameTool'))  { return RNMrender(MENUitem.name, ""); }
   if(e.target.classList.contains('MENUlibraryReset'))   { return MENUlibraryReset(MENUitem); }
   if(e.target.classList.contains('MENUviewChanges'))    { return MENUviewChanges(MENUitem);     }
   if(e.target.classList.contains('MENUlibraryPublish')) { return MENUlibraryPublish(MENUitem);       }
   if(e.target.classList.contains('MENUlibraryUpdate'))  { return MENUlibraryUpdate(MENUitem);        }
   if(e.target.classList.contains('MENUlibraryInitialize')){return MENUlibraryInitialize(MENUitem);   }
   if(e.target.classList.contains('MENUlibraryOutOfSync')){return ERSscreen(MENUitem);   }
   if(e.target.classList.contains('MENUviewRepo'))     { return MENUviewLibraryRemote(MENUitem);    }
   if(e.target.classList.contains('MENUaddToCollection')){ return MENUaddToCollectionClicked(MENUitem);}
   if(e.target.classList.contains('MENUseeInCollection')){ return MENUseeInCollectionClicked(MENUitem);}
   if(e.target.classList.contains('MENUresetComponent')) { return MENUconfirmResetCommit();}
   // if(e.target.classList.contains('MENUrootErrors'))     { return PRJrootErrorModal();                }
   if(e.target.classList.contains('MENUaddSampleData'))  { return SMPLaddSampleData();          }
   if(e.target.classList.contains('MENUcopySampleContents')){ return TRANSsampleContentClick();}
   if(e.target.classList.contains('MENUcopyProjectContents')){ return TRANSprojectContentClick();}
   MENUremoveTooltip();
}

function MENUviewLibraryRemote(MENUitem) { 
   LPRrender(MENUitem.path);
}

function MENUconfirmResetCommit(){
   CONFIRMaction(`Are you sure you want to reset to the <b>latest commit</b>?<br/><br/><b>Warning:</b> This action cannot be reversed.`, function() {
      MENUresetClicked();
   }, "CONFIRM");
}

function MENUlibraryReset(Item){
   CONFIRMaction(`Are you sure you want to reset this library to the latest commit?<br/><br/><b>Warning:</b> This action cannot be reversed.`, function() {
      MENUresetClicked(Item);
   }, "CONFIRM");
}

function MENUresetClicked(Item){
   let Path = Item?.path;
   let PathArgs = Path ? ' -C "' + Path + '"' : "";
   TGITopenBranchTerminal(function(){
      TERMexecuteCommand(['git' + PathArgs + ' reset --hard', 'git' + PathArgs + ' clean -dffx'], TGITinitializeLibraries);
   });
}

function MENUaddToCollectionClicked(MENUitem) {
   const LibraryList = TRGTlibraryList();
   const Data        = {name: MENUitem.name, type: "library", git: LibraryList[MENUitem.name].url};
   ADCrender(Data);
   MENUremoveTooltip();
}

function MENUseeInCollectionClicked(MENUitem) {
   const LibraryList = TRGTlibraryList();
   const Collection  = TRGTisLibraryInCollection(LibraryList[MENUitem.name].url);
   EDCinit(Collection);
}

function MENUviewChanges(CurrentTreeNode){
   const DiffCard = POPpopoutOpen({
      title     : 'Changes to this file from previous git commit',
      content   : TGITdiffViewContainer,
      Zindex    : 1005,
      className : 'TGITgitWindow',
      esc_close : true,
      callback : TRANSsetCode
   });
   const libFile = MENUresolveNodeLibrary(CurrentTreeNode.path);
   libFile ? TGITfileDiff(CurrentTreeNode.name, libFile) : TGITfileDiff(CurrentTreeNode.path);
   document.querySelector('.TGITcancelRevert').addEventListener('click', function() { POPpopoutClose(DiffCard); });
   document.querySelector('.TGITfileRevert')  .addEventListener('click', function() { ACTrevertFileConfirm(DiffCard, CurrentTreeNode); });
   MENUremoveTooltip();
}

function MENUlibraryPublish(CurrentTreeNode) {
   let RelativePath    = CurrentTreeNode.path;
   return PBLrenderWindow(RelativePath);
}

function MENUlibraryUpdate(CurrentTreeNode){
   console.warn('Pull library changes with the terminal prompt');
   TGITopenBranchTerminal(function() {TERMexecuteCommand([`git -C "${CurrentTreeNode.path}" pull`]);});
}

function MENUlibraryInitialize(CurrentTreeNode){
   let LibraryName = CurrentTreeNode.name;
   let ApiData = {};
   ApiData.component = TRANScomponentGuid();
   ApiData.module_name = LibraryName;
   ApiData.development = TRANSdevelopment();
   APIcall("component/library/initialize", ApiData, function(R){
      if(!R.success) return SNCKsnackbar('error', 'Could not initialize library: ' + R.error);
      SNCKsnackbar('success', 'Initialized library in folder: ' + LibraryName);
      MODELforcePoll('TRANSgitModel');
      MODELforcePoll('TRANSmodel');
   });
}

function MENUresolveNodeLibrary(path) {
   if(!path) { return; }
   const root = path.split(FILsep())[0]
   const libraries = TRGTlibraryList();
   for (const lib of Object.keys(libraries)) { if(root === lib) { return root; } }
}
