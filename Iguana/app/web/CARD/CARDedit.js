// function CARDeditUpdate(){ if(CARDcomponentOn()) CARDdisableOpenEdit(); }

function CARDdisableOpenEdit(){
   if(document.querySelector('.CARDedit.CARDnameActions'))    CARDdisableNameEdit();
   if(document.querySelector('.CARDedit.CARDlabelActions'))   CARDdisableDescriptionEdit();
   if(document.querySelector('.CARDedit.CARDcurrentActions')) CARDdisableCurrentCommitEdit(); 
   if(document.querySelector('.CARDedit.CARDviewFields'))     CARDdisableEditedFieldRow();
   if(document.querySelector('.CARDtagRowInnerEdit'))         CARDcancelTags();
}

function CARDsaveOpenEdits(){
   if(CARDnameEdit) CARDsaveName();
   if(CARDdescriptionEdit) CARDsaveDescription();
}