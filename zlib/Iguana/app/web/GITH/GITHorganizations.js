function GITHupdateOrganizations(){
   let Data = GITHowners();
   let list = GITHownersListHtml(Data?.owners);
   let Source = GITHsourceInfoHtml(Data?.organizations, Data?.owners);
   GITHupdateValue(document.querySelector(".GITHorganizationList"), list);
   GITHupdateValue(document.querySelector(".GITHorganizationSource"), Source);
}

function GITHorgsWarning(){
   let Data = GITHowners();
   let list = Data?.owners || [];
   return !list.length;
}

function GITHownersListHtml(Data){
   if(!Data || Data.length == 0) return GITHemptyOwners();
   let list = Data.join(", ");
   return /*html*/`<div class="GITHsuccessMark" title="Found a list of available organizations.">${list}</div>`;
}

function GITHsourceInfoHtml(Organizations, List){
   if(!List || List.length == 0) return "";
   return Organizations ?  "Custom list you entered - organizations.json" : "Auto-generated from your Git token - owners.json";
}

function GITHemptyOwners(){
   return /*html*/`<div class="GITHnotSet">No organizations found. Click Edit to add them.</div>`;
}

function GITHorganizationsHtml(){
   let Link = "";
   let LinkHtml = Link ? /*html*/`[<a class="SETlink" target="_blank" href="${Link}">...</a>]` : "";
   return /*html*/`
   <div class="GITHbox GITHorganization">
      <div class="GITHtitleRow">
         <div class="GITHtitle">Git Organizations</div>
         <div class="GITHdesc">Displays the organizations your Git token can access. If none appear, enter them manually.
         ${LinkHtml}
         </div>
         <div class="GITHedit BUTTONdefault BUTTONstandard">EDIT</div>
      </div>
      <div class="GITHtable">
         <div class="GITHtableTitle">Organizations:</div>
         <div class="GITHtableVal GITHorganizationList"></div>
         <div class="GITHtableTitle">Source:</div>
         <div class="GITHtableVal GITHorganizationSource"></div>
      </div>
   </div>`;
}