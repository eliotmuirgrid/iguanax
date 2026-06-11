function STAgitDetectHtml(){
   return /*html*/`
   <div class="SETversionRow">
      <div class="SETversionLabel STAgitDetected">Git:</div>
      <div class="STAgitInstalled">...</div>
   </div>
   <div class="STAgitInfo">
      <div class="SETversionRow">
         <div class="SETversionLabel">Git Version:</div>
         <div class="STAgitVersion">...</div>
      </div>
   </div>`;
}

function STAsshDetectHtml(){
   return /*html*/`
   <div class="SETversionRow">
      <div class="SETversionLabel STAsshDetected">SSH:</div>
      <div class="STAsshPath">...</div>
   </div>
   <div class="SETversionRow">
      <div class="SETversionLabel STAsshKeygenDetected">Ssh-keygen:</div>
      <div class="STAkeygenPath">...</div>
   </div>
   `;
}

function STAgitHelpHtml(){
   return /*html*/`
   <div class="STAwarningText">Please install Git and/or make sure it is on the system path. It may require restarting the Iguana service. [<a class="SETlink" href=${LINKwarningGitDetection} target="_blank">Learn more</a>]</div>`;
}

function STAsshHelpHtml(Ssh){
   return Ssh ? Ssh : /*html*/`
   <div class="STAwarningText">Could not find ssh on the command line. It is needed to authenticate with git cloud providers.</div>`;
}

function STAsshKeygenHtml(SshKeygen){
   return SshKeygen ? SshKeygen : /*html*/`
   <div class="STAwarningText">Could not find ssh-keygen on the command line. It is needed to generate SSH keys. </div>`;
}

function STArenderGitSshInfo(){
   let Data = STAgitSshDetectionData();
   STAupdateTarget(".STAgitInstalled", Data?.installed ?  Data?.git_path : STAgitHelpHtml());
   STAupdateTarget(".STAgitVersion",   Data?.version  || ""); 
   STAupdateTarget(".STAsshPath",      STAsshHelpHtml  (Data?.ssh_path       || "")); 
   STAupdateTarget(".STAkeygenPath",   STAsshKeygenHtml(Data?.sshkeygen_path || "")); 
}

function STAupdateTarget(selector, value){
   let div = document.querySelector(`${selector}`);
   if(!div) return;
   if(div.innerHTML != value) div.innerHTML = value;
}

function STAupdateTargetClass(targetSelector, addedClass, flag){
   let div = document.querySelector(`${targetSelector}`);
   if(!div) return;
   if(flag) div.classList.add   (addedClass);
   else     div.classList.remove(addedClass);
}