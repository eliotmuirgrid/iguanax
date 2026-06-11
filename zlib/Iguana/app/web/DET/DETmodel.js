
function DETmodelInit(){
   MODELpollerAdd('DETmodel', 'DETcomponentData',     10, 'component/detail',     DETcomponentArgs);
   MODELpollerAdd('DETmodel', 'DETcomponentList',     15, 'component/list',       {});
   MODELpollerAdd('DETmodel', 'DETrunningComponents', 2,  'component/run_list',    {});
   MODELpollerAdd('DETmodel', 'DETdownstreamData',    12, 'log/queue/consumers',  DETcomponentArgs);
   MODELpollerAdd('DETmodel', 'DETnextData',          4,  'logs/retrieve',        DETnextArgs);
   MODELpollerAdd('DETmodel', 'DETprevData',          14, 'logs/retrieve',        DETprevArgs);
   MODELviewAdd  ('DETmodel', 'DETupdateName');
   MODELviewAdd  ('DETmodel', 'DETupdateReposition');
   MODELviewAdd  ('DETmodel', 'DETswitchUpdate');
   MODELviewAdd  ('DETmodel', 'DETswitchUpdateVis');
   MODELviewAdd  ('DETmodel', 'DETupdateNext');
   MODELviewAdd  ('DETmodel', 'DETupdateNextHref');
   MODELviewAdd  ('DETmodel', 'DETupdatePrevious');
   MODELviewAdd  ('DETmodel', 'DETupdatePreviousHref');
   MODELforcePoll('DETmodel', function(){
      MODELpollerAdd('DETmodel', 'DETlastError', 4, 'logs/retrieve', DETlastErrorArgs);
      MODELforcePoll('DETmodel');
   });
   MODELstart('DETmodel');
}

function DETnextArgs(){
   let Args = DETprevArgs();
   Args.forward   = true;
   return Args;
}

function DETprevArgs(){
   let Args = {};
   Args.component = DETcomponentId();
   Args.exclusive = true;
   Args.start     = PAGEhashParameter("time");
   Args.start_id  = PAGEhashParameter("id");
   Args.exact     = PAGEhashParameter("exact");
   Args.search    = PAGEhashParameter("search");
   Args.tags      = PAGEhashParameter("tags");
   Args.include_non_matches = !PAGEhashParameter("filter");
   Args.limit     = 1;
   return Args;
}

function DETlastErrorArgs(){
   let Args = {};
   Args.component = DETcomponentId();
   Args.tags      = "#error";
   Args.exact     = true;
   Args.limit     = 1;
   Args.critical  = false;
   return Args;
}

function DETcomponentArgs(){
   let Args = {};
   Args.component = DETcomponentId();
   return Args;
}

function DETcomponentData(){
   return MODELdata.DETcomponentData.data;
}

function DETcomponentRunning(){
   let RunningData = MODELdata.DETrunningComponents.data;
   return RunningData[DETcomponentId()] ? true : false;
}

function DETcomponentId(){
   return PAGEhashParameter("component");
}

function DETcomponentListData(){
   return MODELdata?.DETcomponentList?.data?.component_data || [];
}

function DETcomponentName(ComponentId, Data){
   if(!ComponentId) return "Iguana Service";
   for(let i = 0; i < Data.length; i++){
      let I = Data[i];
      if(I.guid == ComponentId) return I.name;
   }
   return "";
}

function DETnextData(){
   return MODELdata?.DETnextData?.data || [];
}

function DETprevData(){
   return MODELdata?.DETprevData?.data || [];
}

function DETdownstreamData(){
   return MODELdata?.DETdownstreamData?.data || [];
}

function DETlastError(){
   return MODELdata?.DETlastError?.data || {};
}