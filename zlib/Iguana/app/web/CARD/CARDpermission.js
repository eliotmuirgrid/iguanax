function CARDwritable(){
   return !CARDdata()?.template;
}

function CARDupdatePermission() {
   const EditInProgress = document.querySelector('.CARDedit') !== null;
   const Writable       = CARDwritable();
   let Message = "Read Only Privileges ";
   if(Writable) Message = ""
   if(Writable && EditInProgress) Message += "Must close open fields before editing.";
   let PermissionDiv = document.querySelector('.CARDpermission');
   if(!PermissionDiv) return;
   if(PermissionDiv.innerHTML !== Message) PermissionDiv.innerHTML = Message;
}

function CARDupdatePermissableAction(){
   const EditInProgress = document.querySelector('.CARDedit') !== null;
   const Writable = CARDwritable();
   let Message = "";
   if( Writable && !EditInProgress) Message = `Edit in Translator…`;
   let ActionDiv = document.querySelector('.CARDaction');
   if(!ActionDiv) return;
   if(ActionDiv.innerHTML !== Message) ActionDiv.innerHTML = Message;
}

function CARDupdatePrePostAction(){
   let ActionDiv = document.querySelector('.CARDaction');
   if(!ActionDiv) return;
   let Pre = "[";
   if(ActionDiv.innerHTML === "") Pre = "";
   let PreDiv = document.querySelector('.CARDpreAction');
   if(!PreDiv) return;
   if(PreDiv.innerHTML !== Pre) PreDiv.innerHTML = Pre;
   let Post = "]";
   if(ActionDiv.innerHTML === "") Post = "";
   let PostDiv = document.querySelector('.CARDpostAction');
   if(!PostDiv) return;
   if(PostDiv.innerHTML !== Post) PostDiv.innerHTML = Post;
}

function CARDupdateMakeCopy(){
   let ActionDiv = document.querySelector('.CARDaction');
   if(!ActionDiv) return;
   let MakeCopyDiv = document.querySelector('.CARDcopyAction');
   if(!MakeCopyDiv) return;
   const EditInProgress = document.querySelector('.CARDedit') !== null;
   if( EditInProgress) MakeCopyDiv.style.display = "none";
   else                MakeCopyDiv.style.display = "";

}

function CARDpermissionHtml(){
   let EditPerm = CARDpermissionData()?.edit_script;
   let href = EditPerm ? `href="${TRANSformatUrl(CARDcomponentId(), true)}"` : "";
   return /*html*/`
   <div class="FORMlabel">Code:</div>
   <div class="CARDpermission"></div>
   <div class="CARDpermissableAction">
      <div class="CARDpreAction"></div>
      <a class="CARDaction" ${href}></a>
      <div class="CARDpostAction"></div>
      <div class="CARDcopyAction"></div>
   </div>`;
}

function CARDupdateIsTemplate(){
   let IsTemplate = CARDdata().template;
   let PermissionDiv = document.querySelector('.CARDpermissionDiv');
   if(!PermissionDiv) return;
   if( PermissionDiv.classList.contains("CARDhidden") && !IsTemplate) PermissionDiv.classList.remove("CARDhidden");
   if(!PermissionDiv.classList.contains("CARDhidden") &&  IsTemplate) PermissionDiv.classList.add   ("CARDhidden");
}

function CARDupdatePermissionViews(){
   CARDupdatePermission();
   CARDupdatePermissableAction();
   CARDupdatePrePostAction();
   CARDupdateMakeCopy();
   CARDupdateIsTemplate();
}
