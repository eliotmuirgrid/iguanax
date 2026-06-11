function DOCdeleteWarningHtml(){
   return /*html*/ `
   <div class="DOCdeleteWarning">
      <span>Are you sure you want to delete the document: "${MODELdata.DOCdocument.data.meta.title}"</span>
      <div class="DOCdeleteButtons">
         <button class="BUTTONaction BUTTONstandard DOCdeleteConfirm">CONFIRM</button>
         <button class="BUTTONstandard BUTTONdefault DOCdeleteCancel">CANCEL</button>
      </div>
   </div>`;
}

function DOCdeleteDialog(){
   COL_TRC("Reprocess dialog");
   DOCadapterListRefresh();
   let H = DOCdeleteWarningHtml();
   let DOCconfirmDeletePop = POPpopoutOpen({
      title : "Delete document", 
      content : H,
      callback : DOCadapterRefresh
   });
   document.querySelector(".DOCdeleteConfirm").addEventListener('click', function(E){
     DOCdelete(E); 
   });
   document.querySelector(".DOCdeleteCancel").addEventListener('click',function(E){
      POPpopoutClose(DOCconfirmDeletePop);
   });
   COL_TRC("Registered click handler.");
}

function DOCdelete(){
   let D = {}
   D.id        = PAGEhashParameter("id");
   D.created   = PAGEhashParameter("created");
   D.component = PAGEhashParameter("component");
   APIcall("doc/delete", D, function(R){
      DOCdeleteDone(R);
   });
}

function DOCdeleteDone(R){
   COL_TRC(R);
   if (!R.success){
      document.querySelector(".DOCdeleteWarning").innerText = R.error;
   } else {
      document.querySelector(".DOCdeleteWarning").innerHTML = R.data.result;
      PAGEsetHashParameter("id");
      PAGEsetHashParameter("created");
      DOCadapterRefresh();
   }
}