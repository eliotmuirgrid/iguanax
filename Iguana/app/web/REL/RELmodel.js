let RELrelatedListData = [];

function RELdetailArgs(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.from      = PAGEhashParameter("from");
   Args.time      = PAGEhashParameter("time");
   Args.id        = PAGEhashParameter("id");
   Args.search    = PAGEhashParameter("search");
   Args.whole_word = PAGEhashParameter("whole_word");
   Args.type      = PAGEhashParameter("type");
   let Filter     = PAGEhashParameter("filter");
   Args.filter    = Filter;
   if(Filter) Args.to = PAGEhashParameter("time");
   else {
      Args.to    = PAGEhashParameter("to");
      Args.time  = PAGEhashParameter("time");
   }
   return Args;
}

function RELinitModelInit(){
   MODELpollerAdd("RELinitModel", "RELmessageData",    4, "log/detail",     RELdetailArgs);
   MODELpollerAdd('RELinitModel', 'RELcomponentList', 15, 'component/list', {});
   MODELforcePoll('RELinitModel', function(){
      RELgetRelatedUpstream(); 
      RELrelatedListData = [];
      let Args = RELrelatedSearchArgs();
      if(RELrelateMessageNotSet(Args)) return RELrenderNoRelatedMessages();
      APIcall('logs/retrieve', Args, function(R){
         RELrelatedMessagesModelInit();
      });
   });
}

function RELrelateMessageNotSet(Args){
   if(!Args?.rel_time || Args?.rel_time == "0") return true;
   if(!Args?.rel_id   || Args?.rel_id   == "0") return true;
   return false;
}

function RELrelatedMessagesModelInit(){
   RELselectedIndex = undefined;
   RELclearedOldList = false;
   MODELpollerAdd('RELrelatedMessagesModel', 'RELrelatedMessagesData', 0.5, "logs/retrieve/results", LOGpollerArgs);
   MODELviewAdd  ('RELrelatedMessagesModel', 'RELclearOldList');
   MODELviewAdd  ('RELrelatedMessagesModel', 'RELappendRelatedData');
   MODELviewAdd  ('RELrelatedMessagesModel', 'RELnoRelatedMessages');
   MODELviewAdd  ('RELrelatedMessagesModel', 'RELrelatedMessagesModelControl');
   MODELforcePoll('RELrelatedMessagesModel');
   MODELstart    ('RELrelatedMessagesModel');
}

function RELclearOldList(){
   if(RELclearedOldList || RELrelatedMessagesData().length === 0) return;
   let Div = document.querySelector(".RELlistDiv");
   if(!Div) return;
   Div.innerHTML = "";
   RELclearedOldList = true;
}

function RELappendRelatedData(){
   let Data = RELrelatedMessagesData();
   RELrelatedListAppend(Data);
}

function RELrelatedMessagesData(){
   return MODELdata?.RELrelatedMessagesData?.data?.data || [];
}

function RELnoRelatedMessages(){
   let Done = RELrelatedDone();
   if(Done && !RELrelatedListData?.length){
      RELrenderNoRelatedMessages()
   }
}

function RELrelatedDone(){
   return MODELdata?.RELrelatedMessagesData?.data?.done;
}

function RELrelatedMessagesModelControl(){
   if(RELrelatedDone()){
      MODELremove("RELrelatedMessagesModel");
   }
}

function RELmessageData(){
   return MODELdata?.RELmessageData?.data || {};
}

function RELrelatedSearchArgs(){
   let Data = RELmessageData();
   let Args = {}
   Args.component = PAGEhashParameter("component");
   Args.start   = Data.rel_time;
   Args.forward = true;
   Args.limit   = 100;
   Args.rel_comp = Data.rel_comp;
   Args.rel_time = Data.rel_time;
   Args.rel_id   = Data.rel_id;
   Args.job      = SESSgetTabGuid();
   return Args
}  
