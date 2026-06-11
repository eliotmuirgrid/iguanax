function RPOselectOptionHtml(Address){
   return `<option>${Address}</option>`;
}

function RPOupdateHost(){
   let Address = RPOhostData();
   let div = document.querySelector(".RPOhost");
   if(!div) return;
   let Html = RPOselectOptionHtml(Address);
   if(div.innerHTML !== Html) div.innerHTML = Html;
}