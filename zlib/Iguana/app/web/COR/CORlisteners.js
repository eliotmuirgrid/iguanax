function CORsetListeners(){
   let div = document.querySelector(".CORapp");
   if(!div) return;
   div.removeEventListener("change", CORchange);
   div.addEventListener   ("change", CORchange);
}

function CORchange(e){
   if(e.target.closest(".THMradioContainer")) return THMcacheRadio();
}