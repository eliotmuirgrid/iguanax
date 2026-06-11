let TGITterminal;

function TGITopenBranchTerminal(callback){
   const id = 'TGITterminal';
   let Translator = document.querySelector(".TRANSwindowEdit");
   if(WNDisWindow(id)){
      COL_TRC("Git terminal already open in window");
      WNDforward(id);
      WNDhighlight(id);
   } else if (document.getElementById(id + 'docked')) {
      COL_TRC("Git terminal already open in tab");
      WNDtabSelected(id);
   } else if(Translator){
      COL_TRC('Creating new terminal');
      WNDcreateWindow(id, `<div class='TGITterm'></div>`, "Terminal");
      WNDaddToDock('TGITterminal', 'WNDdockBottom')
      TGITterminal = new TERMterminal({component_id: TRANScomponentGuid(), anchor: '.TGITterm'});
      TGITterminal.setPrompt(`${TRANScomponentGuid()}`);
   }
   TERMdelayedSetFocus();
   if (callback) callback();
}

function TGITgitBranch(){ TGITexecuteCommand('git branch'); }

function TGITinitializeLibraries(commitId = null, callback = null){
   let ApiData = {};
   ApiData.component = TRANScomponentGuid();
   ApiData.development = TRANSdevelopment();
   if(commitId) ApiData.commit_id = commitId;
   APIcall("component/library/initialize/all", ApiData, function(R){
      if(!R.success) return SNCKsnackbar('error', 'Could not initialize libraries: ' + R.error);
      MODELforcePoll('TRANSgitModel');
      MODELforcePoll('TRANSmodel');
      if(callback) callback();
   });
}

function TGITgitPull() {
   TGITopenBranchTerminal(function(){
      let submodules = TGITgetWritableSubmodules();
      let commandChain = ['git pull'];
      // for(let i = 0; i < submodules.length; i++) { 
      //    let Path = submodules[i];
      //    commandChain.push(`git -C "${Path}" pull`); 
      // }
      commandChain.push('git clean -dffx');
      TERMexecuteCommand(commandChain, TGITgitPullCallback);
   });
}

function TGITgitPullCallback(){
   TGITinitializeLibraries();
   const Data = {
      component: TRANScomponentGuid(),
      development: TRANSdevelopment(),
      filename: TRANSfile(),
      sample_data_index: SMPLindex()
   };
   APIcall('ide/annotation', Data, function() {}) // TODO should we do anything?
}

function TGITgitPush() {
   if(TRANSsourceDirectory) {
      let submodules = TGITgetWritableSubmodules();
      let commandChain = [];
      for(let i = 0; i < submodules.length; i++) { 
         let Path = submodules[i];
         if(LIBlibraryUnlocked(Path)) commandChain.push(`git -C "${Path}" push -u origin HEAD`); 
      }
      commandChain.push('git push');
   
   TGITopenBranchTerminal(function() {TERMexecuteCommand(commandChain, function(){ 
      MODELforcePoll('TRANSstatusModel'); 
      // MODELforcePoll('LBUmodel'); 
   });});
   }
   else { TGITopenDialog('Cannot Perform Git Push', 'No repo has been configured to push to.'); }
}

function TGITgitNoChangesContent(dial){ return /*html*/`
   <div class='TGITnoGitChanges'>
      <div>${dial}</div>
      <div class='BUTTONgroup'>
         <div class='TRANSdismissNoChanges BUTTONstandard BUTTONaction'>OK</div>
      </div>
   </div>`;
}

function TGITopenDialog(Title, Dialog){
   COL_TRC("No changes dialog");
   let Card = POPpopoutOpen({title: Title, content: TGITgitNoChangesContent(Dialog), Zindex: 1005, esc_close : true});
   document.querySelector('.TRANSdismissNoChanges').addEventListener('click', function() { POPpopoutClose(Card); });
}

function TGITexecuteCommand(Command){ TGITopenBranchTerminal(function(){ TGITterminal.executeCustom(Command); }); }

function TGITgetWritableSubmodules() {
   let submodules = [];
   let LibraryList = TRGTlibraryList();
   for(let [name, info] of Object.entries(LibraryList)) { 
      // if(info.write === true) { submodules.push(name); } // TODO - we assume we have write access to libraries for now
      submodules.push(name);  
   }
   return submodules;
}