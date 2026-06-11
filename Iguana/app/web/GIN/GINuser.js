let GINeditUserCard;

function GINupdateUser(){
   let Html = GINuserHtml();
   let div = document.querySelector(".GINuser");
   if(div.innerHTML != Html) div.innerHTML = Html;
}

function GINuserValueHtml(){
   let Repo = ESChtmlEscape(GINuser());
   if(!Repo) Repo = "<div class='GINvalueEmpty'>Not set</div>";
   return Repo;
}

function GINuserHtml(){
   return /*html*/`
   <div class="FORMlabel GINmarginTop">Push/Pull as:</div>
   <div class="GINrepoValue GINmarginTop">${GINuserValueHtml()}</div>
   <div class="GINeditUser BUTTONgroup"></div>`;
}

function GINeditUser(){
   APIcall("user/list", {}, function(R){
      if(!R.success) return SNCKsnackbar('error', 'Cannot get user list: ' + R.error);
      let UserList = R?.data || {};
      GINeditUserCard  = POPpopoutOpen({
         title: "Edit Push/Pull as",
         width: "600px",
         content: GINeditUserHtml(UserList)
      });
      GINeditUserListeners();
      let input = document.querySelector(".GINeditUserValue");
      FORMfocus(input);
      FORMenterListener(input, GINeditUserConfirm);
   });
}

function GINeditUserListeners(){
   let div = document.querySelector(".GINeditUserModal");
   div.addEventListener("click", function(e){
      if(e.target.closest(".GINeditUserConfirm")) GINeditUserConfirm();
      if(e.target.closest(".GINeditUserCancel"))  GINeditUserClose();
      if(e.target.closest(".GINeditUserOption"))  GINeditUserOption(e.target.closest(".GINeditUserOption").innerHTML);
   });
}

function GINeditUserConfirm(){
   let div = document.querySelector(".GINeditUserValue");
   if(!div) return;
   let User = div?.value?.trim() || "";
   APIcall("instance/user/set", {user: User}, function(R){
      if(R.success) {
         MODELforcePoll("GINmodel");
         GINeditUserClose();
         let Message = GINupdateInstanceUser(User);
         GINaddAndCommit({message: Message, settings: true});
      }
      else GINeditUserError(R.error);
   });
}

function GINeditUserClose(){
   POPpopoutClose(GINeditUserCard);
}

function GINeditUserHtml(UserList){
   return /*html*/`
   <div class="GINeditUserModal">
      <div class="GINeditUserTitle"><b>Click</b> one of the users to populate the field below:</div>
      <br>
      <div class=GINeditUserBox>
         ${GINvalidUsersHtml(UserList)}
      </div>
      <div class="GINeditUserNote">If the user is not set then it will default to using the server's git user.</div>
      <br>
      <div class="FORMlabel">Push/Pull as:</div>
      <div class="GINeditUserVal">
         <input type="text" class="GINeditUserValue FORMinput" value="${ESChtmlEscape(GINuser())}">
      </div>
      <div class="GINeditUserError"></div>
      <br>
      <br>
      <div class="GINeditUserButtons BUTTONgroup">
         <div class="GINeditUserCancel BUTTONstandard BUTTONdefault">CANCEL</div>
         <div class="GINeditUserConfirm BUTTONstandard BUTTONaction">CONFIRM</div>
      </div>
   </div>
   `;
}

function GINeditUserError(Error  ){
   let div = document.querySelector(".GINeditUserError");
   if(!div) return;
   div.innerHTML = Error;
}

function GINeditUserOption(User){
   let div = document.querySelector(".GINeditUserValue");
   if(!div) return;
   div.value = User;
   let input = document.querySelector(".GINeditUserValue");
   FORMfocus(input);
}

function GINvalidUsersHtml(UserList){
   let html = "";
   for(let User in UserList){
      html += /*html*/`<div class="GINeditUserOption">${User}</div>`;
   }
   return html;
}