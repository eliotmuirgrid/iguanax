function STNupdateEditButton(){
   let div = document.querySelector(".STNeditTarget");
   if(!div) return;
   let Html = STNeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function STNsaveButton(){
   return /*html*/`
   <div class="SETnotificationActions BUTTONgroup">
      <div class="STNsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard"  href="#settings/notifications">CANCEL</a>
   </div>`;
}

function STNeditButton(){
   if(STNeditPage()) return STNsaveButton();
   let Edit = STNeditPermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    = Edit ? ` href="#settings/notifications/edit"` : "";
   return /*html*/`<a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>`;
}

function STNeditPage(){
   let Page = PAGEhash();
   return Page === "settings/notifications/edit";
}