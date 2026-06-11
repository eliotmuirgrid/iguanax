function DOC2documentEditTitleHtml(){
   return `<textarea rows=2 class="DOC2titleEditInput DOC2editOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOC2titleEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOC2titleEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}

function DOC2reloadTitle(Doc){
   document.querySelector(".DOC2titleSection").innerHTML = DOC2documentTitleHtml();
   document.querySelector(".DOC2title").innerHTML = Doc.meta.source + ": " + Doc.meta.title;
}
function DOC2documentTitleEdit(){
   COL_TRC("Edit title");
   let Doc = MODELdata.DOCdocument.data;
   document.querySelector(".DOC2titleSection").innerHTML = DOC2documentEditTitleHtml();
   document.querySelector(".DOC2titleEditInput").value = Doc.meta.title;
   document.querySelector(".DOC2titleEditCancel").addEventListener('click',  function(E){ DOC2reloadTitle(Doc); });
   document.querySelector(".DOC2titleEditConfirm").addEventListener('click', function(E){ DOC2documentTitleConfirmEdit(Doc);});
}

function DOC2documentTitleEditExtract(Doc){
   let Raw = document.querySelector(".DOC2titleEditInput").value;
   let NewTitle = (Raw.trim() || "") !== "" ? Raw.trim() : Doc.meta.title;
   return NewTitle;
}

function DOC2documentTitleConfirmEdit(Doc){
   let C = {};
   C.component  = PAGEhashParameter("component");
   C.created    = Doc.meta.created;
   C.id         = Doc.meta.id;
   C.title      = DOC2documentTitleEditExtract(Doc);
   C.overwrite  = true;
   COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOC2documentTitleConfirm(R); });
}

function DOC2reloadPage(){
   let Args = {
      component : PAGEhashParameter("component"),
      seetags   : PAGEhashParameter("seetags"),
      Search    : PAGEhashParameter("search"),
      tag       : PAGEhashParameter("tag"),
      id        : MODELdata.DOCdocument.data.meta.id,
      created   : MODELdata.DOCdocument.data.meta.created
   }
   let hash = `#documents2${PAGEformatParams(Args)}`;
   window.location.hash = hash;
}

function DOC2documentTitleConfirm(R){
   //COL_TRC(R);
   MODELupdate("DOC2scrollBarModel");
   DOC2documentModelRefresh();
   DOC2reloadPage();
}