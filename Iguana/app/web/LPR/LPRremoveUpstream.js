function LPRremoveUpstream(){
   const Action = "<b>Confirm:</b><br><br>Do you want to remove the library's upstream repository?<br><br>";
   CONFIRMaction(Action, LPRremoveUpstreamConfirm, "CONFIRM", LPRremoveUpstreamCancel, LPRremoveUpstreamCallback);
}

function LPRremoveUpstreamConfirm(){
   let Args = {};
   Args.component = TRANScomponentGuid();
   Args.name = LPRlibraryName;
   Args.development = TRANSdevelopment();
   APIcall("component/library/remote/remove", Args, function(R){
      if(!R.success) SNCKsnackbar("error", "Failed to remove upstream repository: " + R.error);
      SNCKsnackbar("success", "Upstream repository removed");
      MODELforcePoll("TRANSstatusModel");
      RPOclose();
   });
}

function LPRremoveUpstreamCancel(){
   MODELforcePoll("RPOmodel");
}

function LPRremoveUpstreamCallback(){
   MODELforcePoll("RPOmodel");
}