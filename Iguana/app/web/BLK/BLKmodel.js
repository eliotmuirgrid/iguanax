function BLKmodelInit(){
   MODELpollerAdd('BLKmodel', 'BLKcomponent',     10, 'component/list', {});
   MODELpollerAdd("BLKmodel", "BLKpermissionAll",  4, "component/permission/all", {});
   MODELpollerAdd("BLKmodel", "BLKqueueConsumers", 4, "log/queue/consumers", BLKcompArgs);
   MODELviewAdd  ('BLKmodel', 'BLKrenderSelect');
   MODELviewAdd  ('BLKmodel', 'BLKrenderCriteria');
   MODELforcePoll('BLKmodel', function(){
      MODELpollerAdd('BLKmodel', 'BLKlast', 10, 'logs/retrieve', BLKlast);
      MODELforcePoll('BLKmodel');
      BLKgetMiddle();
   });
}

function BLKcompArgs(){
   let args = {};
   args.component = LOGcomponentId();
   return args;
}

function BLKqueueConsumers(){
   return MODELdata?.BLKqueueConsumers?.data || [];
}

function BLKcomponentListData(){
   return MODELdata?.BLKcomponent?.data?.component_data || [];
}

function BLKpermissionAll(){
   return MODELdata?.BLKpermissionAll?.data?.components || {};
}

function BLKlastData(){
   let Data = MODELdata?.BLKlast?.data || [];
   return Data[0] || {};
}