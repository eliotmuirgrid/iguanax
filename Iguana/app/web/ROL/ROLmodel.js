function ROLmodel(){
   MODELpollerAdd("ROLmodel", "ROLroleData",   10, "role/list",            {});
   MODELpollerAdd("ROLmodel", "ROLpermission", 10, "component/permission", {});
   // MODELviewAdd  ("ROLmodel", "ROLrenderView");
   MODELforcePoll("ROLmodel", function(){
      ROLrenderView();
      ROLlisteners();
   });
}

function ROLeditPermission(){
   return MODELdata?.ROLpermission?.data?.edit_roles || false;
}

function ROLrolesData(){
   return MODELdata?.ROLroleData?.data || {};
}