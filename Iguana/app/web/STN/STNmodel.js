let STNruleData;

function STNmodelInit(StartModel){
   STNruleData = undefined;
   MODELpollerAdd('STNmodel', 'STNpermissionData',      5, 'component/permission', {});
   MODELpollerAdd('STNmodel', 'SETselectedNotiData',    5, 'notifications',  {});
   MODELpollerAdd('STNmodel', 'SETcomponentNotiData',   5, 'component/list', {});
   MODELpollerAdd('STNmodel', 'SEToperationalNotiData', 5, 'notifications/enabled', {});
   MODELpollerAdd('STNmodel', 'SETwarningData',         60,'notifications/enabled', {}); // TODO - eliminate redundant api call
   MODELviewAdd  ('STNmodel', 'STNupdateEditButton');
   MODELviewAdd  ('STNmodel', 'STNupdateSelectedComponent');
   MODELviewAdd  ('STNmodel', 'STNupdateRules');
   MODELforcePoll('STNmodel', function(){
      MODELforcePoll('CORmodel');
      MODELforcePoll('CORmodelSlow');
      MODELforcePoll('SETmodel');
   });
   if(StartModel) MODELstart('STNmodel');
}

function STNnotificationRules(){
   return MODELdata.SETselectedNotiData?.data?.rules || [];
}

function SETselectedNotiData(){
   return MODELdata.SETselectedNotiData;
}

function STNeditPermission(){
   return MODELdata?.STNpermissionData?.data?.edit_notifications || false;
}
