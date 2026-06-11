// let ECNrenderedSources =  {};

function ECNsourcesUpdate(){
   let div = document.querySelector(".ECNsourcesList");
   if(!div) return;
   let Html = ECNsourcesHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function ECNsourceRowHtml(Component, Name){
   let Delete = /*html*/`
   <div class="BUTTONcancel ECNdeleteSource ECNdelete">
      <img src="/DASH/DASHclearSearch.svg">
   </div>`;
   if(!ECNeditPermission(Component)) Delete = "";
   return /*html*/`
   <div class="ECNrow">
      <div class="ECNrowSourceChild">
         <span class="ECNsourceName">${Name}</span>
         <span class="ECNsourceLink">
            [<a href="#dashboard/connections?component=${Component}">Connections</a>]
         </span>
      </div>
      <div class="ECNdeletetableCell">${Delete}</div>
   </div>`;
}

function ECNsourcesHtml(){
   let Data = ECNsourcesData();
   let Html = "";
   if (Data.length === 0) {
      Html = '<div class="ECNemptySources">No sources found</div>';
   } else {
      for (let Component of Data) {
         let Name = ECNname(Component);
         Html += ECNsourceRowHtml(Component, Name);
      }
   }
   return Html;
}