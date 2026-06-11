function STLmodelInit(){
   MODELpollerAdd('STLmodel', 'STLpermissionData', 10, 'component/permission', {});
   MODELpollerAdd('STLmodel', 'STLlicenseData',    10, 'license/get',          {});
   MODELforcePoll('STLmodel', function(){
      let div = document.querySelector('.SETcontentBody');
      if(!div) return;
      div.innerHTML = SETlicenseHtml();
      STLrenderLicense();
   });
}

function STLeditPermission(){
   return MODELdata?.STLpermissionData?.data?.edit_license || false;
}

function STLlicenseData(){
   return MODELdata?.STLlicenseData || {};
}