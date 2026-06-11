function LOGinputControl(){
   LOGhandleClear("LOGsearchInput");
   LOGhandleClear("LOGtagsInput");
   LOGhandleClear("LOGstartTime");
   LOGhandleClear("LOGendTime");
   LOGupdateSearchButton();
}

function LOGhandleClear(input){
   let div =  document.querySelector(`.${input}`);
   if(!div) return;
   let empty = div.value.trim() == "";
   let parent = div.closest(".LOGsearch");
   if(!parent) parent = div.closest(".LOGtagsBox");
   if(!parent) parent = div.closest(".LOGdateWrapper");
   if(!parent) return;
   let button = parent.querySelector(".BUTTONcancel");
   if(!button) return;
   let classList = button.classList;
   if( classList.contains("LOGhidden") && !empty) classList.remove("LOGhidden");
   if(!classList.contains("LOGhidden") && empty)  classList.add   ("LOGhidden");
}

function LOGupdateSearchButton(){
   let Diff = false;
   if(LOGdetectDiff("LOGsearchInput", "search")) Diff = true;
   if(LOGdetectDiff("LOGtagsInput",   "tags"))   Diff = true;
   if(LOGdetectDiffCheckbox("LOGexactCheckbox",     "exact"))  Diff = true;
   if(LOGdetectDiffCheckbox("LOGfilterAllCheckbox", "filter")) Diff = true;
   if(LOGtimeDiff())                             Diff = true;
   let button = document.querySelector(".LOGsearchButton");
   if(!button) return;
   if(!Diff) {
      button.classList.remove("BUTTONaction");
      button.classList.add("BUTTONdefault");
   }
   else {
      button.classList.remove("BUTTONdefault");
      button.classList.add("BUTTONaction");
   }
}

function LOGdetectDiff(ClassName, Param){
   let div = document.querySelector(`.${ClassName}`);
   if(!div) return;
   let Val =  div.value || "";
   let ParamVal = PAGEhashParameter(Param);
   return Val != ParamVal;
}

function LOGdetectDiffCheckbox(ClassName, Param){
   let div = document.querySelector(`.${ClassName}`);
   if(!div) return;
   let Val =  div.checked;
   let ParamVal = PAGEhashParameter(Param) === "true";
   return Val != ParamVal;

}

function LOGtimeDiff() {
   const Start = PAGEhashParameter("to");
   const End   = PAGEhashParameter("from");
 
   const StartVal = String(LOGstartTimeValue());
   const EndVal   = String(LOGendTimeValue());
 
   // Convert to minutes (ignore seconds and milliseconds)
   const toMinutes = (t) => Math.floor(Number(t) / 60000);
 
   return (
     toMinutes(Start) !== toMinutes(StartVal) ||
     toMinutes(End)   !== toMinutes(EndVal)
   );
 }
 