function GINmoduleWarningHtml(){
   let Warning = GINmoduleWarning();
   if(!Warning) return "";
   return /*html*/`
   <div class="GINvalueEmpty">Error</div>
   Your .gitmodules file is corrupted, so the instance can’t commit and may behave unpredictably; <b>restart the instance</b> to regenerate the file.`;
}

function GINupdateModuleWarning(){
   let div = document.querySelector(".GINmoduleWarning");
   if(!div) return;
   let html = GINmoduleWarningHtml();
   if(div.innerHTML != html) div.innerHTML = html;
}