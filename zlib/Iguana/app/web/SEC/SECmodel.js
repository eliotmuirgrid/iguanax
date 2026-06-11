function SECmodelInit(ViewMode = true) {
   SECdirsData = undefined;
   MODELpollerAdd('SECmodel', 'SECdata',       10, 'security/get', {});
   MODELpollerAdd('SECmodel', 'SECpermission', 10, 'component/permission', {});
   MODELviewAdd('SECmodel', 'SECbuttonsUpdate')
   MODELviewAdd('SECmodel', 'SECupdateDirs');
   MODELforcePoll('SECmodel');
   if(ViewMode) MODELstart('SECmodel');
}

function SECpermission(){ return MODELdata?.SECpermission?.data?.edit_security;}
function SECdata()      { return MODELdata?.SECdata?.data; }