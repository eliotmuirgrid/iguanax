function LOGstartTimeValue(){
   const Div = document.querySelector('.LOGstartTime');
   return LOGtimeValue(Div);
}

function LOGendTimeValue(){
   const Div = document.querySelector('.LOGendTime');
   return LOGtimeValue(Div);
}

function LOGtimeValue(Div){
   if(!Div) return;
   let TimeInMs = new Date(Div.value);
   if (!isNaN(TimeInMs.getTime())) TimeInMs = TimeInMs.getTime();
   else TimeInMs = "";
   return TimeInMs;
}

function LOGsetStartTime(){
   const Div = document.querySelector('.LOGstartTime');
   let Time = parseInt(PAGEhashParameter("to"));
   LOGsetDateTimeInputValue(Div, Time);
}

function LOGsetEndTime(){
   const Div = document.querySelector('.LOGendTime');
   let EndTime = parseInt(PAGEhashParameter("from"));
   LOGsetDateTimeInputValue(Div, EndTime);
}

function LOGstartTimeArgs(){
   let Start = parseInt(PAGEhashParameter("to"));
   let End  = parseInt(PAGEhashParameter("from"));
   let Time = Start;
   if (Start < End) { Time = End; }
   return Time;
}

function LOGendTimeArgs(){
   let Start = parseInt(PAGEhashParameter("to"));
   let End   = parseInt(PAGEhashParameter("from"));
   let Time = End;
   if (Start < End) { Time = Start; }
   return Time;
}

function LOGformatTimeStampCustom(date){
   // We need to use the low level methods to format the date to "yyyy-MM-ddThh:mm"
   // Can't rely on methods as they don't give the right format, or in the case of toISOString, give UTC time.
   const year = date.getFullYear();
   const month = (date.getMonth() + 1).toString().padStart(2, '0'); // Months are zero-based
   const day = date.getDate().toString().padStart(2, '0');
   const hours = date.getHours().toString().padStart(2, '0');
   const minutes = date.getMinutes().toString().padStart(2, '0');
   return `${year}-${month}-${day}T${hours}:${minutes}`;
}

function LOGepochToTimeSelect(Time){
   const date = new Date(Time);
   if(date == "Invalid Date") return "";
   return LOGformatTimeStampCustom(date);
}

function LOGsetDateTimeInputValue(dateTimeInput, epochMilliseconds) {
   if(!dateTimeInput) return;
   let html;
   if(!epochMilliseconds) html = "";
   else                   html = LOGepochToTimeSelect(epochMilliseconds);
   if( dateTimeInput.value != html ) dateTimeInput.value = html;
}
 
function LOGstartTimeChanged(){
   let Time = LOGstartTimeValue();
   PAGEsetHashParameter("to", Time);
}

function LOGendTimeChanged(){
   let EndTime = LOGendTimeValue();
   PAGEsetHashParameter("from", EndTime);
}

function LOGtopDateLineClicked(){
   let Div = document.querySelector(".LOGstartTime");
   if(!Div) return;
   Div.focus();
}

function LOGbottomDateLineClicked(){
   let Div = document.querySelector(".LOGendTime");
   if(!Div) return;
   Div.focus();
}

function LOGscrollDateLineClicked(){
   let Div = document.querySelector(".LOGcurrentDate");
   if(!Div) return;
   let Text = Div.innerHTML;
   let BoldText = `<b>${Text}</b>`;
   Div.innerHTML = BoldText;
}

