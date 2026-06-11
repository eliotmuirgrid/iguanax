function EDIrowCollapse(Row){
   let Rows = Row.querySelectorAll(".EDInodeRowDiv");
   for(let i = 0; i < Rows.length; i++){
      Rows[i].remove();
   }
}

function EDIdetermineDepth(Row){
   let Depth = 1;
   let Div = Row.parentNode.closest(".EDInodeRowDiv");
   while(Div && Div.closest(".EDInodeRowDiv")){
      Div = Div.parentNode.closest(".EDInodeRowDiv");
      Depth = Depth + 1;
   }
   return Depth;
}

function EDIrowExpand(Row, FileName){
   let Div = Row.querySelector(".EDInodeValue");
   let Val = Div?.getAttribute('value');
   let Depth = EDIdetermineDepth(Row);
   let Html = EDIchildRowsHtml(Val, FileName, "", Depth, 0);
   Row.insertAdjacentHTML('beforeend', Html);
}