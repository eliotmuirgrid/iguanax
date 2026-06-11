function DEPsetListeners(){
   let div = document.querySelector(".CORapp");
   if(!div) return;
   div.removeEventListener("click", DEPclick, true);
   div.addEventListener("click", DEPclick);
}

function DEPclick(e){
   if(e.target.closest(".DEPtooltip a")) DEPhandleClick(e);
}

function DEPhandleClick(e){
   let PathList = e.target.pathname.split('@');
   if (PathList.length !== 2) return;
   e.preventDefault();
   HLPjumpToFunction(PathList[1])
}