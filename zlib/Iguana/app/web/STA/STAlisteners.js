function STAsetListeners(){
   let modal = document.querySelector(".SETsettingsPane");
   modal.addEventListener("click", STAclickHandler);
}

function STAclickHandler(e) {
   if(e.target.closest(".STAupdateLink"))        { STAupdatePopout(); }
   if(e.target.closest(".STAgetLatestLink"))     { STAlatestPopout(); }
   if(e.target.closest('.STAsetAutoUpgrade'))    { STAautoUpgradeModal(); }
   if(e.target.closest('.DBGshowList'))          { DBGlistFetch(); }
   if(e.target.closest('.STAstopAllComponents')) { STAstopAllComponentsPopout(); }
}


function STAstopAllComponentsHtml() {
   return /*html*/`
   <div class="STAdialogConfirm">Are you sure you want to stop all components?</div>
   <div class='BUTTONgroup DASHbuttonGroup'>
      <div class='STAstopComponentcancel BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='STAstopComponentsubmit BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}

function STAstopAllComponentsPopout(){
   let Card = POPpopoutOpen({title: 'Stop all components', content: STAstopAllComponentsHtml(), className: 'CARDactionPopup', esc_close : true, width: "500px"});
   document.querySelector('.STAstopComponentcancel').addEventListener('click', function() { POPpopoutClose(Card); })
   document.querySelector('.STAstopComponentsubmit').addEventListener('click', function() { STAstopAllComponents(); POPpopoutClose(Card);})
}

function STAstopAllComponents() {
   APIcall('component/stop_all', {}, function (R) {
      if (R.success) {
         COL_TRC("All components stopped successfully.");
         SNCKsnackbar("success", "Stopped all components.");
         MODELforcePoll('STAmodel');
      } else {
         let error = "Failed to stop components. Error: " + (R.error || "Unknown error");
         COL_TRC(error);
         SNCKsnackbar("error", error);
      }
   });
}
