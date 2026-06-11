function PBLdeleteDirConfirmHtml(Path){
   return /*html*/`
<p>Are you sure you want to create library:</p>
<p style="margin-top: -8px;"><b class="PBLbreak">${ESChtmlEscape(Path)}</b></p>
<br/>
<div class="BUTTONgroup PBLgroup">
   <div class='PBLclose BUTTONcancel BUTTONstandard'>CANCEL</div>
   <div class='PBLcreate BUTTONaction BUTTONstandard'>CREATE</div>
</div>`;
}

function PBLrenderWindow(DeleteFilePath) {
   COL_TRC(DeleteFilePath);
   let Content = PBLdeleteDirConfirmHtml(DeleteFilePath);
   let Popout = POPpopoutOpen({
      title:'New Library', 
      content: Content, 
      width: '500px', 
      esc_close : true 
   });
   document.querySelector('.PBLcreate').addEventListener('click', function() {
      PBLpublishDir(DeleteFilePath, Popout);
      POPpopoutClose(Popout);
   });
   document.querySelector('.PBLclose').addEventListener('click', function() { POPpopoutClose(Popout); });
}

function PBLpublishDir(DeleteFilePath){
   let ApiData = {};
   ApiData.name        = DeleteFilePath;
   ApiData.component   = TRANScomponentGuid();
   ApiData.development = TRANSdevelopment();
   APIcall('component/library/create', ApiData, function(Response){
      if(!Response.success) return SNCKsnackbar('error','Cannot create library: ' + Response.error);
      MENUremoveTooltip();
      TGITopenBranchTerminal(function(){
         TERMexecuteCommand([`git rm --cached -r "${DeleteFilePath}"`], function(){
            MODELforcePoll('TRANSmodel');
            MODELforcePoll('TRANSstatusModel');
            LPRrender(DeleteFilePath);
         });
      });
   });
}
