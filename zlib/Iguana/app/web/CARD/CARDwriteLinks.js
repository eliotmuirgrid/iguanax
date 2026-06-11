function CARDupdateWriteLinks(){
   CARDupdateWriteLink(document.querySelector(".CARDeditName"), "edit_name", "CARDvisHidden");
   CARDupdateTitlePerm(document.querySelector(".CARDname"),     "edit_name");
   CARDupdateWriteLink(document.querySelector(".CARDdescriptionEditButton"), "edit_description");
   CARDupdateTitlePerm(document.querySelector(".CARDdescription")          , "edit_description");
   
   CARDupdateClearLink(document.querySelector(".CARDeditUpstream"),   "edit_upstream");
   CARDupdateClearLink(document.querySelector(".CARDaction"),         "edit_script");
   CARDupdateClearLink(document.querySelector(".CARDremoveUpstream"), "edit_upstream");

   CARDupdateWriteLink(document.querySelector(".CARDgitEditCurrentCommitButton"), "edit_commit");
   CARDupdateTitlePerm(document.querySelector(".CARDcurrentCommitIdDiv"),         "edit_commit");

   CARDhandleAllFieldRows();
}

function CARDhandleAllFieldRows(){
   let EditFields = document.querySelectorAll(".CARDeditValueInPlace");
   let RowFields  = document.querySelectorAll(".CARDviewFields");
   for(let i=0; i < EditFields.length; i++){
      CARDupdateWriteLink(EditFields[i], "edit_fields");
      CARDupdateTitlePerm(RowFields[i],  "edit_fields");
   }
}

function CARDupdateWriteLink(Div, Key, ClassName="CARDhidden"){
   if(!Div) return;
   let Perm = CARDpermissionData()[Key];
   if(!Perm) Div.classList.add   (ClassName);
   else      Div.classList.remove(ClassName);
}

function CARDupdateTitlePerm(Div, Key){
   if(!Div) return;
   let Perm = CARDpermissionData()[Key];
   let Title = Perm ? "" : "You do not have permission to edit this.";
   if(Div.title != Title) Div.title = Title;
}