function TRANSupdateMessage(Message){
   let ErrorMessageDiv = document.querySelector('.TRANSerrorMessage');
   if(!ErrorMessageDiv) return;
   if(ErrorMessageDiv.innerHTML !== Message) ErrorMessageDiv.innerHTML = Message;
}

function TRANSeditorLaunchErrorHtml(){
   return /*html*/`
   <div class="TRANSerrorDialog">
      <div>Failed to open</div>
      <ul>
         <li class="TRANSerrorMessage"></li>
      </ul>
      <br>
      <a class='BUTTONstandard BUTTONaction' href="${DASHformatUrl()}">GO TO DASHBOARD</a>
   </div>`;
}

function TRANShandleEditorLaunchError(Message) {
   COL_TRC(Message);
   let ErrorDialog = document.querySelector('.TRANSerrorDialog');
   if(!ErrorDialog){
      POPpopoutOpen({
         title   : 'Error',
         content : TRANSeditorLaunchErrorHtml(),
         width   : "auto",
         Zindex  : 1005,
         dismissible : false
      });
   }
   TRANSupdateMessage(Message);
}