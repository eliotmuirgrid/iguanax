let DOC2sources=[];

function DOC2adapterIsSource(Tags){
   for (let i=0; i < Tags.length; i++){
      if (Tags[i] === "#doc_source"){
         return true;
      }
   }
   return false;
}

function DOC2adapterListRefresh(){
   DOC2sources = [];
   let L = MODELdata.DOCcomponents.data.component_data;
   for (let i=0; i < L.length; i++){
      if (DOC2adapterIsSource(L[i].tags)){
         COL_TRC(L[i]);
         DOC2sources.push(L[i]);
      }
   }
   COL_TRC(DOC2sources);
   return DOC2sources;
}

function DOC2adapterListHtml(){
   let H = '<div class="DOC2sourceList">';
   for (let i=0; i < DOC2sources.length; i++){
      H+= `<div class="DOC2source">${DOC2sources[i].name}</div>`;
   }
   return H;
}

function DOC2adapterFind(E){
   if (E.target.classList.contains('DOC2source')) {
      const sources = Array.from(E.currentTarget.querySelectorAll('.DOC2source'));
      const i = sources.indexOf(E.target);
      COL_TRC("Adapter " + DOC2sources[i].name);
      return i;
   } else {
      COL_TRC("No adapter triggered");
   }
   return -1;
}

function DOC2adapterActivate(E, CommandParameters){
   COL_TRC(E);
   let i = DOC2adapterFind(E);
   COL_TRC(i);
   if (i !== -1){
      let D = CommandParameters();
      D.component = DOC2sources[i].guid;
      D.target    = PAGEhashParameter("component");
      COL_TRC(D);
      APIcall("doc/source/query", D, function(R){
         DOC2adapterDone(R);
      });
   } else {
      COL_TRC("Missed adapter click.");
   }
}

function DOC2adapterDone(R){
   COL_TRC(R);
   if (!R.success){
      document.querySelector(".DOC2sourceList").innerText = R.error;
   } else {
      document.querySelector(".DOC2sourceList").innerHTML = R.data.result;
   }
}

function DOC2adapterRefresh(){
   MODELupdate("DOC2scrollBarModel");
   DOC2documentModelRefresh();
   //DOC2reloadPage();
}