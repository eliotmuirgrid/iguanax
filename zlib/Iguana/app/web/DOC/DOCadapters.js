let DOCsources=[];

function DOCadapterIsSource(Tags){
   for (let i=0; i < Tags.length; i++){
      if (Tags[i] === "#doc_source"){
         return true;
      }
   }
   return false;
}


function DOCadapterListRefresh(){
   DOCsources = [];
   let L = MODELdata.DOCcomponents.data.component_data;
   for (let i=0; i < L.length; i++){
      if (DOCadapterIsSource(L[i].tags)){
         COL_TRC(L[i]);
         DOCsources.push(L[i]);
      }
   }
   COL_TRC(DOCsources);
   return DOCsources;
}

function DOCadapterListHtml(){
   let H = '<div class="DOCsourceList">';
   for (let i=0; i < DOCsources.length; i++){
      H+= `<div class="DOCsource">${DOCsources[i].name}</div>`;
   }
   return H;
}

function DOCadapterFind(E){
   if (E.target.classList.contains('DOCsource')) {
      const sources = Array.from(E.currentTarget.querySelectorAll('.DOCsource'));
      const i = sources.indexOf(E.target);
      COL_TRC("Adapter " + DOCsources[i].name);
      return i;
   } else {
      COL_TRC("No adapter triggered");
   }
   return -1;
}

function DOCadapterActivate(E, CommandParameters){
   COL_TRC(E);
   let i = DOCadapterFind(E);
   COL_TRC(i);
   if (i !== -1){
      let D = CommandParameters();
      D.component = DOCsources[i].guid;
      D.target    = PAGEhashParameter("component");
      COL_TRC(D);
      APIcall("doc/source/query", D, function(R){
         DOCadapterDone(R);
      });
   } else {
      COL_TRC("Missed adapter click.");
   }
}

function DOCadapterDone(R){
   COL_TRC(R);
   if (!R.success){
      document.querySelector(".DOCsourceList").innerText = R.error;
   } else {
      document.querySelector(".DOCsourceList").innerHTML = R.data.result;
   }
}

async function DOCadapterRefresh(){
   await DOCsearch();
   DOCmodelRefresh();
}