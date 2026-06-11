// TODO - Refactor this into MODEL view framework and move to it's own library

let SMPLimportLogFilter;
let SMPLimportLogTags;
let SMPLimportLogComponent;
let SMPLimportList = {};

function SMPLrenderLogList(Filter) {
   COL_TRC(Filter);
   const LogList = document.querySelector('.SMPLimportLogMessages');
   LogList.innerHTML = '';
   if (SMPLimportList.length == 0) {
      LogList.innerHTML = `<div class='SMPLimportLogMessage SMPLimportLogMessageMeta SMPLimportLogMessageMetaNoMatch'>* No matching results *</div>`;
      return;
   }
   for (let i = 0; i < SMPLimportList.length; ++i) {
      let Log = SMPLimportList[i].content;
      LogList.insertAdjacentHTML('beforeend', `<div class='SMPLimportLogMessage'>` + Log + `</div>`);
   }
   if (Filter) {  // scroll to search term location
      let Logs = document.querySelectorAll(".SMPLimportLogMessage:not(.SMPLimportLogMessageMeta)");
      for (let i = 0; i < SMPLimportList.length; ++i) {
         let Log = Logs[i];
         let LogOffset = Log.offsetLeft;
         let SearchTerm = Log.querySelector("span");
         if(!SearchTerm) { return; }
         let SearchTermOffset = SearchTerm.offsetLeft;
         Log.scrollLeft = SearchTermOffset - LogOffset - Log.clientWidth / 2;
      }
   }
}

function SMPLloadLogs() {
   let Args = {};
   Args.component = document.querySelector('.SMPLcomponentList').value.trim();
   Args.limit = 100;
   let SearchText = document.querySelector(".SMPLsearchFilter")?.value;
   let SearchTag  = document.querySelector(".SMPLtagFilter")?.value;
   if(SearchText) {
      Args.search = SearchText;
   } else { SearchText = ""; }
   if(SearchTag) {
      Args.tags = SearchTag;
   } else { SearchTag = ""; }
   APIcall("logs/retrieve", Args, function(R) {
      if (!R.success) return SMPLshowLogListError(R);
      SMPLimportList = R.data || [];
      SMPLrenderLogList(SearchText);
   });
}

function SMPLsetupLogList() {
   COL_TRC("SMPLsetupLogList");
   const LogList = document.querySelector('.SMPLimportLogMessages');
   LogList.addEventListener('click', function(e) {
      if (e.target.matches('.SMPLimportLogMessage:not(.SMPLimportLogMessageMeta)')) {
         SMPLonListClick(e);
      }
   });
}

function SMPLlistItemIndex(ClickTarget){
   return [...ClickTarget.parentElement.children].indexOf(ClickTarget); // expand the parent div, iterate until we match the click target and return the index
}

function SMPLloadRawLogMessage(Id, Time, Component, Callback){
   let Args = {
      time      : Time,
      id        : Id,
      component : Component,
      raw       : true // This means we'll get the message without any html escaping or trimming.
   }
   APIcall("log/detail", Args, function(R) {
      if (!R.success) return ""; // TODO -figure out error reporting
      if (Callback) { Callback(SMPLeditor, R.data.content); }
   });
}

function SMPLonListClick(E){
   COL_TRC(E);
   const Index = SMPLlistItemIndex(E.target);
   // Using the index, we can get the ID and time from SMPLimportList to get the raw message
   SMPLloadRawLogMessage(SMPLimportList[Index].id, SMPLimportList[Index].time, document.querySelector('.SMPLcomponentList').value.trim(), SMPLloadPreview);
}
function SMPLonComponentSelectLoad(R) {
   COL_TRC(R);
   if (!R.success) return SMPLhandleComponentDataError(R);

   const array = R.data.component_data;
   const dataMap = TRANScomponentMapData();
   const currentGuid = TRANScomponentGuid();
   const currentComponent = dataMap[currentGuid] || {};
   const sources = PRJgetSourcesAndReceivers(currentComponent, dataMap).sources;
   const allComponents = SMPLgetAllComponents(array);

   const select = document.querySelector('.SMPLcomponentList');
   SMPLresetSelect(select);
   SMPLaddOptgroup(select, 'Sources', sources);
   SMPLaddOptgroup(select, 'All components', allComponents);

   select.value = currentGuid;
   SMPLattachSelectChangeHandler(select);
   SMPLloadLogs();
 }

 function SMPLhandleComponentDataError(R) {
   COL_TRC('Failed to load component list');
   return SMPLshowLogImporterError(R);
 }

 function SMPLbuildComponentDataMap(array) {
   return array.reduce((map, comp) => {
     map[comp.guid] = comp;
     return map;
   }, {});
 }

 function SMPLgetCurrentGuid() {
   return SMPLimportLogComponent || TRANScomponentGuid();
 }

 function SMPLgetAllComponents(array) {
   return array
     .filter(c => c.name !== 'Iguana Service')
     .sort((a, b) => (a.name > b.name ? 1 : -1))
     .map(c => ({ guid: c.guid, name: c.name }));
 }

 function SMPLresetSelect(select) {
   select.innerHTML = '';
 }

 function SMPLaddOptgroup(select, label, items) {
   if (label === 'Sources' && items.length === 0) return;
   const group = document.createElement('optgroup');
   group.label = label;
   items.forEach(item => {
     const opt = document.createElement('option');
     opt.value = item.guid;
     opt.textContent = item.name;
     group.appendChild(opt);
   });
   select.appendChild(group);
 }

 function SMPLattachSelectChangeHandler(select) {
   select.addEventListener('change', Event => {
     SMPLimportLogComponent = Event.target.value;
     SMPLloadLogs();
   });
 }

 function SMPLsetupComponentSelect() {
   APIcall("component/list", null, SMPLonComponentSelectLoad);
 }

function SMPLsetupFilterInput() {
   let Filter = document.querySelector(".SMPLsearchFilter");
   COL_TRC(Filter);
   Filter.value = SMPLimportLogFilter ? SMPLimportLogFilter : "";
   Filter.addEventListener('change', function() {
      SMPLimportLogFilter = Filter.value;
   });
   Filter.addEventListener("keyup", function(e) {
      e.preventDefault();
      SMPLloadLogs();
   });
}

function SMPLsetupTagInput() {
   let Tags = document.querySelector(".SMPLtagFilter");
   COL_TRC(Tags);
   Tags.value = SMPLimportLogTags ? SMPLimportLogTags : "";
   Tags.addEventListener('change', function() {
      SMPLimportLogTags = Tags.value;
   });
   Tags.addEventListener("keyup", function(e) {
      e.preventDefault();
      SMPLloadLogs();
   });
}

function SMPLdialogSetUpImport() {
   SMPLsetupComponentSelect();
   SMPLsetupLogList();
   SMPLsetupFilterInput();
   SMPLsetupTagInput();
}

function SMPLshowLogListError(R){
   const ComponentLogs = document.querySelector('.SMPLcomponentLogs');
   if (ComponentLogs){
      ComponentLogs.innerHTML = "Failed to get list of logs for this component. Details: " + R.error;
      ComponentLogs.style.color = 'red';
   }
}

function SMPLshowLogImporterError(R){
   const Display = document.querySelector('.SMPLimportFromLogControls');
   const ComponentList = document.querySelector('.SMPLcomponentList');
   const ComponentLogs = document.querySelector('.SMPLcomponentLogs');
   if (Display && ComponentList && ComponentLogs) {
      const ErrorDisplay = document.createElement('div');
      ErrorDisplay.textContent = R.error;
      ErrorDisplay.style.color='red';
      Display.insertBefore(ErrorDisplay, ComponentList);
      ComponentList.remove();
      ComponentLogs.remove();
   }
}