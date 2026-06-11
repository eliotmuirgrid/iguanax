let EDCcard = {};

function EDCinit(GitUrl, AddToCollection = false){
   EDCgit = GitUrl;
   EDCrender();
   let ApiData = {};
   ApiData.git = GitUrl;
   APIcall('git/collection/edit', ApiData, function(R){
      let Modal = document.querySelector('.EDCmodal');
	   if(!Modal) return;
      if(!R.success) Modal.innerHTML = EDCfailedToGetCollectionHtml(R.error);
      else           EDCcollectionRenderHtml(R.data, AddToCollection);
   });
}

function EDCrender(){
   console.log("EDCrender");
	if(document.querySelector('.EDCmodal')) return;
   EDCcard = POPpopoutOpen({
      title: 'Edit collection', 
      height: '90%', 
      width: '90%', 
      minWidth: '1000px', 
      maxWidth: '1300px', 
      maxHeight: '90%', 
      content: EDChtml(),  
      className: 'EDCwindowContent',  
      esc_close : true,
      callback : EDCclose
   });
   EDClisteners();
}

function EDCclose(){
   POPpopoutClose(EDCcard);
}

function EDChtml(){
   return /*html*/`
   <div class="EDCmodal">
      ${EDCwaitWhilePullingHtml()}
      <div class="BUTTONgroup EDCbuttons">
         <div class="BUTTONstandard BUTTONdisabled">DISMISS</div>
      </div>
   </div>`;
}

function EDCwaitWhilePullingHtml(){
   return /*html*/`
   <div class="EDCloadingMessage">Git url: ${EDCgit}<br><br>Hang tight while our application pulls in the latest version of the collection file, it will only take a moment.</div>`;
}

function EDCwaitWhilePushingHtml(){
   return /*html*/`
   <div class="EDCloadingMessage">Git url: ${EDCgit} <br><br>Hang tight while our application saves and pushes the latest changes to the collection file, it will only take a moment.</div>
   <div class="BUTTONgroup EDCbuttons">
      <div class="BUTTONstandard BUTTONdisabled">DISMISS</div>
   </div>
   `;
}

function EDCfailedToGetCollectionHtml(ErrorMessage){
   return /*html*/`
   <div class="EDCloadingMessage">${ErrorMessage}</div>
   <div class="BUTTONgroup EDCbuttons">
      <div class="EDCcancel BUTTONstandard BUTTONaction">DISMISS</div>
   </div>`;
}