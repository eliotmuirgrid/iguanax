function DOC2deleteWarningHtml(){
   return /*html*/ `
   <div class="DOC2deleteWarning">
      <span>Are you sure you want to delete the document: "${MODELdata.DOCdocument.data.meta.title}"</span>
      <div class="DOC2deleteButtons">
         <button class="BUTTONaction BUTTONstandard DOC2deleteConfirm">CONFIRM</button>
         <button class="BUTTONstandard BUTTONdefault DOC2deleteCancel">CANCEL</button>
      </div>
   </div>`;
}

function DOC2deleteDialog(){
   DOC2adapterListRefresh();
   let H = DOC2deleteWarningHtml();
   let DOC2confirmDeletePop = POPpopoutOpen({
      title : "Delete document", 
      content : H,
      callback : DOC2adapterRefresh
   });
   document.querySelector(".DOC2deleteConfirm").addEventListener('click', function(E){
     DOC2delete(E); 
   });
   document.querySelector(".DOC2deleteCancel").addEventListener('click',function(E){
      POPpopoutClose(DOC2confirmDeletePop);
   });
}

function DOC2delete(){
   let D = {}
   D.id        = MODELdata.DOCdocument.data.meta.id;
   D.created   = MODELdata.DOCdocument.data.meta.created;
   D.component = PAGEhashParameter("component");
   APIcall("doc/delete", D, function(R){
      DOC2deleteDone(R);
   });
}

function DOC2deleteDone(R){
   COL_TRC(R);
   if (!R.success){
      document.querySelector(".DOC2deleteWarning").innerText = R.error;
   } else {
      document.querySelector(".DOC2deleteWarning").innerHTML = R.data.result;
      PAGEsetHashParameter("id");
      PAGEsetHashParameter("created");
      DOC2adapterRefresh();
   }
}

function DOC2bulkDeleteHtml(ActiveCheckbox){
   return /*html*/`
   <div class="DOC2bulkDialogClass">
      ${DOC2docTable('DELETION', ActiveCheckbox)}
      <br>
      <div class="DOC2bulkDialogConfirm">Would you like to delete every document selected?</div>
      <br>
      <div class="DOC2bulkWarning">It is recomended to commit docs prior to performing a bulk delete.</div>
   </div>
   <div class="DOC2bulkEditSubmitFeedback"></div>
   <div class='BUTTONgroup DOC2buttonGroup'>
      <div class='DOC2cancelDel BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DOC2confirmDel BUTTONstandard BUTTONaction'>SUBMIT</div>
   </div>`;
}

function DOC2bulkDelete(){
   let H = DOC2bulkDeleteHtml(DOCcheckboxActive);
   let DOC2bulkDeletePop = POPpopoutOpen({title : "Edit Tags", content : H});
   document.querySelector(".DOC2confirmDel").addEventListener("click", function(E){ DOC2bulkDeleteClicked(DOC2bulkDeletePop) });
   document.querySelector(".DOC2cancelDel").addEventListener("click", function(E){ POPpopoutClose(DOC2bulkDeletePop) });
}

function DOC2bulkDeleteClicked(DOC2bulkDeletePop){
   for (let i=0; i<DOCcheckboxActive.length; i++){
      let CurrDoc = DOCcheckboxActive[i];
      let D = {
         id        : CurrDoc.id,
         created   : CurrDoc.created,
         component : PAGEhashParameter("component")
      }
      
      APIcall("doc/delete", D, function(R){
         if (i == DOCcheckboxActive.length-1){
            POPpopoutClose(DOC2bulkDeletePop);
            PAGEsetHashParameter("id");
            PAGEsetHashParameter("created");
            DOC2adapterRefresh();
         };
      });
      
   }
}