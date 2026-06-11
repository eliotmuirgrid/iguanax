function AUTHmodelInit(StartModel){
   AUTHauthData = undefined;
   MODELpollerAdd("AUTHmodel", 'AUTHeditPermission', 10, 'component/permission', {});
   MODELpollerAdd("AUTHmodel", 'AUTHprovider',       10, 'auth/get',     {});
   MODELviewAdd  ("AUTHmodel", "AUTHupdateEditButton");
   MODELviewAdd  ("AUTHmodel", "AUTHupdateProvider");
   MODELforcePoll("AUTHmodel");
   if(StartModel) MODELstart("AUTHmodel");
}

function AUTHeditPermission(){
   return MODELdata?.AUTHeditPermission?.data?.edit_authentication;
}

function AUTHdata(){
   return MODELdata?.AUTHprovider?.data || {};
}

function AUTHlicensed(){
   return MODELdata?.AUTHprovider?.data?.license || false;
}

function AUTHcompareData(data) {
   const oldData = AUTHdata();
   if(oldData.mode !== data.mode)         { return false; }
   if(oldData.provider !== data.provider) { return false; }
   if(data.mode === 'client') {
      return oldData.client_id === data.client_id && oldData.organization_id === data.organization_id;
   } else if(data.mode === 'saml') {
      return oldData.entity_id === data.entity_id && oldData.acs_url === data.acs_url && oldData.metadata_path === data.metadata_path;
   }
   return true;
}
