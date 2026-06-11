let USERusername;
let USERtagModal;

function USERupdateTagsCard(Name){
   USERusername = Name;
   let Data = USERdata()[Name];
   if(!Data) return;
   USERtagModal = POPpopoutOpen({
      title: 'Edit Roles', 
      content: USERupdateTagsHtml(Data),
      width: "660px"
   });
   let div = document.querySelector(".USERtagModal");
   if(!div) return;
   div.addEventListener("click", function(e){ 
      USERupdateTagClick(e.target);
   });
   let input = document.querySelector(".USERtags");
   FORMfocus(input);
   FORMenterListener(input, USERsaveTag);
}

function USERupdateTagClick(target){
   if(target.closest(".USERsaveTag"))   return USERsaveTag();
   if(target.closest(".USERcancelTag")) return USERcancelTag();
}

function USERsetTagError(error){
   let div = document.querySelector(".USERsetTagError");
   if(!div) return;
   div.innerHTML = error;
}

function USERcancelTag(){
   MODELforcePoll("USERmodel");
   POPpopoutClose(USERtagModal);  
}

function USERsaveTag(){
   let div = document.querySelector(".USERtags");
   if(!div) return;
   let roles = div?.value?.split(" ");
   let Data = {};
   Data.username = USERusername;
   Data.roles = roles;
   APIcall("user/set/roles", Data, function(R){
      if(!R.success) return USERsetTagError(R.error);
      MODELforcePoll("USERmodel");
      POPpopoutClose(USERtagModal);
      let Message = GINsaveUserRoles(Data.username);
      GINaddAndCommit({message: Message, users: true});
   });
}

function USERupdateTagsHtml(Data){
   const displayName = Data.display_name || Data.username;
   return /*html*/`
   <div class="USERtagModal">
      <div class="USERtagRows">
         <div class="SETuserInfo">Username:</div>
         <div>${ESChtmlEscape(displayName)}</div>
         <div class="SETuserInfo">Roles:</div>
         <input class="FORMinput USERtags" type="text" value="${ESChtmlEscape(Data.roles.join(" "))}" placeholder="Enter roles"/>
         <div></div>
         <div class="USERsetTagError"></div>
      </div>
      <br>
      <div class="USERbuttonGroup">
         <div class="USERcancelTag USERbutton USERbuttonCancel">CANCEL</div>
         <div class="USERsaveTag USERbutton USERbuttonAction">SAVE</div>
      </div>
   </div>`;
}