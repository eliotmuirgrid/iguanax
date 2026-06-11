function DOC2documentEditTagsHtml(){
   return `<textarea rows=5 class="DOC2tagsEditInput DOC2editOpen"></textarea>
      <div class="BUTTONcancel BUTTONstandard DOC2tagEditCancel">
         <img src="CARD/CARDiconCancel.svg">
      </div>
      <div class="BUTTONaction BUTTONstandard DOC2tagEditConfirm">
         <img src="CARD/CARDiconCheck.svg">
      </div>`;
}

function DOC2documentEditTagsValue(Tags){
   let H = '';
   for (let i=0; i < Tags.length; i++){
      H+= " #" + Tags[i];
   }
   return H.substring(1);
}

function DOC2documentTagEditPopulate(Doc){
   document.querySelector(".DOC2tagBarTags").innerHTML = DOC2transformTags(Doc.meta.tags);
}

function DOC2documentTagsView(Doc){
   document.querySelector(".DOC2documentTagBar").innerHTML = DOC2tagBarHtml();
   DOC2documentTagEditPopulate(Doc);
   document.querySelector(".DOC2editTags").addEventListener('click', function(E){ DOC2documentTagsEdit(Doc);});1
}

function DOC2documentTagsEdit(){
   let Doc = MODELdata.DOCdocument.data;
   document.querySelector(".DOC2documentTagBar").innerHTML = DOC2documentEditTagsHtml();
   document.querySelector(".DOC2tagsEditInput").value = DOC2documentEditTagsValue(Doc.meta.tags);
   document.querySelector(".DOC2tagEditCancel").addEventListener('click',  function(E){ DOC2documentTagsView(Doc);});
   document.querySelector(".DOC2tagEditConfirm").addEventListener('click', function(E){ DOC2documentTagsConfirmEdit(Doc);});
}


function DOC2documentTagsEditExtract(){
   let Raw = document.querySelector(".DOC2tagsEditInput").value;
   let Tags = Raw.split(" ");
   for (let i=0; i < Tags.length; i++){
      Tags[i] = Tags[i].trim();
      Tags[i] = Tags[i].substring(1);
   }
   return Tags;
}

function DOC2documentTagsEditConfirm(R){
   // DOC2modelRefresh();
   DOC2documentTagsView(MODELdata.DOCdocument.data);
   MODELupdate("DOC2scrollBarModel");
   DOC2documentModelRefresh();
   //DOC2reloadPage();
}

function DOC2documentTagsConfirmEdit(Doc){
   let C = {};
   C.component = PAGEhashParameter("component");
   C.created   = Doc.meta.created;
   C.id        = Doc.meta.id;
   C.tags      = DOC2documentTagsEditExtract();
   C.overwrite = true;
   COL_TRC(C);
   APIcall("doc/set", C, function(R) { DOC2documentTagsEditConfirm(R); });
}

function DOC2bulkEditTagsHtml(ActiveCheckbox){
   return /*html*/`
   <div class="DOC2bulkDialogClass">
      ${DOC2docTable('EDITING TAGS', ActiveCheckbox)}
      <div class='DOC2label DOC2addTagLabel'>Add tags:</div><input class='DOC2addTag DOC2input' placeholder=''/>
      <div class="DOC2bulkEditAddFeedback"></div>
      <br>
      <div class='DOC2label'>Remove tags:</div><input class='DOC2removeTag DOC2input' placeholder=''/>
      <div class="DOC2bulkEditRemoveFeedback"></div>
      <br>
      <div class="DOC2bulkDialogConfirm">Would you like to edit the tags of every component selected?</div>
   </div>
   <div class="DOC2bulkEditSubmitFeedback"></div>
   <div class='BUTTONgroup DOC2buttonGroup'>
      <div class='DOC2cancelTags BUTTONstandard BUTTONcancel'>CANCEL</div>
      <div class='DOC2saveTags BUTTONstandard BUTTONaction'>SAVE</div>
   </div>`;
}

function DOC2bulkEditTags(){
   let H = DOC2bulkEditTagsHtml(DOCcheckboxActive);
   let DOC2bulkEditTagsPop = POPpopoutOpen({title : "Edit Tags", content : H});
   document.querySelector(".DOC2saveTags").addEventListener("click", function(E){ DOC2bulkModifyTagsClicked(DOC2bulkEditTagsPop) });
   document.querySelector(".DOC2cancelTags").addEventListener("click", function(E){ POPpopoutClose(DOC2bulkEditTagsPop) });
}


function DOC2bulkModifyTagsClicked(DOC2bulkEditTagsPop){
   const AddTags = document.querySelector(".DOC2addTag.DOC2input").value.trim();
   let AddTagsTable = AddTags.split(" ");
   AddTagsTable = DOC2bulkCleanTags(AddTagsTable);
   const DelTags = document.querySelector(".DOC2removeTag.DOC2input").value.trim();
   let DelTagsTable = DelTags.split(" ");
   DelTagsTable = DOC2bulkCleanTags(DelTagsTable);

   for (let i=0; i<DOCcheckboxActive.length; i++){
      let CurrDoc = DOCcheckboxActive[i];
      let Args = DOC2documentGetParams(CurrDoc);
      APIcall("doc/get",Args,function(R){
         let CurrTags = R.data.meta.tags;
         let Merged = CurrTags.concat(AddTagsTable.filter(x => !CurrTags.includes(x)));
         Merged = Merged.filter(x => !DelTagsTable.includes(x));
         let C = {
            component : PAGEhashParameter("component"),
            created   : CurrDoc.created,
            id        : CurrDoc.id,
            tags      : Merged,
            overwrite : true
         };
         APIcall("doc/set", C, function(R){
            if (i == DOCcheckboxActive.length-1){POPpopoutClose(DOC2bulkEditTagsPop);};
         });
      });

   }
}

function DOC2bulkCleanTags(Tags){
   for (let i=0; i < Tags.length; i++){
      Tags[i] = Tags[i].trim();
      Tags[i] = Tags[i].substring(1);
   }
   return Tags
}

function DOC2documentGetParams(CurrDoc){
   let Args = {};
   Args.component = PAGEhashParameter("component");
   Args.id = CurrDoc.id;
   Args.created = CurrDoc.created;
   return Args;
}