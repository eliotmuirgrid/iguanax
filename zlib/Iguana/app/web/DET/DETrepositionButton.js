function DETupdateReposition() {
   const RepositionBtn = document.querySelector('.DETreposition');
   if(!RepositionBtn) return;
   let Type = DETmessageData()?.type;
   let Perm = DETpermissionData()?.reposition_queue;
   const IsDataMessage = Type === "message";
   let NoDownstream = DETdownstreamData().length == 0;
   if(!IsDataMessage || !Perm) {
      RepositionBtn.classList.add("DETdisabled");
      RepositionBtn.title = "Reposition Queue - Must be a data message";
   } 
   else if(NoDownstream){
      RepositionBtn.classList.add("DETdisabled");
      RepositionBtn.title = "Reposition Queue - No downstream components connected";
   }
   else{
      RepositionBtn.classList.remove("DETdisabled");
      RepositionBtn.title = "Reposition Queue";
   }
}
