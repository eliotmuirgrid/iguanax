function GITHbitbucketSetupScreen(){
   SETrender();
	document.querySelector('.GITHgitHost').classList.add('selected');
   document.querySelector('.SETcontentBody').innerHTML = GITHhtml();
   GITHlisteners();
   GITHinitModel();
   GITHinitSlowModel();
}

function GITHhtml(){
   return /*html*/`
   <div class="GITSpane SETsettingsPane">
      <div class="GITStitle">Git</div>
      <div class="SETpageDescription">
      Configure Git settings sequentially for proper setup. Make sure each step is completed before proceeding to the next. [<a target="_blank" href="${LINKsettingsGitHeader}">Learn more</a>]</div>
      <div class="GITHmodal">
         ${GITHcontentHtml()}
      </div>
   </div>`;
}

function GITHcontentHtml(){
   return /*html*/`
   <div class="GITHgrid">
      ${GITHusernameAndEmailHtml()}
      ${GITHserviceHtml()}
      <div class="GITHusernameTarget">
         ${GITHusernameHtml("Bitbucket")}
      </div>
      ${GITHappPasswordHtml("Bitbucket")}
      ${GITHorganizationsHtml()}
      ${GITHkeyFilesHtml()}
      ${GITHsshHtml()}
   </div>`;
}

function GITHnotSet(Message="Not set"){
   return /*html*/`<div class="GITHnotSet">${Message}</div>`;
}

function GITHsetError(selector, error){
   let div = document.querySelector(`${selector}`);
   if(!div) return;
   let ErrorDiv = div.querySelector(".GITHerror");
   if(!ErrorDiv) return;
   if(ErrorDiv.innerHTML != error) ErrorDiv.innerHTML = error;
}

function GITHresetTokenFields(){
   GITHupdateValue(document.querySelector(".GITHtokenVal"), "...");
   STAupdateTargetClass(".GITHappPassword .GITHtitle", "GITHwarning", false);
}

function GITHresetSshFields(){
   GITHupdateValue(document.querySelector(".GITHsshModal .GITHtableVal"), "...");
   STAupdateTargetClass(".GITHsshModal .GITHtitle",    "GITHwarning", false);
}