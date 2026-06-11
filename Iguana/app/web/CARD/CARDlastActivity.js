let CARDtodaysDate = new Date();

function CARDsomeLastActivityHtml(TimeStamp){
   return /*html*/`
   <div class="CARDlastActivityTimestamp">${TimeStamp}</div><div class="CARDlastActivityTooltip"></div>`;
}

function CARDnoLastActivityHtml(){
   return /*html*/`<div class="CARDlastActivityTimestamp CARDnoLastActivity">N/A</div>`;
}

function CARDgetDateOffsetFromToday(TheDate) {
   if(TheDate > CARDtodaysDate) { // update todays date
      CARDtodaysDate = TheDate;
      return 0;
   }
   return Math.abs(TheDate - CARDtodaysDate) / (1000 * 60 * 60 * 24);
}

function CARDlastLogData(){
   return MODELdata?.CARDlastLog?.data;
}

function CARDhandleNoLastLog(Error){
   let LastActivity = document.querySelector('.CARDlastActivityValue');
   if(!LastActivity) return;
   let Html = `<div class="CARDdisable">${Error || ""}</div>`;
   if(LastActivity.innerHTML !== Html) LastActivity.innerHTML = Html;
}

// TODO - this could probably be simplified - just separated it from the polling
function CARDupdateLastActivity(){
   let Perm = CARDpermissionData()?.view_logs;
   if(!Perm) return CARDhandleNoLastLog("You do not have permission to view this.");
   let Response = CARDlastLogData();
   if(!Response) return CARDhandleNoLastLog("Failed to get last activity log. Details: " + Response.error);
   if(!Response.length && document.querySelector('.CARDnoLastActivity') === null || !Perm) {
      let LastActivity = document.querySelector('.CARDlastActivityValue');
      let Html = CARDnoLastActivityHtml();
      if(LastActivity && LastActivity.innerHTML != Html){
         LastActivity.innerHTML = Html;
      }
   }
   if(Response.length){
      let TimeStamp = LOGtimeFormat(Response[0]['time']);
      if(document.querySelector('.CARDlastActivityTimestamp') === null || TimeStamp !== document.querySelector('.CARDlastActivityTimestamp').innerHTML){
         let LastActivity = document.querySelector('.CARDlastActivityValue');
         if(!LastActivity) return;
         const LogContent = LGEgetPopoverContent(Response, "CARDlastActivityPopoverEntry");
         LastActivity.innerHTML = CARDsomeLastActivityHtml(TimeStamp);
         POPtooltip({
            target: document.querySelector('.CARDlastActivityTimestamp'),
            content: LogContent,
            position: POPtooltipPosition.RIGHT,
            style: 'CARDlastActivityWrap'
         });
      }
   }
}