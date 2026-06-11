
function CARDerrorHtml(){
   let Data = CARDdata();
   if(Data.error_count) return CARDwithErrorHtml();
   else                 return CARDnoErrorHtml();
}

function CARDwithErrorHtml(){
   let Data = CARDdata();
   return /*html*/`
   <div>
      <div class="CARDerrorCount CARDerrorDisplayRed">${Data.error_count}</div>
      <div class="CARDstatusClearError CARDerrorOffset">[<div class="CARDclearFont">Clear</div>]</div>
   </div>
   `;
}

function CARDnoErrorHtml(){
   let Data = CARDdata();
   return /*html*/`<div class="CARDerrorCount CARDerrorDisplayGrey">${Data.error_count}</div><div class="CARDstatusClearError">[<div class="CARDclearFont">Clear</div>]</div>`;
}

function CARDupdateError() {
   let Data = CARDdata();
   let ErrorCount = Data.error_count;
   if (ErrorCount == null) return;
   if(ErrorCount.toString() !== document.querySelector('.CARDerrorCount').innerHTML){
      document.querySelector('.CARDerrorCountValue').innerHTML = CARDerrorHtml();
   }
}

function CARDclearError(ComponentGuidList) {
   console.log("Clear error");
   APIcall('component/clear_error', {'components': ComponentGuidList}, function(Response) {
      if(!Response.success){ SNCKsnackbar('error', "Failed to clear errors: " + Response.error );} 
      else{
         if (Response.data.cleared.length) { SNCKsnackbar('success', 'Component errors cleared.', 3000); }
         else if (Response.data.errors.length) { SNCKsnackbar('error', "Failed to clear errors. <a href='/#logs?tags=%23error&filter=true'>View error in Logs</a>", 3000, false); }
         else                              { SNCKsnackbar('error', "Failed to clear errors. ", 3000); }
         MODELforcePoll('CARDmodel');
         MODELforcePoll('DASHmodel');
      }
   });
}

function CARDlastErrorArgs(){
   return LGEargs({error : true});
}

function CARDlastErrorData(){
   return MODELdata?.CARDlastError?.data;
}

function CARDupdateLastErrorPopover() {
	let Perm = CARDpermissionData()?.view_logs;
	if (!Perm) ErrorLog = "You do not have permission to view this.";
	let ErrorData = CARDlastErrorData();
	if (!ErrorData) return;
	let ErrorDiv = document.querySelector(".CARDerrorCount");
	if (!ErrorDiv) return;
   if(parseInt(ErrorDiv.innerText) === 0) return;
   if(document.querySelector('.LGEtooltipWrap')) { return; }
   const data = CARDdata();
   let Html = LGEformmattedContent(ErrorData, data.guid, !data.commit, true);
   POPtooltip({
		target: ErrorDiv,
      content: Html,
      persist: true,
		position: POPtooltipPosition.BOTTOM,
      type: POPtooltipType.ERROR,
      style   : 'LGEtooltipWrap',
      callback: () => {
         STOREcreateErrorJumpStore();
         DASHclearErrorTooltip(data.guid);
      }
	});
}
