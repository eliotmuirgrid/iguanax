function GINview(){
   SETrender();
   GINrender();
   GINmodelInit();
}

function GINrender(){
   document.querySelector('.GINheader').classList.add('selected');
   let div = document.querySelector(".SETcontentBody");
   if(!div) return;
   let rollback = PAGEhashParameter("rollback");
   if(rollback) GINrevert();
   let skeleton = div.querySelector(".GINmodal");
   if(skeleton) return;
   div.innerHTML = GINskeletonHtml();
   GINlisteners();
}

function GINcollectionDescription(){
   return /*html*/`Optionally configure Git source control for your instance to simplify backups. Connect a Git repository to automatically push your changes and restore them if needed.
   [<a href="${LINKsettingsGitInstanceHeader}" target="_blank">Learn more</a>]`;
}

function GINskeletonHtml(){
   return /*html*/`
   <div class="GINmodal SETsettingsPane SETsettingsPaneLayout">
      <div class="GINtitle">
         <div class="">Manage Instance</div>
      </div>
      <div class="SETpageDescription">${GINcollectionDescription()}</div>
      <div class="GINuser GINdiv"></div>
      <br>

      <div class="GINrestore GINdiv">${GINrestoreHtml()}</div>
      <br>
      <!-- Revert functionality for Git Instance -->
      <!--<div class="GINrevert GINdiv">${GINrevertHtml()}</div>
      <br>-->
      <div class="GINkey GINdiv"></div>
      <br>
      <div class="GINauto GINdiv"></div>
      <br>
      <div class="GINrepo GINdiv"></div>
      <br>
      <div class="GINmoduleWarning"></div>
      <div class="GINwarning"></div>
      <div class="GINstatus"></div>
   </div>`;
}
