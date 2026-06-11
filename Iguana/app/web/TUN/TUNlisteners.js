function TUNlistenersInit(){
   let Div = document.querySelector('.TUNlistView');
   if(!Div) return;
   Div.addEventListener('click', TUNclick);
   Div.addEventListener('dblclick', TUNdblclick);
}

function TUNclick(e){
   if(e.target.closest('.TUNaddPeer'))          return location.hash = "cluster/peer/add"; 
   if(e.target.closest('.TUNaddFromUrlButton')) return location.hash = "cluster/peer/info";
}

function TUNdblclick(e){
   let Row = e.target.closest('.TUNcomponentRow');
   let RowIdx = TUNrowIndex(Row);
   let Name = TUNrenderedRows[RowIdx];
   if(Name) location.hash = `#cluster/peer?name=${Name}`;
}

function TUNrowIndex(targetRow){ return targetRow ? Array.from(targetRow.parentElement.children).indexOf(targetRow) : -1; }
