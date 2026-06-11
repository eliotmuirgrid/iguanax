function PERlisteners(){
   let div = document.querySelector(".PERmodal");  
   if(!div) return;
   div.removeEventListener("click", PERclick);
   div.addEventListener   ("click", PERclick);
}

function PERclick(e){
   if(e.target.closest(".PERreconnect")) return PERreconnect(e.target);
}

function PERrowToIdx(target){
   let row = target.closest('.PERrow');
   const parentElement = row.closest(".PERruleTable");
   const logEntries = Array.from(parentElement.querySelectorAll('.PERrow'));
   const Index = logEntries.indexOf(row);
   return Index;
}

function PERidxToId(Idx){
   return PERserversData[Idx].guid;
}

