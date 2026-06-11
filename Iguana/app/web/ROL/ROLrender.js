let ROLopenedTo;

function ROLrender(){
   SETrender();
   document.querySelector('.ROLroles').classList.add('selected');
   ROLmodel();
}

function ROLrenderView(){
   let Data = ROLrolesData();
   document.querySelector('.SETcontentBody').innerHTML = ROLhtml(Data);
   ROLopenedTo = "";
}

function ROLaddButtonHtml(){
   let enabled = ROLeditPermission();
   let classes  = enabled ? "BUTTONaction" : "BUTTONdisabled";
   let hrefAttr = enabled ? `href="#settings/roles/add"` : "";
   let title    = enabled ? "" : `title="You do not have permission to edit this."`;
   return /*html*/`<a class="${classes} BUTTONstandard" ${title} ${hrefAttr}>ADD ROLE</a>`;
}

function ROLhtml(Data){
   return /*html*/`
   <div class="SETsettingsPaneLayout SETsettingsPane ROLmodal">
      <div class="ROLtitle">
         <div>Roles</div>
         <div class="SETlicenseActions BUTTONgroup">
            ${ROLaddButtonHtml()}
         </div>
      </div>
      <div class="SETpageDescription">
   Create, edit and remove roles. [<a href="${LINKsettingsRolesHeader}" target="_blank">Learn more</a>]</div>
      <div class="ROLcontent">${ROLroleDataHtml(Data)}
      </div>
   </div>`;
}

function ROLroleDataHtml(Data){
   if(Object.keys(Data).length == 0) return ROLnoRolesHtml();
   let Html = "";
   for(let role in Data){
      Html += ROLcardHtml(Data[role], role, false);
   } 
   return Html;
}

function ROLnoRolesHtml(){
   return `<div class="ROLcard">You do not have any roles defined so only the 'admin' user has permission to do anything.</div>`
}