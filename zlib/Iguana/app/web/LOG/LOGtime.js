function LOGtimeFormatDate(date, weekdayFlag){
   const day = date.getDate(); // Get the day of the month
   const dayOfWeek = date.getDay();
   const month = date.toLocaleString('default', { month: 'short' }); // Get the short month name
   const year = date.getFullYear(); // Get the year
   
   const weekdays = ['Sunday', 'Monday', 'Tuesday', 'Wednesday', 'Thursday', 'Friday', 'Saturday'];
   const weekday = weekdays[dayOfWeek];

   // Get the suffix for the day (e.g. "st" for 1st, "nd" for 2nd, etc.)
   let suffix = "";
   if (day === 1 || day === 21 || day === 31) {
      suffix = "st";
   } else if (day === 2 || day === 22) {
      suffix = "nd";
   } else if (day === 3 || day === 23) {
      suffix = "rd";
   } else {
      suffix = "th";
   }
   // Format the date as "1st May 2023"
   const formattedDate = `${weekdayFlag ? weekday + ", " : ""}${month} ${day}${suffix}, ${year}`;
   return formattedDate;
}

function LOGtimeDay(date, weekdayFlag = true){
   let now =  new Date();
   now.setHours(0, 0, 0, 0);
   let datecopy = date;
   datecopy.setHours(0, 0, 0, 0);
   let Days = Math.floor((now - datecopy) / (1000 * 60 * 60 * 24));
   switch(Days){
      case 0: return "Today";
      case 1: return "Yesterday";
      default:return LOGtimeFormatDate(date, weekdayFlag);
   }
}

function LOGtimeFormat(EpochString, weekdayFlag = true, includeMilliseconds = true){
   let EpochNumber = parseInt(EpochString);
   let date = new Date(EpochNumber);
   const hours   = String(date.getHours())  .padStart(2, "0"); // Zero-padded hours
   const minutes = String(date.getMinutes()).padStart(2, "0"); // Zero-padded minutes
   const seconds = String(date.getSeconds()).padStart(2, "0");
   const fraction = String(EpochNumber % 1000).padEnd(3, "0");
   if(!includeMilliseconds) return `${LOGtimeDay(date, weekdayFlag)} ${hours}:${minutes}:${seconds}`;
   return `${LOGtimeDay(date, weekdayFlag)} ${hours}:${minutes}:${seconds}.${fraction}`;
}

function LOGjustTimeFormat(EpochString){
   let EpochNumber = parseInt(EpochString);
   let date = new Date(EpochNumber);
   const hours   = String(date.getHours())  .padStart(2, "0"); // Zero-padded hours
   const minutes = String(date.getMinutes()).padStart(2, "0"); // Zero-padded minutes
   const seconds = String(date.getSeconds()).padStart(2, "0");
   const fraction = String(EpochNumber % 1000).padEnd(3, "0");
   return `${hours}:${minutes}:${seconds}.${fraction}`;
}

function LOGdifferenceInDays(TimeDiff) {
   return Math.floor(TimeDiff / (1000 * 60 * 60 * 24));
}

function LOGdifferenceInHours(TimeDiff) {
   return Math.floor(TimeDiff / (1000 * 60 * 60));
}

function LOGdifferenceInMinutes(TimeDiff) {
   return Math.floor(TimeDiff / (1000 * 60));
}

function LOGaddSuffixS(Time){
   if (Time !== 1) return "s"
   return "";
}

function LOGtimeFormatShort(EpochString){
   let EpochNumber = parseInt(EpochString);
   let LogDate = new Date(EpochNumber);
   let Today = new Date();
   let Difference = Today - LogDate;
   const DaysBack    = LOGdifferenceInDays(Difference);
   const HoursBack   = LOGdifferenceInHours(Difference);
   const MinutesBack = LOGdifferenceInMinutes(Difference);
   if (DaysBack < 1){
      if (HoursBack < 1){ return `${MinutesBack} min` + LOGaddSuffixS(MinutesBack); }
      else              { return `${HoursBack} hr` + LOGaddSuffixS(HoursBack);   }
   }
   return `${DaysBack} day` + LOGaddSuffixS(DaysBack);
}

function LOGdayFormat(EpochString, OffByThousand = false){
   let EpochNumber = parseInt(EpochString);
   if(OffByThousand) EpochNumber = EpochNumber * 1000;
   let date = new Date(EpochNumber);
   return LOGtimeDay(date);
}

function LOGtimeOfDayFormat(EpochString){
   let EpochNumber = parseInt(EpochString);
   let date = new Date(EpochNumber);
   const hours   = String(date.getHours())  .padStart(2, "0"); // Zero-padded hours
   const minutes = String(date.getMinutes()).padStart(2, "0"); // Zero-padded minutes
   const seconds = String(date.getSeconds()).padStart(2, "0");
   const fraction = String(EpochNumber % 1000).padEnd(3, "0");
   return `${hours}:${minutes}:${seconds}.${fraction}`;
}

function LOGupdateDate(){
let TopLog = document.querySelector('.LOGtopVisible');
let Div = document.querySelector('.LOGcurrentDate');
let Perm = LOGviewPermission();
   if(!TopLog && Perm) {
      Div.classList.add("LOGhidden");
      return;
   }
   else{
      Div.classList.remove("LOGhidden");
   }
   let Idx = !TopLog ? 0 : LOGentryIndex(TopLog);
   let Data = LOGshowListData[Idx];
   let Date = LOGyearMonthDateTime({
      epoch: Data?.time,
      includeTime: false,
      simpleTime: false,
      shortDayLabel: false
   });
   if(Date.includes("Nth")) Date = "...";
   if(!Perm) Date = "You do not have permission to view this.";
   if(Div.innerHTML !== Date) Div.innerHTML = Date;
}

function LOGqueuePositionTime(QueuePosition){
   if (parseInt(QueuePosition) == 0) { return "N/A"; } // see TRNCfindEarliestQueuePosition
   else                              { return LOGtimeFormat(QueuePosition); }
}

function LOGdayFormatBanner(EpochString){
   let EpochNumber = parseInt(EpochString);
   let date = new Date(EpochNumber);
   return LOGtimeDay(date, false);
}

function LOGscrollbarPopout(){
   let Scrollbar = document.querySelector('.LOGscrollbarBox');
   if(!Scrollbar) return;
   POPtooltip({
      target: Scrollbar,
      content:`<div class="LOGscrollbarPopout"></div>`, 
      style: 'LOGtimeTooltip LOGhidden',
      position: POPtooltipPosition.RIGHT
   });
}

function LOGyearMonthDateTime(config) {
   const {
      epoch,
      includeTime = false,     // whether to include any time
      includeSeconds = false,  // <-- new option for seconds
      includeDay  = true,      // whether to show weekday
      includeYear = true,      // whether to show year
      simpleTime = false,      // if true -> HH:mm only
      shortDayLabel = false
   } = config;

   const date = new Date(Number(epoch));
   const now = new Date();

   // Normalize both dates to midnight for accurate relative day comparison
   now.setHours(0, 0, 0, 0);
   const datecopy = new Date(date);
   datecopy.setHours(0, 0, 0, 0);

   const Days = Math.floor((now - datecopy) / (1000 * 60 * 60 * 24));

   const year = date.getFullYear();
   const month = date.toLocaleString('default', { month: 'short' });
   const day = String(date.getDate()).padStart(2, '0');
   const weekday = date.toLocaleString('default', { weekday: 'short' });

   let prefix = "";
   if (Days === 0) prefix = "(Today) ";
   else if (Days === 1) prefix = shortDayLabel ? "(Yest) " : "(Yesterday) ";

   // Build parts dynamically
   const parts = [];
   if (includeYear) parts.push(year);
   if (includeDay) parts.push(weekday);
   parts.push(month, day);

   let result = prefix + parts.join(" ");

   if (includeTime) {
      const hours = String(date.getHours()).padStart(2, '0');
      const minutes = String(date.getMinutes()).padStart(2, '0');
      let timeStr = `${hours}:${minutes}`;
      let secondsPart = '';
      if (includeSeconds || !simpleTime) {
         const seconds = String(date.getSeconds()).padStart(2, '0');
         secondsPart = `:${seconds}`;
      }

      if (!simpleTime && !includeSeconds) {
         const seconds = String(date.getSeconds()).padStart(2, '0');
         const milliseconds = String(date.getMilliseconds()).padStart(3, '0');
         secondsPart = `:${seconds}.${milliseconds}`;
      }
      timeStr += secondsPart;

      result += ` ${timeStr}`;
   }

   return result;
}
