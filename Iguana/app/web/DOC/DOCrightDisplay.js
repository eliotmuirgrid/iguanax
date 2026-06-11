function DOCrightDisplayEvent(){
   document.querySelector(".DOCrightDisplay").addEventListener('click', function(E){ DOCrightDisplayClick(E); });
}

function DOCrightDisplayClick(E){
   const Target = E.target;
   if (Target.closest('.DOCtagDiv')){ return DOCtagDivClick(Target); }  
   if (Target.closest('.DOCrelationDiv')){ return DOCrelationDivClick(Target); }  
}

function DOCmetaTagData(){
   return MODELdata.DOCdocument.data.meta.tags;
}

function DOCtagDivClick(Target){
   let Idx = DOCaddressOf(Target, "DOCtagDiv");
   COL_TRC(Idx);
   let Data = DOCmetaTagData() || [];
   let Tag = Data[Idx];
   COL_TRC(Tag);
   DOCtagAddTag("DOCsearchInput", Tag);
   DOCsearch();
}

function DOCrelationDivClick(Target){
   let Idx = DOCaddressOf(Target, "DOCrelationDiv");
   let Created = MODELdata.DOCdocument.data.meta.created;
   let Id = MODELdata.DOCdocument.data.meta.relations[Idx].id;
   PAGEsetHashParameter("id",Id);
   PAGEsetHashParameter("created",Created);
   PAGEsetHashParameter("search","");
}

function DOCtagAddTag(ClassName, Tag){
   let Div = document.querySelector("."+ClassName);
   if(!Div || !Tag) return;
   let Term = Div.value.trim();
   if (Term.length > 0){
      Div.value = Term + ' OR #' + Tag;   // TODO eliminate duplicates.
   } else {
      Div.value = "#" + Tag;
   }
}