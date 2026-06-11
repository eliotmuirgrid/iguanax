function HAupdateEditButton() {
   let div = document.querySelector(".HAeditTarget");
   if(!div) { return; }
   let Html = HAeditButton();
   if(div.innerHTML !== Html) { div.innerHTML = Html; }
}

function HAsaveButton() {
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="HAsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/high_availability">CANCEL</a>
   </div>`;
}

function HAeditButton() {
   if(HAeditPage()) { return HAsaveButton(); }
   let Edit    = HAeditPermission();
   let Classes = Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    = Edit ? ` href="#settings/high_availability/edit"` : "";
   return /*html*/`<a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>`;
}

function HAeditPage() {
   let Page = PAGEhash();
   return Page === "settings/high_availability/edit";
}
