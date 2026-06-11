function SLCupdateEditButton(){
   let div = document.querySelector(".SLCeditTarget");
   if(!div) return;
   let Html = SLCeditButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function SLCsaveButton(){
   return /*html*/`
   <div class="BUTTONgroup">
      <div class="SLCsave BUTTONaction BUTTONstandard">SAVE</div>
      <a class="BUTTONcancel BUTTONstandard" href="#settings/log_config">CANCEL</a>
   </div>`;
}

function SLCeditButton(){
   if(SLCeditPage()) return SLCsaveButton();
   let Edit = SLCeditPermission();
   let ComponentsStopped = !SLCcomponentsRunning();
   let Classes =  (ComponentsStopped && Edit) ? "BUTTONaction" : "BUTTONdisabled";
   let title;
   if (!ComponentsStopped) { title =` title="All components must be stopped first."`}
   if (!Edit)              { title =` title="You do not have permission to edit this."`; } // this error takes priority
   let href = ComponentsStopped && Edit ? ` href="#settings/log_config/edit"` : "";
   return /*html*/`
      <a class="${Classes} BUTTONstandard"${title}${href}>EDIT</a>
   `;
}

function SLCeditPage(){
   let Page = PAGEhash();
   return Page === "settings/log_config/edit";
}