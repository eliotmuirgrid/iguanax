
function ECNreceiversUpdate(){
   let div = document.querySelector(".ECNreceiversList");
   if(!div) return;
   let Html = ECNreceiversHtml();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function ECNreceiverRowHtml(Component, Name){
   let Delete = /*html*/`
   <div class="BUTTONcancel ECNdeleteReceiver ECNdelete">
      <img src="/DASH/DASHclearSearch.svg">
   </div>`;
   if(!ECNeditPermission(Component)) Delete = "";
   return /*html*/`
   <div class="ECNrow">
      <div class="ECNrowReceiverChild">
         <span class="ECNreceiverName">${Name}</span>
         <span class="ECNreceiverLink">
            [<a href="#dashboard/connections?component=${Component}">Connections</a>]
         </span>
      </div>
      <div class="ECNdeletetableCell">${Delete}</div>
   </div>`;
}

function ECNreceiversHtml(){
   let Data = ECNreceiversData();
   let Html = "";
   if (Data.length === 0) {
      Html = '<div class="ECNemptyReceivers">No destinations found</div>';
   } else {
      for (let Component of Data) {
         let Name = ECNname(Component);
         Html += ECNreceiverRowHtml(Component, Name);
      }
   }
   return Html;
}