function SECbuttonsUpdate(){
   let div = document.querySelector(".SANDeditTarget");
   if(!div) return;
   let Html = SECeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function SECsaveButton(){
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="SANDsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/security">CANCEL</a>
   </div>`;
}

function SECeditButton(){
   if(SECeditPage()) return SECsaveButton();
   let Edit    = SECpermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    =  Edit ? ` href="#settings/security/edit"` : "";
   return /*html*/`
   <div class="BUTTONgroup">
      <a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>
   </div>`;
}

