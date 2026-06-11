function DOC2diffClick(){
   PAGEsetHashParameter("development", true);
   APIcall('git/uncommitted_changes', DOC2diffArgs(), function(R) {
      if(!R.success) return SNCKsnackbar("error", R?.error || "");
      TGITUcommitInfo = R.data;
      if(!Object.keys(TGITUcommitInfo).length) { 
         SNCKsnackbar("info", "No changes to display.")
         return DOC2closeCommit(); 
      }
      TGITrenderModal();
      TGITselectFirstFile();
      TGITsetupListeners();
   });
}

function DOC2diffArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.development = true;
   return Args;
}

function DOC2commit(commandChain){
   let Args = DOC2diffArgs();
   Args.commands = commandChain;
   APIcall('git/custom_command', Args, function(R) {
      if(!R?.success) SNCKsnackbar("error", R?.error);
      else SNCKsnackbar("success", "Committed changes successfully.")
      DOC2closeCommit();
   });
}

function DOC2closeCommit(){
   let GitWindow = document.querySelector('.TGITgitWindow');
   if(!GitWindow) return;
   GitWindow.parentElement.remove();
}