function CARDupdateQueue(){
   let Data = CARDdata();
   let Count = Data.queue_count;
   let Div = document.querySelector('.CARDqueuePosition');
   if(!Div) return;
   if( Div.innerHTML !== String(Count)) Div.innerHTML = Count;
}

function CARDclearQueue(ComponentGuidList){
   APIcall('component/clear_queue', {'components': ComponentGuidList}, function(Response) {
      if(!Response.success) return SNCKsnackbar('error', `Failed to clear queue: ${Response.error}`);
      let Data = Response.data;
      if(!Data)                               return SNCKsnackbar('error', 'Failed to clear queue.', 3000);
      if(Data.errors  && Data.errors.length)  return SNCKsnackbar('error', 'Failed to clear queue. <a href="/#logs?tags=%23error&filter=true">View error in Logs</a>', 5000, false);
      if(Data.skipped && Data.skipped.length) return SNCKsnackbar('info', 'Queue count is already zero.', 5000);
      SNCKsnackbar('success', 'Component queue cleared.', 3000); 
      MODELforcePoll('CARDmodel');
      MODELforcePoll('DASHmodel');
   });
}

function CARDupdateClearLinks(){
   CARDupdateClearLink(document.querySelector(".CARDstatusClearQueue"), "clear_queue");
   CARDupdateClearLink(document.querySelector(".CARDstatusClearError"), "clear_errors");
   CARDupdateClearLink(document.querySelector(".CARDstatusRepositionQueue"), "reposition_queue");
}

function CARDupdateClearLink(Div, Field){
   if(!Div) return;
   let ControlPerm = CARDpermissionData()[Field];
   let Class = "CARDdisableLink";
   if(ControlPerm) Div.classList.remove(Class);
   else            Div.classList.add   (Class);
   let title = ControlPerm ? "" : "You do not have permission to do this.";
   if(Div.title != title) Div.title = title;
}

function CARDupdateReposition(){
   let Target = document.querySelector(".CARDstatusRepositionQueue");
   if (!Target) return;
   let ControlPerm = !CARDcomponentOn();
   let Class = "CARDdisableLink";
   if(ControlPerm) Target.classList.remove(Class);
   else            Target.classList.add   (Class);
   let title = ControlPerm ? "" : "Cannot reposition queue while the component is running.";
   if(Target.title != title) Target.title = title;
}