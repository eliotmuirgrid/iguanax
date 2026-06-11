function USERlisteners(){
   let div = document.querySelector(".USERmodal");
   if(!div) return;
   div.addEventListener("click", USERclick);
}

function USERclick(e){
   if(e.target.closest(".SETeditUsersButton")) return USERaddNewUserCard();
   if(e.target.closest(".USERupdateTag"))      return USERupdateTags(e.target);
   if(e.target.closest(".SETupdateRow"))       return USERupdateRow(e.target);
   if(e.target.closest(".SETdeleteRow"))       return USERdeleteRow(e.target);
   if(e.target.closest(".USERtagClickable"))   return USERtagClicked(e.target);
}

function USERnameFromTarget(target) {
   const row        = target.closest('.SETuserRow');
   const rows       = document.querySelectorAll('.SETuserRow');
   const index      = Array.from(rows).indexOf(row);
   const userObject = USERlistCache[index];
   return userObject.username;
}

function USERuserObjectFromTarget(target) {
   const row   = target.closest('.SETuserRow');
   const rows  = document.querySelectorAll('.SETuserRow');
   const index = Array.from(rows).indexOf(row);
   return USERlistCache[index];
}

function USERupdateTags(target){
   const Name = USERnameFromTarget(target);
   USERupdateTagsCard(Name);
}

function USERupdateRow(target){
   const UserObject = USERuserObjectFromTarget(target);
   const Name = USERnameFromTarget(target);
   USERupdateUserCard(UserObject, Name);
}

function USERdeleteRow(target) {
   const UserObject = USERuserObjectFromTarget(target);
   USERdeleteCard(UserObject);
}

function USERtagClicked(target){
   let div = target.closest(".USERtagClickable");
   if(!div) return;
   let hashtag = target.getAttribute('word')?.trim();
   if(!hashtag) return;
   let Roles = USERroleData();
   if(!Roles) return;
   let Tag = Roles[hashtag];
   if(!Tag) return;
   let Url = "#settings/roles";
   ROLopenedTo = hashtag;
   window.location.hash = Url;
}
