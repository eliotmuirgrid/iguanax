function DOCdocumentEditMdViewHtml(){
   return `<div class="DOCmdContent"></div>`;
}

function DOCdocumentEditMdHtml(){
   return `<textarea rows=6 class="DOCmdEditInput DOCeditOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOCmdEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOCmdEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}

function DOCdocumentMdEditPopulate(){
   document.querySelector(".DOCmdContent").innerText = MODELdata.DOCdocument.data.markdown;
}

function DOCdocumentMdView(){
   document.querySelector(".DOCmd").innerHTML = DOCdocumentEditMdViewHtml();
   DOCdocumentMdEditPopulate();
   document.querySelector(".DOCeditButton").addEventListener('click', function(E){ DOCdocumentMdEdit();});
}

function DOCdocumentMdEdit(){
   COL_TRC("Edit md");
   document.querySelector(".DOCmd").innerHTML = DOCdocumentEditMdHtml();
   document.querySelector(".DOCmdEditInput").value = MODELdata.DOCdocument.data.markdown;
   document.querySelector(".DOCmdEditCancel").addEventListener('click',  function(E){ DOCdocumentMdView();});
   document.querySelector(".DOCmdEditConfirm").addEventListener('click', function(E){ DOCdocumentMdConfirmEdit();});
}

function DOCdocumentMdEditExtract(){
   let Raw = document.querySelector(".DOCmdEditInput").value;
   let NewMd = (Raw.trim() || "") !== "" ? Raw.trim() : MODELdata.DOCdocument.data.markdown;
   return NewMd;
}

function DOCdocumentMdConfirmEdit(){
   let C = {};
   C.component  = PAGEhashParameter("component");
   C.created    = PAGEhashParameter("created");
   C.id         = PAGEhashParameter("id");
   C.markdown   = DOCdocumentMdEditExtract();
   C.overwrite  = true;
   //COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOCdocumentMdConfirm(R); });
}

async function DOCdocumentMdConfirm(R){
   COL_TRC(R);
   await DOCsearch();
   DOCmodelRefresh();
   DOCdocumentMdView();
}