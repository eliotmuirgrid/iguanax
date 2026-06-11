function GINcredentialWarning(callback){
   SNCKsnackbar('warning', "You must set your Git credentials to commit changes to the instance. [<a class='COMerrorWarningLink' href='/#settings/git'>Set Git Credentials</a>]", 5000, false);
   if(callback) { callback(); }
}

function GINaddAndCommit(args = {}) {
   APIcall("git/credentials", {}, function(R){
      if(!R.success) return false;
      let Data = R?.data || {};
      let Warning = !Data?.name?.length || !Data?.email?.length;
      if(Warning) return GINcredentialWarning(args.callback);
      const {message = "Instance Modifications", 
         allow_empty = true, 
         components = [], 
         settings = false, 
         users = false, 
         reports = false, 
         other = false, 
         all = false, 
         component = null,   
         callback = null} = args;
      const Args = {message, allow_empty, components, settings, users, reports, other, all, component};
      APIcall("instance/add_and_commit", Args, function(R) {
         if(!R.success) { SNCKsnackbar('error', R.error); }
         MODELforcePoll("COMmodel");
         if(callback) { callback(); }
      });
   });
}
