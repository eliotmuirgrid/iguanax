function TRGTtoggleClass(classSelector, className, toggle=false) {
   let Button = document.querySelector(classSelector);
   if(!Button) { return; }
   if( toggle && !Button.classList.contains(className)){ Button.classList.add   (className); }
   if(!toggle &&  Button.classList.contains(className)){ Button.classList.remove(className); }
}

function TRGTupdateTitle(classSelector, Title){
   let Button = document.querySelector(classSelector);
   if(!Button) { return; }
   if(Button.title !== Title) Button.title = Title;
}

function TRGTfilePathToRelativePath(FilePath){
   COL_TRC("FilePath=", FilePath);
   if (!TRANSsourceDirectory){
      COL_TRC("We don't have our source directory yet since");
      return;
   }
   if(!FilePath) {
      let Result = TRANScomponentGuid() + FILsep();
      COL_TRC("Result=", Result);
      return Result;
   }
   let Path =  LIBremoveBeginning(FilePath, TRANSsourceDirectory);
   Path =  LIBremoveBeginning(Path, TRANScomponentGuid() + FILsep());
   let Result =  TRANScomponentGuid() + FILsep() +  Path;
   COL_TRC("Result =", Result);
   return Result;
}

function TRGTinitialized(FilePath){
   let Path = TRGTfilePathToRelativePath(FilePath);
   let AheadBehind = TRGTaheadBehind();
   if(!AheadBehind.hasOwnProperty(Path)) return true;
   let RepoObj = AheadBehind[Path];
   if(RepoObj) return RepoObj['initialized'];
   return false;
}

function TRGTreposInSync(FilePath){
   let Path = TRGTfilePathToRelativePath(FilePath);
   let AheadBehind = TRGTaheadBehind();
   if(!AheadBehind.hasOwnProperty(Path)) return true;
   let RepoObj = AheadBehind[Path];
   if(RepoObj) return RepoObj['correct_repo'];
   return false;
}

function TRGTanythingAhead(){
   let AheadBehind = TRGTaheadBehind();
   for(Repo in AheadBehind){
      if(TRGTahead(Repo)) return true;
   }
   return false;
}

function TRGTahead(FilePath){
   let Path = TRGTfilePathToRelativePath(FilePath);
   let AheadBehind = TRGTaheadBehind();
   let RepoObj = AheadBehind[Path];
   if(RepoObj) return RepoObj['can_push'];
   return false;
}

function TRGTbehind(FilePath){
   let Path = TRGTfilePathToRelativePath(FilePath);
   let AheadBehind = TRGTaheadBehind();
   let RepoObj = AheadBehind[Path];
   if(RepoObj) return RepoObj['can_pull'];
   return false;
}

function TRGTgitChanges(){
   let Changes = TRGTuncommittedChanges();
   return Object.keys(Changes).length;
}

function TRGTupdateCommitEnable(){ 
   let Disabled = !TRGTgitChanges() || !TRANSdevelopment();
   TRGTtoggleClass('.TRGTcommit', 'disabled', Disabled);
   TRGTtoggleClass('.TRGTcommit', 'TRGTblueDot',! Disabled); 
}

function TRGTupdatePullEnable(){ 
   let Disabled = TRGTgitChanges() || !TRANSdevelopment() || !TRGTgitRepo();
   TRGTtoggleClass('.TRGTgitPullRemote', 'disabled', Disabled ); 
}

function TRGTterminalUpdate(){
   TRGTupdateTerminalEnable();
   TRGTupdateTerminalTitle();
}

function TRGTupdateTerminalEnable(){
   let Enabled = document.querySelector('.TGITterm') && 1;
   TRGTtoggleClass('.TRGTgitTerminalButton', 'on', Enabled ); 
}

function TRGTupdateTerminalTitle(){
   let Enabled = document.querySelector('.TGITterm') && 1;
   let Title = Enabled ? "Close git terminal" : "Open git terminal";
   TRGTupdateTitle('.TRGTgitTerminalButton', Title);
}

function TRGTupdatePushEnable(){ 
   let PromotionState = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   let PromotionPhase = (typeof TRANSpromotionPhase === "function") ? TRANSpromotionPhase(PromotionState) : "";
   let LockedByPromotion = !!PromotionState && PromotionPhase === "pending_push";
   let Disabled = TRGTbehind() || !TRANSdevelopment() || LockedByPromotion;
   TRGTtoggleClass('.TRGTgitPushRemote', 'disabled',  Disabled); 
}

function TRGTupdatePullDot(){ 
   let Enabled = TRGTbehind("") && TRANSdevelopment();
   TRGTtoggleClass('.TRGTgitPullRemote', 'TRGTblueDot', Enabled); 
}

function TRGTupdatePushDot(){ 
   let Enabled = TRGTanythingAhead() && TRANSdevelopment();
   TRGTtoggleClass('.TRGTgitPushRemote', 'TRGTblueDot', Enabled); 
}
function TRGTupdateUpstreamDot(){
   TRGTtoggleClass('.TRGTrepoButton', 'TRGTblueDot', !TRGTgitRepo()?.repo && !TRGTgitRepo()?.plain && TRANSdevelopment());
}

function TRGTupdateCommitTitle(){ 
   let Title = TRGTgitChanges() ? 'Commit changes' : 'No changes to commit'
   TRGTupdateTitle('.TRGTcommit', Title); 
}

function TRGTupdatePullTitle(){
   let Title = TRGTgitChanges() ? 'Commit your changes before pulling from remote' : 'Pull changes from remote'; 
   Title = TRGTgitRepo() ? Title : 'Using a local repository – you can set an upstream repository to pull from'; 
   TRGTupdateTitle('.TRGTgitPullRemote', Title);
}

function TRGTpushTitle(){
   let PromotionState = (typeof TRANSpromotionCurrent === "function") ? TRANSpromotionCurrent() : null;
   let PromotionPhase = (typeof TRANSpromotionPhase === "function") ? TRANSpromotionPhase(PromotionState) : "";
   if(PromotionState && PromotionPhase === "pending_push") {
      return "Finish Promotion to push and return safely";
   }
   if(TRGTbehind()) return "Pull and integrate remote changes before pushing";
   if(!TRGTanythingAhead()) return "There are no changes to push";
   return "Push changes to remote"
}

function TRGTupdatePushTitle(){
   TRGTupdateTitle('.TRGTgitPushRemote', TRGTpushTitle());
}

function TRGTuncommittedChangesUpdate(){
   TRGTupdateCommitEnable();
   TRGTupdateCommitTitle();
}

function TRGTrepoLinkUpdate(){
   TRGTupdateUpstreamDot();
}

function TRGTuncommittedChangesAndRepoLinkUpdate(){
   TRGTupdatePullEnable();
   TRGTupdatePullTitle();
}

function TRGTaheadBehindUpdate(){
   TRGTupdatePullDot();
   TRGTupdatePushEnable();
   TRGTupdatePushDot();
   TRGTupdatePushTitle();
}

function TRGTisLibraryInCollection(LibUrl) {
   let AllLibraries = TRGTallLibraries();
   for(let i = 0; i < AllLibraries.length; i++) {
      const Library = AllLibraries[i];
      if(Library.git === LibUrl)      { return Library.collection; }
      if(Library.urls.ssh === LibUrl) { return Library.collection; }
      if(Library.urls.url === LibUrl) { return Library.collection; }
   }
   return false;
}

function TRGTupdateCopyUrlEnable() {
   // we treat "has a repo" as truthiness of TRGTgitRepo()
   let HasRepo = !!TRGTgitRepo();

   // visually disable the whole row
   TRGTtoggleClass('.GITTactionCopyUrl', 'is-disabled', !HasRepo);

   // also mark the inner icon as disabled to match existing styling
   TRGTtoggleClass('.GITTactionCopyUrl .TRANStoolbarButton', 'disabled', !HasRepo);

   // update tooltip/title on the row
   let Title = HasRepo
      ? 'Copy repository URL'
      : 'Set an upstream repository to copy its URL';
   TRGTupdateTitle('.GITTactionCopyUrl', Title);
}