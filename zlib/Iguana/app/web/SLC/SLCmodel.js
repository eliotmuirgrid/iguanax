function SLCmodelInit(StartModel){
   SLCmodelData = undefined;
   MODELpollerAdd("SLCmodel", "SLCeditPermission",    4, 'log/encryption/permission');
   MODELpollerAdd("SLCmodel", "SLCcomponentsRunning", 3, 'component/run_list');
   MODELpollerAdd("SLCmodel", 'SLClogEncryption',     6, 'log/encryption', {});
   MODELviewAdd  ("SLCmodel", "SLCupdateEditButton");
   MODELviewAdd  ("SLCmodel", "SLCupdateLogEncryption");
   MODELforcePoll("SLCmodel");
   if(StartModel) MODELstart("SLCmodel");
}

function SLCeditPermission(){
   return MODELdata?.SLCeditPermission?.success || false;
}

function SLClogEncryption(){
   return MODELdata?.SLClogEncryption?.data?.encyption;
}

function SLCcomponentsRunning(){ 
   let RunningComponents = MODELdata?.SLCcomponentsRunning?.data || {}; 
   let Count = Object.keys(RunningComponents).length;
   return Count != 0;
}
