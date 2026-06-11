function DOC2documentListeners(){
   document.querySelector(".DOC2editTitle").addEventListener      ("click", DOC2documentTitleEdit);
   document.querySelector(".DOC2editTags").addEventListener       ("click", DOC2documentTagsEdit);
   document.querySelector(".DOC2tagBarTags")?.addEventListener     ("click", DOC2tagClick);
   document.querySelector(".DOC2relations")?.addEventListener    ("click", DOC2relationClick);
   document.querySelector(".DOC2editButton").addEventListener     ('click', DOC2documentMdEdit);
   document.querySelector(".DOC2downloadButton") .addEventListener('click', DOC2documentDownload);
   document.querySelector(".DOC2reprocessButton").addEventListener('click', DOC2reprocessDialog);
   document.querySelector(".DOC2deleteButton").addEventListener   ('click', DOC2deleteDialog);
   document.querySelector(".DOC2linkButton").addEventListener      ('click', DOC2copyLink);
   DOC2treeTagsEventRegister();
}

async function DOC2copyLink(E){
   let Doc = MODELdata.DOCdocument.data.meta;
   const Data = { created : Doc.created, id: Doc.id, component: PAGEhashParameter("component") };
   const Base = window.location.origin + "#documents2";
   let Url = Base + PAGEformatParams(Data);
   try {
      await navigator.clipboard.writeText(Url);
      let ToolTip = POPtooltip({
         target: E.target,
         content: `<div class="DOC2copyPopout">Copied!</div>`,
         position: POPtooltipPosition.BOTTOM
      });
      E.target.addEventListener('mouseleave', function(){ ToolTip.remove(); });
    } catch (err) {
      console.error("Failed to copy: ", err);
    }
}

function DOC2documentDownload(){
   let Doc = MODELdata.DOCdocument.data.meta;
   const Data = { created : Doc.created, id: Doc.id, component: PAGEhashParameter("component") };
   let Url = "doc/download" + PAGEformatParams(Data);
   window.open(Url, "_blank");
}

function DOC2reprocessDialog(){
   DOC2adapterListRefresh();
   let H = DOC2adapterListHtml();
   POPpopoutOpen({
      title : "Reprocess markdown from source", 
      content : H,
      callback : DOC2adapterRefresh
   });
   document.querySelector(".DOC2sourceList").addEventListener('click', function(E){
      DOC2adapterActivate(E, DOC2reprocessCommand); 
   });
}

function DOC2reprocessCommand(){
   let D = {}
   D.action    = "regenerate_markdown";
   D.id        = MODELdata.DOCdocument.data.meta.id;
   D.created   = MODELdata.DOCdocument.data.meta.created;
   return D;
}

function DOC2tagClick(E){
   const Target = E.target;
   if (Target.closest(".DOC2tagDiv")){
      let Idx = DOC2addressOf(Target, "DOC2tagDiv");
      let Data = MODELdata.DOCdocument.data.meta.tags;
      let Tag = Data[Idx[0]];
      DOC2tagAddTag("DOC2searchInput", Tag);
   }
}

function DOC2relationClick(E){
   const Target = E.target;
   if (Target.closest(".DOC2relationDiv")){
      let Idx = DOC2addressOf(Target, "DOC2relationDiv");
      let Data = MODELdata.DOCdocument.data.meta.relations;
      let SelectedRelation = Data[Idx];
      PAGEsetHashParameter("created",SelectedRelation.created);
      PAGEsetHashParameter("id",SelectedRelation.id);
   }
}


function DOC2addressOf(node, className) {
   const path = [];
   let n = node;
   while (n?.classList?.contains(className)) {
     const p = n.parentElement;
     const sibs = [...p.children].filter(el => el.classList.contains(className));
     path.unshift(sibs.indexOf(n));
     n = p.closest('.' + className);
   }
   return path;
}

function DOC2tagAddTag(ClassName, Tag){
   let Div = document.querySelector("."+ClassName);
   if(!Div || !Tag) return;
   let Term = Div.value.trim();
   if (Term.length > 0){
      Div.value = Term + ' OR #' + Tag;   // TODO eliminate duplicates.
   } else {
      Div.value = "#" + Tag;
   }
}