function CARDswitchHtml() {
   return TOGswitchHtml("CARDlight", CARDstatus(), CARDcomponentOn(), CARDpermissionData()?.stop, CARDpermissionData()?.start);
}

function CARDupdateSwitch(){
   let Html = CARDswitchHtml();
   let Div = document.querySelector('.CARDswitchTarget');
   if(!Div) return;
   if(Div.innerHTML !== Html) {
      POPtooltipRemoveByClass('CARDswitchTooltip');
      Div.innerHTML = Html;
   }
   if(document.querySelector('.CARDinProgressInfo') === null) CARDrenderSwitchToolTip();
}

function CARDswitchClicked(e) {
   if(!e.target.closest(".TOGbutton")) return;
   let Div = document.querySelector(".CARDswitchTarget");
   let Command = TOGswitchApi(Div);
   let Arg = TOGswitchArg(Div, [CARDcomponentId()]);
   if(Command == 'component/start') CARDsaveOpenEdits();
   APIcall(Command, Arg, function(Response) {
      if (!Response.success) SNCKsnackbar('error', Response.error, 3000);
      else if (0 != Response.data.errors.length) SNCKsnackbar('error', Response.data.errors[0], 3000);
      MODELforcePoll('CARDmodel');
      MODELforcePoll('DASHmodel');
   });
}

function CARDrenderSwitchToolTip() {
   let Data = CARDdata();
	let ErrorData = CARDlastErrorData();
   if(Data.light === 'RED' && ErrorData) {
      let ErrorLog = LGEgetPopoverContent(ErrorData);
      POPtooltip({
         target  : document.querySelector(".CARDlight").children[0],
         content: LGEconvertLogErrorToUrl(Data.guid, !Data.commit, ErrorLog),
         position: POPtooltipPosition.BOTTOM,
         type: POPtooltipType.ERROR,
         style  : 'CARDinProgressInfo',
      });
   }
   POPtooltip({
      target: document.querySelector(".CARDlight.TOGswitchDiv .TOGbutton"),
      content: `<div class="CARDinProgressInfo">` + TOGswitchDescription(Data.light, CARDcomponentOn()) + `</div>`,
      position: POPtooltipPosition.BOTTOM,
      style: 'CARDswitchTooltip',
   });
}

function CARDcomponentOn(){
   let RunningData = MODELdata.CARDrunningComponents.data;
   return RunningData[CARDcomponentId()] ? true : false; 
}