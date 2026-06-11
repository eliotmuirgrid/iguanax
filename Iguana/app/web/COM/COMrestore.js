let COMrestoreHash;

function COMrestore(Hash){
   COMrestoreHash = Hash;
   CONFIRMaction(COMrestoreHtml(), COMrestoreFunc, "RESTORE");
}

function COMrestoreHtml(){
   let html = `<b>Restore to Commit ${COMrestoreHash}</b>
   <div class="COMbody">
      This will load your project exactly as it was at this commit.
      <br>Your history will remain but any uncommitted changes will be lost.
   </div>
   <p>Would you like to continue?</p>`
   return html;
}

function COMrestoreFunc(){
   TERMexecuteCommand(COMrestoreGitCommand(), function(){
      COMaddCustom(function(){
         TGITinitializeLibraries(COMrestoreHash, COMrestoreGitCommandCallback);
      });
   });
}

function COMrestoreGitCommand(){
   return [`git restore --source ${COMrestoreHash} --staged --worktree .`, `git clean -dffx`];
}

function COMrestoreGitCommandCallback(){
   let Edit = PAGEhashParameter("edit");
   if( !Edit || Edit == "main.lua" ) TRANSfileCurrentReload();
   else                              PAGEsetHashParameter("edit", "");
   setTimeout(TERMsetFocus, 300);
}

function COMaddCustom(callback){
   let ApiData = {};
   ApiData.component = PAGEhashParameter("component");
   ApiData.content = "-- Restored to commit " + COMrestoreHash;
   ApiData.development = PAGEhashParameter("development");
   APIcall('component/add/custom', ApiData, function(R) {
      if(callback) callback();
   });
}