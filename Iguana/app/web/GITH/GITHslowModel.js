function GITHinitSlowModel(){
   MODELpollerAdd('GITHslowModel', 'GITHgitTokenValid', 30, 'git/token/valid');
   MODELpollerAdd('GITHslowModel', 'GITHgitSshValid',   30, 'git/ssh/valid');
   MODELviewAdd('GITHslowModel', 'GITHupdateHidden');
   MODELviewAdd('GITHslowModel', 'GITHupdateToken');
   MODELviewAdd('GITHslowModel', 'GITHupdateSsh');
   MODELviewAdd('GITHslowModel', 'GITHupdateSlowWarning');
   MODELforcePoll('GITHslowModel', function(){
      CORmodelUpdateSlow();
   });
   MODELstart('GITHslowModel');
}

function GITHgitSshError(){
   return MODELdata?.GITHgitSshValid?.success == false ? MODELdata?.GITHgitSshValid?.error : "";
}
 
function GITHgitSshValid(){
   return MODELdata?.GITHgitSshValid?.data || {};
}



function GITHgitTokenValid(){
   return MODELdata?.GITHgitTokenValid?.data || {};
}
