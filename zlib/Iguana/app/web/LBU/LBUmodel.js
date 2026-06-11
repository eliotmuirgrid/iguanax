function LBUmodelInit(){
   MODELpollerAdd('LBUmodel', 'LBUdata',        30, 'library/info',  LBUargs, LBUlibsExist);
   MODELpollerAdd('LBUmodel', 'LBUcomponents',  10, 'component/list',  {});
   MODELviewAdd("LBUmodel", "LBUbuttonUpdate");
   MODELviewAdd("LBUmodel", "LBUcalcComponentNames");
   MODELviewAdd("LBUmodel", "LBUupdatePopout");
   MODELforcePoll('LBUmodel');
   // MODELstart    ('LBUmodel');
}

function LBUreset(){
   if(MODELdata && MODELdata.LBUdata) MODELdata.LBUdata = {};
}

function LBUcomponents(){
   return MODELdata?.LBUcomponents?.data.component_data || [];
}