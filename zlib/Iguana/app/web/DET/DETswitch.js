function DETisServer(){
   let ComponentId = DETcomponentId();
   return !ComponentId;
}

function DETswitchUpdateVis(){
   let S = document.querySelector('.DETswitchTarget');
   if(!S) return;
   DETswitchVisibility(".DETswitchTarget",  !DETisServer());
}

function DETswitchVisibility(Class, On){
   let S = document.querySelector(Class);
   if(!S) return;
   if(On){
      S.classList.remove("DEThidden");
   }
   else{
      S.classList.add("DEThidden");
   }
}

function DETswitchUpdate(){
   let S = document.querySelector('.DETlight');
   if(!S) return;
   let Html = TOGswitchHtml("DETlight", DETcomponentData()?.light, DETcomponentRunning(), DETpermissionData()?.stop, DETpermissionData()?.start); 
   let Div = document.querySelector('.DETswitchTarget');
   if(Div.innerHTML !== Html) {
      POPtooltipRemoveByClass('DETswitchTooltip');
      Div.innerHTML = Html;
   }
   if(document.querySelector('.DETinProgressInfo') === null) DETrenderSwitchToolTip();
}

function DETswitchClicked(){
   let Div = document.querySelector(".DETswitchTarget");
   let Command = TOGswitchApi(Div);
   let Arg = TOGswitchArg(Div, [DETcomponentId()]);
   APIcall( Command, Arg, function(R) {
      if (0 != R.data.errors.length) SNCKsnackbar('error', R.data.errors[0], 3000);
      MODELforcePoll("DETmodel");
   });
}

function DETrenderSwitchToolTip() {
   let Data = DETcomponentData();
	let ErrorData = DETlastError();
   if(Data?.light === 'RED' && ErrorData) {
      let ErrorLog = LGEgetPopoverContent(ErrorData);
      POPtooltip({
         target: document.querySelector(".DETlight.TOGswitchDiv .TOGstatus"),
         content: LGEconvertLogErrorToUrl(Data.guid, !Data.commit, ErrorLog),
         position: POPtooltipPosition.BOTTOM,
         type: POPtooltipType.ERROR,
         style  : 'DETinProgressInfo',
         callback: () => {
            STOREcreateErrorJumpStore();
         }
      });
   }
   POPtooltip({
      target: document.querySelector(".DETlight.TOGswitchDiv .TOGbutton"),
      content: `<div class="DETinProgressInfo">` + TOGswitchDescription(Data?.light, DETcomponentRunning()) + `</div>`,
      position: POPtooltipPosition.TOP,
      style: 'CARDswitchTooltip',
   });
}
