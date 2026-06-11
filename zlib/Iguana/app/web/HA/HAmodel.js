let HAconfigData;
let HAlastEditState;

function HAmodelInit(StartModel) {
   HAconfigData    = undefined;
   HAlastEditState = undefined;
   MODELpollerAdd("HAmodel", 'HAeditPermission', 10, 'component/permission', {});
   MODELpollerAdd("HAmodel", 'HAconfig',         10, 'ha/get',     {});
   MODELviewAdd  ("HAmodel", "HAupdateEditButton");
   MODELviewAdd  ("HAmodel", "HAupdateConfig");
   MODELforcePoll("HAmodel");
   if(StartModel) MODELstart("HAmodel");
}

function HAeditPermission() { return MODELdata?.HAeditPermission?.data?.edit_authentication; }

function HAdata() { return MODELdata?.HAconfig?.data || {}; }

function HAcompareData(data) {
   const oldData = HAdata();
   return oldData.enabled === data.enabled && oldData.shared_working_directory === data.shared_working_directory && oldData.type === data.type;
}
