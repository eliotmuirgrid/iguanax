let SLBdetailData = undefined;

function SLBdetailModelInit(){
   MODELpollerAdd('SLBdetModel', 'SLBdetData',     30, 'library/info',  SLBargs);
   MODELpollerAdd('SLBdetModel', 'LBUcomponents',  10, 'component/list',  {});
   // MODELviewAdd("SLBdetModel", "LBUbuttonUpdate");
   MODELviewAdd("SLBdetModel", "LBUcalcComponentNames");
   MODELviewAdd("SLBdetModel", "SLBupdateData");
   MODELviewAdd("SLBdetModel", "SLBupdatePopout");
   MODELviewAdd("SLBdetModel", "SLBupdateDetails");
   MODELforcePoll('SLBdetModel');
   MODELstart    ('SLBdetModel');
}

function SLBargs(){
   let key = PAGEhashParameter("url") || "";
   return { key : [key] };
}

function SLBdetData(){
   return MODELdata?.SLBdetData?.data || SLBdetailData || {};
}

function SLBupdateData(){
   SLBdetailData = SLBdetData();
}