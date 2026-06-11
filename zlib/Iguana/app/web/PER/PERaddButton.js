function PERupdateAddButton(){
   let div = document.querySelector(".PEReditTarget");
   if(!div) return;
   let Html = PERaddButton();
   if(div.innerHTML !== Html) div.innerHTML = Html;
}

function PERaddButton(){
   let Edit    = PEReditPermission();
   let Classes =  Edit ? "BUTTONaction" : "BUTTONdisabled";
   let title   = !Edit ? ` title="You do not have permission to edit this."` : "";
   let href    =  Edit ? ` href="#settings/peers/add"` : "";
   return /*html*/`
   <a class="BUTTONstandard BUTTONdefault" href="#settings/peers/myinfo">MY INFO</a>
   <a class="${Classes} BUTTONstandard"${title}${href}>ADD PEER</a>
   `;
}
