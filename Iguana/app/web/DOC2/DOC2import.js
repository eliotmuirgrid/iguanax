
function DOC2importDialog(){
   DOC2adapterListRefresh();
   let H = DOC2adapterListHtml();
   H+= `<input class='DOC2config' type='text'></div>`;
   POPpopoutOpen({title : "Import new documents", content : H, callback : DOC2adapterRefresh});
   document.querySelector(".DOC2sourceList").addEventListener('click', function(E){ DOC2adapterActivate(E, DOC2importCommand)});
}

function DOC2importCommand(){
   let D = {};
   D.config = document.querySelector(".DOC2config").value.trim();
   D.action = "import";
   return D;
}