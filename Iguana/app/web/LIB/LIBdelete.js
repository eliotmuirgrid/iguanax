function LIBdeleteModal(LibDetail){
   COL_TRC(LibDetail);
   let Card = POPpopoutOpen({title:'Delete Library', content: LIBconfirmDeleteHtml(LibDetail.path), className : 'LIBdeleteModal', esc_close : true});
   document.querySelector('.LIBdeleteDir')      .addEventListener('click', function() { LIBdelete(LibDetail, Card); });
   document.querySelector('.LIBdeleteDirCancel').addEventListener('click', function() { LIBdeleteCancel(Card); });
   MENUremoveTooltip();
}

function LIBdeleteCancel(Card) {
   COL_TRC(Card);
   POPpopoutClose(Card);
}

function LIBdelete(LibDetail, Popout) {
   COL_TRC(LibDetail, Popout);
   POPpopoutClose(Popout); 
   APIcall('component/library/delete', {name: LibDetail.name, component: TRANScomponentGuid(), development: TRANSdevelopment()}, function(Response) {
      Response.success ? SNCKsnackbar('success', 'Library Removed', 6000) : SNCKsnackbar('error', 'Library remove failed: ' + Response.error, 6000);
      if (TRANSfile().includes(LibDetail.name)) TRANSfileChangeWithoutSaving("main.lua");
      else {
         TRANSfileCurrentReload();
      }
   });
}

function LIBconfirmDeleteHtml(Path) { 
   return /*html*/`
   <div>The library and all the sub-directories and files will be deleted:</div>
   <br>
   <div class="ACTfolderNameDiv">
      <div class="ACTfolderNamePrefix">${ESChtmlEscape(Path + FILsep())}</div>
   </div>
   <br>
   <br>
   <b>Are you sure you want to delete them?</b>
   <br>
   <br>
   <br>
   <div class="BUTTONgroup ACTdeleteButtons">
      <div class='LIBdeleteDirCancel BUTTONcancel BUTTONstandard'>CANCEL</div>
      <div class='LIBdeleteDir BUTTONaction BUTTONstandard'>DELETE</div>
   </div>`;
}
