function STGmodelInit(StartModel){
   STGrulesData = undefined;
   MODELpollerAdd("STGmodel", 'STGeditPermission', 10, 'component/permission', {});
   MODELpollerAdd("STGmodel", 'STGpurgeRules',     10, 'log/purge_rules',      {});
   MODELpollerAdd("STGmodel", 'STGpurgeAlt',       10, 'log/purge_rules/alt_dir', {});
   MODELviewAdd  ("STGmodel", "STGupdateEditButton");
   MODELviewAdd  ("STGmodel", "STGupdateRules");
   MODELforcePoll("STGmodel");
   if(StartModel) MODELstart("STGmodel");
}

function STGeditPermission(){
   return MODELdata?.STGeditPermission?.data?.log_purging || false;
}

function STGruleData(){
   return MODELdata?.STGpurgeRules?.data || [];
}

function STGaltEnabled(){
   return MODELdata?.STGpurgeAlt?.data?.alt_enabled || false;
}