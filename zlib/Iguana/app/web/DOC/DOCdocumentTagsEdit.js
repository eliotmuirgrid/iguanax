function DOCdocumentEditTagsViewHtml(){
   return `<div class="DOCtagBarTags"></div>  <img class="DOCeditTags" src="DOC/DOCellipsis.svg">`;
}

function DOCdocumentEditTagsValue(Tags){
   let H = '';
   for (let i=0; i < Tags.length; i++){
      H+= " #" + Tags[i];
   }
   return H.substring(1);
}

// TODO - quite reckless having code referencing the CARD svg - fragile code warning.
function DOCdocumentEditTagsHtml(){
   return `<textarea rows=5 class="DOCtagsEditInput DOCeditOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOCtagEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOCtagEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}

function DOCdocumentTagEditPopulate(){
   document.querySelector(".DOCtagBarTags").innerHTML = DOCtransformTags(MODELdata.DOCdocument.data.meta.tags);
}

function DOCdocumentTagsView(){
   document.querySelector(".DOCdocumentTagBar").innerHTML = DOCdocumentEditTagsViewHtml();
   DOCdocumentTagEditPopulate();
   document.querySelector(".DOCeditTags").addEventListener('click', function(E){ DOCdocumentTagsEdit();});1
}

function DOCdocumentTagsEditExtract(){
   let Raw = document.querySelector(".DOCtagsEditInput").value;
   COL_TRC(Raw);
   let Tags = Raw.split(" ");
   COL_TRC(Tags);
   for (let i=0; i < Tags.length; i++){
      Tags[i] = Tags[i].trim();
      Tags[i] = Tags[i].substring(1);
   }
   return Tags;
}


async function DOCdocumentTagsEditConfirm(R){
   COL_TRC(R);
   await DOCsearch();
   DOCmodelRefresh();
   DOCdocumentTagsView();
}

function DOCdocumentTagsConfirmEdit(){
   let C = {};
   C.component = PAGEhashParameter("component");
   C.created   = PAGEhashParameter("created");
   C.id        = PAGEhashParameter("id");
   C.tags      = DOCdocumentTagsEditExtract();
   C.overwrite = true;
   COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOCdocumentTagsEditConfirm(R); });
}

function DOCdocumentTagsEdit(){
   COL_TRC("Edit tags");
   document.querySelector(".DOCdocumentTagBar").innerHTML = DOCdocumentEditTagsHtml();
   document.querySelector(".DOCtagsEditInput").value = DOCdocumentEditTagsValue(MODELdata.DOCdocument.data.meta.tags);
   document.querySelector(".DOCtagEditCancel").addEventListener('click',  function(E){ DOCdocumentTagsView();});
   document.querySelector(".DOCtagEditConfirm").addEventListener('click', function(E){ DOCdocumentTagsConfirmEdit();});
}