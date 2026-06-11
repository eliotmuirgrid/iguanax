function RSBmodelInit(){
   MODELpollerAdd('RSBmodel', 'RSBcomponent',     10, 'component/list', {});
   MODELpollerAdd("RSBmodel", "RSBqueueConsumers", 4, "log/queue/consumers", RSBcompArgs);
   MODELpollerAdd("RSBmodel", "RSBpermissionAll",  4, "component/permission/all", {});
   MODELviewAdd  ('RSBmodel', 'RSBrenderSelect');
   MODELforcePoll("RSBmodel", function(){
      // RSBrenderView();
      // RSBlisteners();
   });
}

function RSBcompArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   return Args;
}

function RSBqueueConsumers(){
   return MODELdata?.RSBqueueConsumers?.data || [];
}

function RSBpermissionAll(){
   return MODELdata?.RSBpermissionAll?.data?.components || {};
}

function RSBcomponentListData(){
   return MODELdata?.RSBcomponent?.data?.component_data || [];
}
