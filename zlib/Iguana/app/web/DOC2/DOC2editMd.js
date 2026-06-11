function DOC2documentEditMdViewHtml(){
   return `<div class="DOC2mdContent"></div>`;
}

function DOC2documentEditMdHtml(){
   return `<textarea rows=6 class="DOC2mdEditInput DOC2editOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOC2mdEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOC2mdEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}

function DOC2documentMdEditPopulate(){
   document.querySelector(".DOC2mdContent").innerText = MODELdata.DOCdocument.data.markdown;
}

function DOC2documentMdView(){
   document.querySelector(".DOC2md").innerHTML = DOC2documentEditMdViewHtml();
   DOC2documentMdEditPopulate();
   document.querySelector(".DOC2editButton").addEventListener('click', function(E){ DOC2documentMdEdit();});
}

function DOC2documentMdEdit(){
   document.querySelector(".DOC2md").innerHTML = DOC2documentEditMdHtml();
   document.querySelector(".DOC2mdEditInput").value = MODELdata.DOCdocument.data.markdown;
   document.querySelector(".DOC2mdEditCancel").addEventListener('click',  function(E){ DOC2documentMdView();});
   document.querySelector(".DOC2mdEditConfirm").addEventListener('click', function(E){ DOC2documentMdConfirmEdit();});
}

function DOCdocumentMdEditExtract(){
   let Raw = document.querySelector(".DOC2mdEditInput").value;
   let NewMd = (Raw.trim() || "") !== "" ? Raw.trim() : MODELdata.DOCdocument.data.markdown;
   return NewMd;
}

function DOC2documentMdConfirmEdit(){
   let C = {};
   C.component  = PAGEhashParameter("component");
   C.created    = MODELdata.DOCdocument.data.meta.created;
   C.id         = MODELdata.DOCdocument.data.meta.id;
   C.markdown   = DOCdocumentMdEditExtract();
   C.overwrite  = true;
   //COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOC2documentMdConfirm(R); });
}

function DOC2documentMdConfirm(R){
   MODELupdate("DOC2scrollBarModel");
   DOC2documentModelRefresh();
   DOC2reloadPage();
}