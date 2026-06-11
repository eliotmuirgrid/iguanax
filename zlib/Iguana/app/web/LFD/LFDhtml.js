
function LFDselectHtml(){ 
   return /*html*/`
   <select class='LFDlineFeedSelect FORMselectBox'>
      <option value=${LFDlf}>LF</option>
      <option value=${LFDcr}>CR</option>
      <option value=${LFDcrlf}>CRLF</option>
   </select>
   <div class="TRANStoolbarButton LFDconvert" title='Convert Newlines'></div>
   <div class="TRANStoolbarButton LFDshowHide" title='Show/Hide Newlines'></div>`;
}


function LFDrender(){
   const div = document.querySelector(".TRANSwindowEdit");
   if(!div) return;
   if (SMPLeditingSampleData()) div.classList.add("LFDsampleData");
   else                         div.classList.remove("LFDsampleData");
}