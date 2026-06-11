function CRPremoveUpstream(){
   const Action = "<b>Confirm:</b><br><br>Do you want to remove the component's upstream repository?<br><br>";
   CONFIRMaction(Action, CRPremoveUpstreamConfirm, "CONFIRM", CRPremoveUpstreamCancel, CRPremoveUpstreamCallback);
}

function CRPremoveUpstreamConfirm(){
   let Component = RPOcomponent();
   if(!Component) return;
   APIcall("component/remove_upstream", {component : Component}, function(Data){
      if(!Data.success) SNCKsnackbar("error", "Failed to remove upstream repository: " + Data.error);
      let Name = RPOcomponentName(Component);
      let Message = GINremoveComponentUpstream(Name);
      GINaddAndCommit({message : Message, component : Component, all : true, callback : RPOclose});
   });
}

function CRPremoveUpstreamCancel(){
   MODELforcePoll("RPOmodel");
}

function CRPremoveUpstreamCallback(){
   MODELforcePoll("RPOmodel");
   MODELforcePoll("TRANSmodel");
   MODELforcePoll("TRANSstatusModel");
}