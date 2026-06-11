function GSVmodelInit(StartModel){
   GSVserversData = undefined;
   MODELpollerAdd("GSVmodel", 'GSVeditPermission', 10, 'component/permission', {});
   MODELpollerAdd("GSVmodel", 'GSVservers',        10, 'git/servers/load',     {});
   MODELviewAdd  ("GSVmodel", "GSVupdateEditButton");
   MODELviewAdd  ("GSVmodel", "GSVupdateServers");
   MODELforcePoll("GSVmodel");
   if(StartModel) MODELstart("GSVmodel");
}

function GSVeditPermission(){
   return MODELdata?.GSVeditPermission?.data?.edit_gitservers;
}

function GSVdata(){
   return MODELdata?.GSVservers?.data || {};
}
