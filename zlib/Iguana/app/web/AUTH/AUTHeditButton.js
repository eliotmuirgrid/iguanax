function AUTHupdateEditButton(){
   let div = document.querySelector(".AUTHeditTarget");
   if(!div) return;
   let Html = AUTHeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function AUTHsaveButton(){
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="AUTHsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/authentication">CANCEL</a>
   </div>`;
}

function AUTHeditButton(){
   if(AUTHeditPage()) return AUTHsaveButton();
   let Edit    = AUTHeditPermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    =  Edit ? ` href="#settings/authentication/edit"` : "";
   return /*html*/`<a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>`;
}

function AUTHeditPage(){
   let Page = PAGEhash();
   return Page === "settings/authentication/edit";
}