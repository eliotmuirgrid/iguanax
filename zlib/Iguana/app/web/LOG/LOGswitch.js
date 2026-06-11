function LOGcomponentData(){
   return MODELdata?.LOGcomponentData?.data || {};
}

function LOGswitchVisibility(Class, On){
   let S = document.querySelector(Class);
   if(!S) return;
   let Style = On ? "visible" : "hidden";
   S.style.visibility = Style;
}

function LOGcomponentRunning(){
   let RunningData = MODELdata?.LOGrunningComponents?.data;
   if(!RunningData) return false;
   return RunningData[LOGcomponentId()] ? true : false; 
}

function LOGswitchUpdate(){
   let S = document.querySelector('.LOGlight');
   if(!S) return;
   let Html = TOGswitchHtml("LOGlight", LOGcomponentData()?.light, LOGcomponentRunning(), LOGpermissionData()?.stop, LOGpermissionData()?.start); 
   let Div = document.querySelector('.LOGswitchTarget');
   if(Div.innerHTML !== Html) Div.innerHTML = Html;
   if(document.querySelector('.LOGinProgressInfo') === null) { LOGrenderSwitchToolTip(); }
}

function LOGrenderSwitchToolTip() {
   let Data      = LOGcomponentData();
   let ErrorData = LOGlastErrorData();
   if(Data.light === 'RED' && ErrorData) {
      let ErrorLog = LGEgetPopoverContent(ErrorData, "LGEtooltipWrap");
      POPtooltip({
         target  : document.querySelector(".LOGlight.TOGswitchDiv .TOGstatus"),
         content: LGEconvertLogErrorToUrl(Data.guid, !Data.commit, ErrorLog),
         position: POPtooltipPosition.BOTTOM,
         type    : POPtooltipType.ERROR,
         style  : 'LOGinProgressInfo',
         callback: () => {
            STOREcreateErrorJumpStore();
         }
      });
   }
   POPtooltip({
      target  : document.querySelector(".LOGlight.TOGswitchDiv .TOGbutton"),
      content : `<div class="LOGinProgressInfo">` + TOGswitchDescription(Data.light, LOGcomponentRunning()) + `</div>`,
      position: POPtooltipPosition.BOTTOM,
      style   : 'CARDswitchTooltip',
   });
}

function LOGlinkUpdate(){
   let div = document.querySelector(".LOGcomponentLink");
   if(!div) return;
   let href = DASHformatUrl(LOGcomponentId());
   if(div.href !== href) div.href = href;
}

function LOGisServer(){
   let ComponentId = LOGcomponentId();
   return !ComponentId;
}

function LOGswitchClicked() {
   let Div = document.querySelector(".LOGswitchTarget");
   let Command = TOGswitchApi(Div);
   let Arg = TOGswitchArg(Div, [LOGcomponentId()]);
   APIcall(Command, Arg, function(R) {
      if (!LICcheckComponentStartError(Command, R, function(){ MODELforcePoll('LOGmodel'); })){ return; }
      if (0 != R.data.errors.length) SNCKsnackbar('error', R.data.errors[0], 3000);
      MODELforcePoll("LOGmodel");
   });
}

