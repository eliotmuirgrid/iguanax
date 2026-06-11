function STDmodelInit(){
   MODELpollerAdd('STDmodel', 'STDpermissionData', 10, 'component/permission', {});
   MODELpollerAdd('STDmodel', 'STDdatabaseData',   10, 'settings/db/get', {});
   MODELforcePoll('STDmodel', function(){
      document.querySelector('.SETcontentBody').innerHTML = SETdatabaseHtml();
      STDrenderDatabases();
   });
}

function STDeditPermission(){
   return MODELdata?.STDpermissionData?.data?.database || false;
}

function STDdatabaseData(){
   return MODELdata?.STDdatabaseData || {};
}