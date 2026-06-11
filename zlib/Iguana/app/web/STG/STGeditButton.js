function STGupdateEditButton(){
   let div = document.querySelector(".STGeditTarget");
   if(!div) return;
   let Html = STGeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function STGsaveButton(){
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="STGsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/purging">CANCEL</a>
   </div>`;
}

function STGeditButton(){
   if(STGeditPage()) return STGsaveButton();
   let Edit    = STGeditPermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    = Edit ? ` href="#settings/purging/edit"` : "";
   return /*html*/`
      <div class="STGtest BUTTONstandard BUTTONdefault">Preview Purge</div>
      <a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>
   `;
}

function STGeditPage(){
   let Page = PAGEhash();
   return Page === "settings/purging/edit";
}