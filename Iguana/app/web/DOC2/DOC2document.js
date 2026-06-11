function DOC2documentParams(){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   const selected = document.querySelector('.DOC2list .DOC2listRowSelected');
   if (!selected) return Args;
   let Idx = DOC2listRowIndex(selected);
   let RowData = DOCshowListData[Idx];
   Args.id = RowData.id;
   Args.created = RowData.created;
   return Args;
}

function DOC2searchParams(){
   let D = {};
   D.component = PAGEhashParameter("component");
   D.search    = PAGEhashParameter("search") || "";
   return D
}

function DOC2documentUpdate(){
   COL_TRC("Document update");
   if (!MODELdata.DOCdocument.success) {
      COL_TRC("No document loaded.");
      document.querySelector(".DOC2content").innerHTML = "No document loaded.";
   } else {
      DOC2documentDisplay(MODELdata.DOCdocument.data); 
   }
}

function DOC2documentDisplay(Doc){
   // An edit box is open
   if(document.querySelector(".DOC2editOpen")) return;
   document.querySelector(".DOC2date").innerText       = DOC2convertDate(Doc.meta.created);
   document.querySelector(".DOC2title").innerText      = Doc.meta.source + ": " +Doc.meta.title;
   document.querySelector(".DOC2tagBarTags").innerHTML = DOC2transformTags(Doc.meta.tags);
   DOC2relationsPopulate(Doc);
   DOC2renderContent(Doc); 
   DOC2tagCloud();
}

function DOC2transformTags(Tags){
   let HTags = [];
   for (let i=0; i < Tags.length; i++){
      HTags.push("#" + Tags[i]);
   }
   let R = HASHTAGrenderTags(HTags, "DOC2tagDiv");
   return R;
}

function DOC2replaceInnerTextIfChanged(Element, Value){
   if (Element.innerText !== Value){
      Element.innerText = Value;
   }
}