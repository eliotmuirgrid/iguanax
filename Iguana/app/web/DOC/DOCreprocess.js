function DOCreprocessDialog(){
   COL_TRC("Reprocess dialog");
   DOCadapterListRefresh();
   let H = DOCadapterListHtml();
   POPpopoutOpen({
      title : "Reprocess markdown from source", 
      content : H,
      callback : DOCadapterRefresh
   });
   document.querySelector(".DOCsourceList").addEventListener('click', function(E){
      DOCadapterActivate(E, DOCreprocessCommand); 
   });
   COL_TRC("Registered click handler.");
}

function DOCreprocessCommand(){
   let D = {}
   D.action    = "regenerate_markdown";
   D.id        = PAGEhashParameter("id");
   D.created   = PAGEhashParameter("created");
   return D;
}
