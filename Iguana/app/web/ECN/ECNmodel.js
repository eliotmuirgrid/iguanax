let ECNnameData = {};

function ECNmodelInit(){
   MODELpollerAdd("ECNmodel", "ECNmapData",          2, "component/map",  ECNargs);
   MODELpollerAdd("ECNmodel", "ECNlistData",         5, "component/list", {});
   MODELpollerAdd("ECNmodel", "ECNsourcesData",      5, "component/sources/allowable", ECNallowableArgs);
   MODELpollerAdd('ECNmodel', 'ECNpermsisionAll',   10, 'component/permission/all', {});
   MODELpollerAdd("ECNmodel", "ECNdestinationsData", 5, "component/destinations/allowable", ECNallowableArgs);
   MODELviewAdd  ('ECNmodel', "ECNlistDataUpdate");
   MODELviewAdd  ('ECNmodel', "ECNsourcesUpdate");
   MODELviewAdd  ('ECNmodel', "ECNtargetUpdate");
   MODELviewAdd  ('ECNmodel', "ECNreceiversUpdate");
   MODELviewAdd  ('ECNmodel', "ECNaddSourceUpdate");
   MODELviewAdd  ('ECNmodel', "ECNaddReceiverUpdate");
   MODELforcePoll("ECNmodel");
}

function ECNallowableArgs(){
   let Args = {};
   Args.component = ECNcomponent();
   return Args;
}

function ECNcomponent(){
   return PAGEhashParameter("component");
}

function ECNmapData(){
   return MODELdata?.ECNmapData?.data || {};
}

function ECNlistData(){
   return MODELdata?.ECNlistData?.data?.component_data || [];
}

function ECNlistDataUpdate(){
   let Data = ECNlistData()
   ECNnameData = {};
   for(ComponentData of Data){
      let CompData = ComponentData || {};
      let Component = CompData?.guid;
      ECNnameData[Component] = CompData;
   }
}

function ECNname(Component){
   let Data = ECNnameData;
   return Data[Component]?.name;
}

function ECNdata(){
   let Data = ECNmapData();
   let Component = ECNcomponent();
   return Data[Component] || {};
}

function ECNsourcesData(){
   let Data = ECNdata();
   return Data?.sources || [];
}

function ECNreceiversData(){
   let Data = ECNdata();
   return Data?.receivers || [];
}

function ECNargs(){
   let Args = {};
   Args.checked_only = true;
   Args.checked = [ECNcomponent()];
   return Args;
}

function ECNallowableSources(){
   return MODELdata?.ECNsourcesData?.data?.sources || [];
}

function ECNallowableDestinations(){
   return MODELdata?.ECNdestinationsData?.data?.destinations || [];
}

function ECNpermsisionAll(){
   return MODELdata?.ECNpermsisionAll?.data?.components || {};
}

function ECNeditPermission(Guid){
   let Data = ECNpermsisionAll();
   if(!Data) return false;
   if(!Data[Guid]) return false;
   let Edit1 = Data[Guid]?.edit_connections;
   let Comp = ECNcomponent();
   if(!Data[Comp]) return false;
   let Edit2 = Data[Comp]?.edit_connections;
   return Edit1 && Edit2;
}