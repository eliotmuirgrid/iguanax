function LOGclearLogsUpdate(){
   let Perm = LOGdeletePermission();
   let div = document.querySelector(".LOGclearLogs");
   if(!div) return;
   let title = !Perm ? "You do not have permission to do this." : "";
   if(div.title != title) div.title = title;
   if(!div.classList.contains("LOGdisabled") && !Perm) div.classList.add("LOGdisabled");
   if( div.classList.contains("LOGdisabled") &&  Perm) div.classList.remove("LOGdisabled");
}