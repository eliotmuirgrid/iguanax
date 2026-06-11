function LOGmodelInit(Callback){
   MODELpollerAdd("LOGmodel", "LOGcomponentData",     2, "component/detail",   LOGcomponentArgs);
   MODELpollerAdd('LOGmodel', 'LOGrunningComponents', 2, 'component/run_list', {});
   MODELpollerAdd('LOGmodel', 'LOGsizeData',          10, 'log/directory', {});
   MODELpollerAdd('LOGmodel', 'LOGlogComponent',      10, 'log/component/usage', {});
   MODELpollerAdd('LOGmodel', 'LOGcomponentList',     5, 'component/list',   {});
   MODELpollerAdd('LOGmodel', 'LOGpermissionData',    5, 'component/permission',     LOGcomponentArgs);
   MODELpollerAdd('LOGmodel', 'LOGpermissionAll',     5, 'component/permission/all', LOGcomponentArgs);
   MODELpollerAdd("LOGmodel", "LOGlstPoll",           5, "logs/resubmit/results", {});
   MODELpollerAdd("LOGmodel", "LOGcomponentMapData",  10, "component/map", {group: false});
   MODELpollerAdd('LOGmodel', 'LOGlastError', 2, 'logs/retrieve', LOGlastErrorArgs);
   MODELviewAdd  ('LOGmodel', 'LOGswitchUpdate');
   MODELviewAdd  ('LOGmodel', 'LOGlinkUpdate');
   MODELviewAdd  ('LOGmodel', 'LOGcomponentListUpdate');
   MODELviewAdd  ('LOGmodel', 'LOGclearSearchStatus');
   MODELviewAdd  ('LOGmodel', 'LOGclearSearchCount');
   MODELviewAdd  ('LOGmodel', 'LOGclearLogsUpdate');
   MODELviewAdd  ('LOGmodel', 'LOGsizeDisplay');
   MODELviewAdd  ('LOGmodel', 'LOGsizeDirDisplay');
   MODELviewAdd  ('LOGmodel', 'LOGupdateDate');
   MODELviewAdd  ('LOGmodel', 'LOGcomponentExistsUpdate');
   MODELviewAdd  ('LOGmodel', 'LOGlstControl');
   // MODELviewAdd  ('LOGmodel', 'LOGinputControl');
   MODELviewAdd  ('LOGmodel', 'PRJpopulateSourcesReceivers');
   MODELviewAdd  ('LOGmodel', 'LOGcontrolSourcesReceivers');
   MODELforcePoll('LOGmodel');
   MODELstart    ('LOGmodel', function(){
      if(Callback) Callback();
   });
}

function LOGpermissionData(){
   return MODELdata?.LOGpermissionData?.data || {};
}

function LOGpermissionAll(){
   return MODELdata?.LOGpermissionAll?.data?.components || {};
}

function LOGlastErrorData() {
   return MODELdata?.LOGlastError?.data;
}

function LOGcomponentMapData() {
   return MODELdata?.LOGcomponentMapData?.data || {};
}

function LOGclearSearchStatus(){                // IX-4185 See if it needs a tags equiv
   if(PAGEhashParameter("search") || PAGEhashParameter("tags")) return;
   let Div = document.querySelector(".LOGsearchStatus");
   let html = "";
   if(Div.innerHTML != html) Div.innerHTML = html;
}

function LOGclearSearchCount(){                 // IX-4185 See if it needs a tags equiv
   let SearchArrows = document.querySelector(".LOGsearchNavRow");
   if(!SearchArrows) return;
   if(PAGEhashParameter("search") || PAGEhashParameter("tags")) {
      SearchArrows.classList.remove("LOGhidden");
      return;
   }
   let Input = document.querySelector(".LOGsearchIndexInput");
   let Total = document.querySelector(".LOGsearchTotal");
   if(Input) Input.value = "";
   if(Total) Total.innerHTML = "";
   SearchArrows.classList.add("LOGhidden");
}

function LOGcomponentArgs(){
   let Args = {};
   Args.component = LOGcomponentId();
   return Args;
}

function LOGcomponentId(){
   return PAGEhashParameter("component");
}

function LOGcomponentListData(){ return MODELdata?.LOGcomponentList?.data?.component_data; }

function LOGviewPermission(){
   let Data = LOGpermissionAll();
   let Guid = LOGcomponentId();
   let Exists = Data[Guid];
   if(!Exists) return true;
   return LOGpermissionData()?.view_logs;
}

function LOGdeletePermission(){
   let Data = LOGpermissionAll();
   let Guid = LOGcomponentId();
   let Exists = Data[Guid];
   if(!Exists) return LOGpermissionData()?.delete_service_logs;
   return LOGpermissionData()?.delete_logs;
}

function LOGlstDone(){ return MODELdata?.LOGlstPoll?.data?.done; }
