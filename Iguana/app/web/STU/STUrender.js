function STUrender() {
   SETrender();
	document.querySelector('.STGlogging').classList.add('selected');
   STUmodelInit();
   STUlisteners();
}

function STUrenderLogging() {
   let Target = document.querySelector(".SETcontentBody");
   let Html = STUloggingHtml();
   if (Html !== Target.innerHTML){
      Target.innerHTML = Html;
      // STGupdateDiskUsage();
      // STUarchiveRemoveListener();
   }
}

function STUviewPerm(Guid){
   if (Guid == "Iguana-Service") return true;
   return STUdeletePermission()[Guid]?.view_logs;
}

function STUdeletePerm(Guid){
   if (Guid == "Iguana-Service") return STUpermissionData()?.delete_service_logs;
   return STUdeletePermission()[Guid]?.delete_logs;
}

function STUdeleteButtonHtml(Guid){
   let Delete = STUdeletePerm(Guid);
   if(!Delete) return "";
   return /*html*/`
   <div class="FORMbutton BUTTONcancel BUTTONwarning LOGcomponentClearLog">
      ${SETtrashIcon()}
   </div>`;
}

function STGlogSizeRowHtml(Obj, ShowAltStats) {
   let Guid = Obj.component;
   let View = STUviewPerm(Guid);
   let href = View ? `<a href="#logs?component=${ESChtmlEscape(Guid)}">${ESChtmlEscape(Obj.name)}</a>` : `<div>${ESChtmlEscape(Obj.name)}</div>`;
   if(Guid == "Iguana-Service") href = `<a href="#logs">${ESChtmlEscape(Obj.name)}</a>`;
   let Button = STUdeleteButtonHtml(Guid);
   return /*html*/`
   <div class="SETloggingRow">
      <div class="SETuserData STGcomponentIdOnTable"> ${href}</div>
      <div class="SETuserData">${Obj.path}</div>
      <div class="SETuserData">${STGfuzzySize(Obj.log_size)}</div>
      <div class="SETuserData">${ShowAltStats ? STGfuzzySize(Obj.alt_log_size) : ""}</div>
      <div class="SETuserData">
         ${Button}
      </div>
   </div>`;
}



function STGlogUsageHtml() {
   return `
   <div class="STGdisk-usage">
      <div class="STGbar">
         <div class="STGusage"></div>
      </div>
   </div>`;
}

function STGfuzzySize(Size) {
   if (Size === undefined) return "...";
   if (Size < 0)  { throw new Error('Size must be a positive number'); }
   let Count = 0;
   while (Size > 1000) {
      Size /= 1000;
      Count++;
   }
   const Units = ["B", "KB", "MB", "GB", "TB"];
   if (Count > 0) { Size = Size.toFixed(2); }
   return Size + " " + Units[Count];
}

function STGsortBySizeDesc() {
   let ComponentLogSizes = Object.values(STUcomponentData());
   ComponentLogSizes.sort((a, b) => parseInt(b.log_size) - parseInt(a.log_size));
   return ComponentLogSizes;
}

function STGupdateDiskUsage() {
   let data = STUlogData();
   const Free             = STGfuzzySize(data.system.disk_size - data.system.disk_used);
   const BarElement       = document.querySelector('.STGusage');
   const PercentageUsed   = data.system.disk_used / data.system.disk_size * 100;
   BarElement.style.width = Math.floor(PercentageUsed) + "%";
   document.querySelector('.STGspace').innerText = `${Free} free of ${STGfuzzySize(data.system.disk_size)}`;
}

function STGgetGuidFromName(Name, SourceData){
   for (let i of Object.values(SourceData)) if (i.name == Name) return i.component;
   return '';
}
