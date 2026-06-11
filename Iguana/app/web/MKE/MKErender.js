let MKEpopout;
let MKEgitUrl;
let MKEcomponentId;
let MKEcallback;
let MKEmessage;
function MKEmakeEditablePopout(T){
   MKEresetGlobals();
   MKEcomponentId = T?.component; 
   MKEgitUrl      = T?.url;
   MKEcallback    = T?.callback;
   MKEmessage     = T?.message;
   MKEinitModel(MKErender);
}

function MKErender(){
   MKEpopout = POPpopoutOpen({title: 'Make component editable', content: MKEhtml(), width : "750px", close: MKEclose});
   MKEsetListeners();
}

function MKEhtml() {
   let Message = MKEmessage || "";
   if(Message) Message = `<p class="MKEmessage">${Message}</p>`;
   let Name = MKEdata().name || "";
   let Url = MKEurl() || "";
   let UrlHtml = Url ? `<p class="MKEtab">Currently connected to: ${Url}</p><br>` : "";
   return /*html*/`
   <div class="MKEmodal">
      ${Message}
      <div class="MKEbox">
         <p>To edit "${Name}" please choose one of the options below.</p>
         ${UrlHtml}
      </div>
      <div class="MKEfeedback"></div>
      <div class="BUTTONgroup MKEbuttons">
         <div class="MKEcancel BUTTONcancel BUTTONstandard">Cancel</div>
         <div class="MKEmakeEditableTarg BUTTONdisabled BUTTONstandard" title="Change the original. Requires write permissions to this component.">
            Edit Original
            <div class="CARDloading-spinner"></div>
         </div>
         <div class="MKEmakeCopy BUTTONaction BUTTONstandard" title="Copy the component - this leaves the original the same.">Make a Copy</div>
      </div>
   </div>`;
}

function MKEcustomizeError(error) {
   let ErrorDiv = document.querySelector('.MKEfeedback');
   if(ErrorDiv) { ErrorDiv.innerText = error; }
}

function MKEupdateUpstreamWritable() {
   if(MODELdata?.MKEwritable === undefined) { return; }
   const spinner = document.querySelector('.CARDloading-spinner');
   if(spinner) { spinner.remove(); }
   const EditableBtn = document.querySelector('.MKEmakeEditableTarg');
   if(!EditableBtn) { return; }
   if(MODELdata?.MKEwritable?.data?.writable === true) {
      EditableBtn.classList.remove('BUTTONdisabled');
      EditableBtn.classList.add('BUTTONdefault');
      EditableBtn.classList.add('MKEeditOrig');
   }
}