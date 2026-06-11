function GSVupdateEditButton(){
   let div = document.querySelector(".GSVeditTarget");
   if(!div) return;
   let Html = GSVeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function GSVsaveButton(){
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="GSVsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/git_servers">CANCEL</a>
   </div>`;
}

function GSVeditButton(){
   if(GSVeditPage()) return GSVsaveButton();
   let Edit    = GSVeditPermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    =  Edit ? ` href="#settings/git_servers/edit"` : "";
   return /*html*/`<a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>`;
}

function GSVeditPage(){
   let Page = PAGEhash();
   return Page === "settings/git_servers/edit";
}