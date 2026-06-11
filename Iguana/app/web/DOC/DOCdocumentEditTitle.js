function DOCdocumentEditTitleViewHtml(){
   return `<div class="DOCtitle"></div>  <img class="DOCeditTitle" src="DOC/DOCellipsis.svg">`;
}

function DOCdocumentEditTitleHtml(){
   return `<textarea rows=2 class="DOCtitleEditInput DOCeditOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOCtitleEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOCtitleEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}
function DOCdocumentTitleEditPopulate(){
   document.querySelector(".DOCtitle").innerHTML = MODELdata.DOCdocument.data.meta.source +": " + MODELdata.DOCdocument.data.meta.title;
}

function DOCdocumentTitleView(){
   document.querySelector(".DOCtitleSection").innerHTML = DOCdocumentEditTitleViewHtml();
    DOCdocumentTitleEditPopulate();
    document.querySelector(".DOCeditTitle").addEventListener('click', function(E){ DOCdocumentTitleEdit();});
}

function DOCdocumentTitleEditExtract(){
   let Raw = document.querySelector(".DOCtitleEditInput").value;
   let NewTitle = (Raw.trim() || "") !== "" ? Raw.trim() : MODELdata.DOCdocument.data.meta.title;
   return NewTitle;
}

async function DOCdocumentTitleConfirm(R){
   COL_TRC(R);
   await DOCsearch();
   DOCmodelRefresh();
   DOCdocumentTitleView();
}

function DOCdocumentTitleConfirmEdit(){
   let C = {};
   C.component  = PAGEhashParameter("component");
   C.created    = PAGEhashParameter("created");
   C.id         = PAGEhashParameter("id");
   C.title      = DOCdocumentTitleEditExtract();
   C.overwrite  = true;
   COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOCdocumentTitleConfirm(R); });
}

function DOCdocumentTitleEdit(){
   COL_TRC("Edit title");
   document.querySelector(".DOCtitleSection").innerHTML = DOCdocumentEditTitleHtml();
   document.querySelector(".DOCtitleEditInput").value = MODELdata.DOCdocument.data.meta.title;
   document.querySelector(".DOCtitleEditCancel").addEventListener('click',  function(E){ DOCdocumentTitleView();});
   document.querySelector(".DOCtitleEditConfirm").addEventListener('click', function(E){ DOCdocumentTitleConfirmEdit();});
}