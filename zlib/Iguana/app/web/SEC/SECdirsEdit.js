function SECaddRowHtml(){
   return /*html*/`<div class="SECadd">
      <svg class="SETaddIcon" width="14" height="14">
         <use href="SET/SETiconAdd.svg#SETiconAdd"></use>
      </svg>
      PATH
   </div>`;
}

function SECrowEditInputHtml(Path){
   return /*html*/`
   <div class="SETdatabaseTableValue FORMinputGroup">
      <input class="FORMinput SANDeditValue" type="text" value="${ESChtmlEscape(Path)}" placeholder="Enter a directory path">
      <div class="SANDbrowse BUTTONstandard BUTTONcancel">BROWSE...</div>
   </div>`;
}

function SECrowEditHtml(Path){
   return /*html*/`
   <div class="SECrow">
      ${SECrowEditInputHtml(Path)}
      <div class="FORMbutton BUTTONcancel BUTTONwarning SANDdelete">
         ${SETtrashIcon()}
      </div>
   </div>`;
}

function SECrowsEditHtml(Dirs){
   let Html = "";
   for(dir of Dirs) Html += SECrowEditHtml(dir);
   if(Dirs.length === 0) Html += SECrowEditHtml("");
   return Html;
}

function SECtableEditHtml(Dirs){
   let Rows = SECrowsEditHtml(Dirs);
   return /*html*/`   
   <div class="SECdirsTable">
      <div class="SECdirHeading">
         <div class="SECheading SANDpaddingLeft">Directories Allowed to be Accessed:</div>
      </div>
      ${Rows}
   </div>`
}

function SECeditRowsHtml(Dirs, DirsEnabled, OSExEnabled){
   let UseAccess = SECuseAccessHtml(DirsEnabled, OSExEnabled, true);
   let DirsHtml  = SECtableEditHtml(Dirs);
   let AddRow    = SECaddRowHtml();
   return /*html*/`
   ${UseAccess}
   <br>
   ${DirsHtml}
   ${AddRow}`;
}
