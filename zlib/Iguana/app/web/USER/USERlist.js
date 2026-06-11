let USERlistCache = [];

function USEReditButton() {
   let Edit = USEReditPermission();
   return /*html*/`
   <div class="SETuserActions BUTTONgroup">
      <div class="${Edit ? "BUTTONaction SETeditUsersButton" : "BUTTONdisabled"} BUTTONstandard">ADD USER</div>
   </div>`;
}

function USERchangeTagsHtml(){
   return /*html*/`
   <div class="BUTTONstandard BUTTONcancel USERupdateTag">EDIT</div>`;
}

function USERactionButtons(IsUser, IsLocal){
   const ChangePwdHtml = /*html*/`<div class="FORMbutton BUTTONcancel SETupdateRow">
      <svg class="SETpasswordIcon" width="16" height="16">
         <use href="SET/SETpasswordIcon.svg#SETpasswordIcon"></use>
      </svg>
   </div>`;
   let Edit = USEReditPermission();
   if(!Edit && IsUser && IsLocal) { return ChangePwdHtml; }
   if(!Edit) return "";
   return /*html*/`\
   ${USERchangeTagsHtml()}
   ${IsLocal ?  ChangePwdHtml : ''}
   <div class="FORMbutton BUTTONcancel BUTTONwarning SETdeleteRow">${SETtrashIcon()}</div>`;
}

function USERexternalAuthHtml(){
   return /*html*/`<div title="This user is externally authenticated." class="USERexternal">Externally authenticated</div>`;
}

function USERrowHtml(UserObject) {
   const CurrentUser = USERcurrentUser();
   const IsUser      = CurrentUser && UserObject.username === CurrentUser.username;
   let IsUserClass   = IsUser ? " USERuserSelected" : "";
   const IsExternal  = UserObject?.external;
   const displayName = UserObject.display_name || UserObject.username;
   const provider    = UserObject.provider || 'Local';
   const Buttons     = IsExternal ? USERexternalAuthHtml() : USERactionButtons(IsUser, provider === 'Local');
   return /*html*/`
   <div class="SETuserRow">
      <div class="SETuserData ${IsUserClass}">
         ${ESChtmlEscape(displayName)}
      </div>
      <div class="SETuserData">${HASHTAGrenderTags(UserObject.roles, "USERtagClickable", USERroleData())}</div>
      <div class="SETuserData">${ESChtmlEscape(provider)}</div>
      <div class="SETuserData">${Buttons}</div>
   </div>`;
};
