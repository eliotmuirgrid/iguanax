
function DOCimportButtonEvent(){
   document.querySelector(".DOCimport").addEventListener('click', function(E){DOCimportDialog();});
}

function DOCimportDialog(){
   DOCadapterListRefresh();
   let H = DOCadapterListHtml();
   H+= `<input class='DOCconfig' type='text'></div>`;
   POPpopoutOpen({title : "Import new documents", content : H, callback : DOCadapterRefresh});
   document.querySelector(".DOCsourceList").addEventListener('click', function(E){ DOCadapterActivate(E, DOCimportCommand)});
}

function DOCimportCommand(){
   let D = {};
   D.config = document.querySelector(".DOCconfig").value.trim();
   D.action = "import";

   return D;
}