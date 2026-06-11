function FILbrowserHTML(config) {
    return `
    <div class="FIL-browser">
      <div class="FIL-header"></div>
      <div class="FIL-controls">
        ${FILbrowserControlsHtml()}
        ${FILbrowserNewFolder(config)}
      </div>
      <div class="FIL-list"></div>
      <div class="FIL-footer">
        <div class="FIL-pathfield-row">
          <input class="FIL-pathfield" type="text" readonly value="" />
          <img class="FILpathCopy" src="/FIL/images/copyIcon.svg" title="Copy path to clipboard" alt="Copy"/>
        </div>
        <div class="BUTTONstandard BUTTONcancel FIL-cancel">CANCEL</div>
        <div class="BUTTONstandard BUTTONaction FIL-primary FIL-select" disabled>SELECT</div>
      </div>
    </div>
    `;
}

function FILbrowserControlsHtml() {
    return `
<button class="FIL-btn FIL-backbtn" title="Back" disabled>
    <svg width="16" height="16" viewBox="0 0 16 16"><path d="M10.5 13L5.5 8L10.5 3" stroke="var(--blue)" stroke-width="1.7" fill="none" stroke-linecap="round" stroke-linejoin="round"/></svg>
</button>
<button class="FIL-btn FIL-nextbtn" title="Next" disabled>
    <svg width="16" height="16" viewBox="0 0 16 16"><path d="M5.5 3l5 5-5 5" stroke="var(--blue)" stroke-width="1.7" fill="none" stroke-linecap="round" stroke-linejoin="round"/></svg>
</button>
<input class="FIL-searchfield" type="text" placeholder="Search Directory…" autocomplete="off" />`;
}

function FILbrowserNewFolder(config) {
    return config.new_folder ?
       `<a class="FIL-btn FIL-newfolder" title="New Folder">
         <svg class="FILiconAddFolder" xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 16 16">
           <use href="FIL/images/FILiconAddFolder.svg#FILiconAddFolder"></use>
         </svg>
       </a>`
       : ``;
}