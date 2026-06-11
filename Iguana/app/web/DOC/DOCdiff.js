function DOCdiffClick(){
   PAGEsetHashParameter("development", true);
   APIcall('git/uncommitted_changes', DOCdiffArgs(), function(R) {
      if(!R.success) return SNCKsnackbar("error", R?.error || "");
      TGITUcommitInfo = R.data;
      if(!Object.keys(TGITUcommitInfo).length) { 
         SNCKsnackbar("info", "No changes to display.")
         return DOCcloseCommit(); 
      }
      TGITrenderModal();
      TGITselectFirstFile();
      TGITsetupListeners();
   });
}

function DOCdiffArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.development = true;
   return Args;
}

function DOCcommit(commandChain){
   let Args = DOCdiffArgs();
   Args.commands = commandChain;
   APIcall('git/custom_command', Args, function(R) {
      if(!R?.success) SNCKsnackbar("error", R?.error);
      else SNCKsnackbar("success", "Committed changes successfully.")
      DOCcloseCommit();
   });
}

function DOCcloseCommit(){
   let GitWindow = document.querySelector('.TGITgitWindow');
   if(!GitWindow) return;
   GitWindow.parentElement.remove();
}