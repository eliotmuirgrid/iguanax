function ENVmodelInit(){
   MODELpollerAdd('ENVmodel', 'ENVpermissionData', 10, 'component/permission', {});
   MODELpollerAdd('ENVmodel', 'ENVenvironmentData', 10, 'environment/all', {});
   MODELviewAdd  ('ENVmodel', 'ENVinit');
   MODELviewAdd  ('ENVmodel', 'ENVsort');
   MODELviewAdd  ('ENVmodel', 'ENVpaint');
   MODELforcePoll('ENVmodel');
}

function ENVcreatePermission(){
   return MODELdata?.ENVpermissionData?.data?.env_variables || false;
}

function ENVenvironmentData(){
   return MODELdata?.ENVenvironmentData?.data || {};
}