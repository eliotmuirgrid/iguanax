let LOGSclearLogsPopout;

function LOGclearLogsClicked(){
   let Perm = LOGdeletePermission();
   if(!Perm) return;
   let ComponentData = LOGcomponentData();
   if( !Object.keys(ComponentData).length ) ComponentData = { name : "Iguana Service" };
   let Name = ComponentData.name;
   let Html = LOGclearLogsHtml(Name);
   LOGSclearLogsPopout = POPpopoutOpen({
      title:'Delete Logs', 
      className : "LOGSclearLogsModal", 
      width: "550px",
      content: Html
   });

   let DeleteLogs = document.querySelector(".LOGSdeleteLogs");
   if(DeleteLogs) DeleteLogs.addEventListener("click", LOGclearLogsCall);
   document.querySelector(".LOGSdeleteCancel").addEventListener("click",  function() { POPpopoutClose(LOGSclearLogsPopout);} );
}

function LOGdeleteButtonHtml(){
   let Perm = LOGdeletePermission();
   let Classes =  Perm ? "LOGSdeleteLogs BUTTONaction" : "BUTTONdisabled";
   let Title   = !Perm ? ` title="You do not have permission to do this."` : "";
   return /*html*/`<div class="${Classes} BUTTONstandard"${Title}>DELETE</div>`;
}

function LOGclearLogsHtml(ComponentName){
   return /*html*/`
   IguanaX will delete the logs that can be safely removed. 
   <br/>
   <br/>
   Are you certain you wish to delete the logs for the <b>${ComponentName}</b>?
   <br>
   <br>
   <b>Warning:</b> This action cannot be reversed.
   <div class="BUTTONgroup DETbuttons">
      <div class='LOGSdeleteCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      ${LOGdeleteButtonHtml()}
   </div>`;
}


function LOGclearLogsCall(){
   let ComponentId = LOGcomponentId();
   APIcall("log/component/clear", {component : ComponentId}, function(R) {
      let DeleteSuccess = R.data.before_time != "";
      let Message = DeleteSuccess ? `Deleted all log files before ${R.data.before_time}` : `No logs to delete`;
      SNCKsnackbar("success", Message, 10000);
      POPpopoutClose(LOGSclearLogsPopout);
      LOGscreen();
   });
}
