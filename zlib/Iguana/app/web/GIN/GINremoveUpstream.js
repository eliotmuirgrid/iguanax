function GINremoveUpstream(callback){  
   const Action = "<b>Confirm:</b><br><br>Do you want to remove the instance's upstream repository?<br><br>";
   CONFIRMaction(Action, GINremoveUpstreamConfirm, "CONFIRM", GINremoveUpstreamCancel, GINremoveUpstreamCallback);
}

function GINremoveUpstreamConfirm(){
   APIcall("instance/remove_upstream", {}, function(R){
      if(!R.success) SNCKsnackbar("error", "Failed to remove upstream repository: " + R.error);
      SNCKsnackbar("success", "Upstream repository removed from the instance");
      RPOclose();
      MODELforcePoll("GINmodel");
   });
}

function GINremoveUpstreamCancel(){
   MODELforcePoll("RPOmodel");
}

function GINremoveUpstreamCallback(){
   MODELforcePoll("RPOmodel");
}