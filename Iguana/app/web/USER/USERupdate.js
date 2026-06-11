function USERscreen(){
   SETrender();
   document.querySelector('.USERusers').classList.add('selected');
   USERmodelInit();
}

function USERupdateUserHtml(UserObject) {
   return /*html*/`
   <div class="USERupdateUserCard">
      <div class="USERreadDiv">
         <div class="SETuserInfo">Username:</div>
         <div class="USERname USERreadOnly SETpageDescription">${ESChtmlEscape(UserObject.username)}</div>
      </div>   
      <br>
      <div class="SETuserForm">
         <div class="SETformFullWidth USERpassword USERrequirePassword">
            <div class="SETpasswordLabel">Current Password:</div>
            <div class="SETinputFieldIcon USERshowHideIcon">` +
      USERshowHideIconHtml() + /*html*/`
               <input class="FORMinput USERcurrentPassword" type="password" value="" placeholder="Enter current password"/>
            </div>
         </div>
         <div class="SETformFullWidth USERpassword">
            <div class="SETpasswordLabel">New Password:</div>
            <div class="SETinputFieldIcon USERshowHideIcon">` +
      USERshowHideIconHtml() + /*html*/`
               <input class="FORMinput USERnewPassword" type="password" value="" placeholder="Enter new password"/>
            </div>
			   <div class="SETinputFieldInfo">Must be 8+ characters, containing uppercase, lowercase and numerical digit(s).</div>
         </div>
         <div class="SETformFullWidth USERpassword">
            <div class="SETpasswordLabel">Confirm New Password:</div>
            <div class="SETinputFieldIcon USERshowHideIcon">` +
      USERshowHideIconHtml() + /*html*/`
               <input class="FORMinput USERpasswordAgain" type="password" value="" placeholder="Enter new password again"/>
            </div>
         </div>
      </div>
      <br>
      <div class="USERbuttonGroup">
         <div class="USERcancel USERbutton USERbuttonCancel">CANCEL</div>
         <div class="USERsave USERbutton USERbuttonAction">SAVE</div>
      </div>
   </div>`;
}



function USERupdateCheckAllFields(NewPassword, PasswordAgain) {
   // Saving in different variables to make sure all checker functions
   // gets executed - they have side effect of adding input hints
   const PasswordCheck  = USERcheckPassAndShowErrors(NewPassword);
   const PasswordsMatch = USERpassMatchAndShowErrors(NewPassword, PasswordAgain);
   if(!document.querySelector('.USERpassword').classList.contains('USERhidden')) { return PasswordCheck && PasswordsMatch; }
   return true;
}

function USERupdateExtractData(Username, IsAdmin) {
   let Result = {
      username         : Username,
      password         : document.querySelector(".USERnewPassword").value,
      passwordAgain    : document.querySelector(".USERpasswordAgain").value,
      current_password : document.querySelector(".USERcurrentPassword").value
   };
   return Result
}

function USERupdateUser(Data, Card) {
   APIcall('user/set/password', Data, function(Response) {
      MODELforcePoll('USERmodel');
      if(!Response.success) return USERaddHint(Response.error);
      POPpopoutClose(Card);
      SNCKsnackbar('success', 'User saved', 6000);
      let Message = GINsaveUserPassword(Data.username);
      GINaddAndCommit({message: Message, users: true});
   });
}

function USERupdateUserCard(UserObject, Name) {
   USERusername = Name;
   let Card = POPpopoutOpen({
      className: 'SETeditUserPopoutCard', 
      title: 'Edit Password', 
      content: USERupdateUserHtml(UserObject), 
      esc_close : true
   });
   USERaddInputHints()
   USERupdateRegisterEventListener(UserObject.username, SESScache.is_admin,Card);
}

function USERupdateRegisterEventListener(Username, IsAdmin,Card) {
   const CardElement = document.querySelector('.POPpopoutCard--anchor')
   CardElement.addEventListener('click', function(Event){
      const Classlist = Event.target.classList;
      Event.stopPropagation();
      if (Classlist.contains('USERnewPassword'))     { USERclearHint('Password')}
      else if (Classlist.contains('USERcurrentPassword')) { USERclearHint('Wrong password')}
      else if (Classlist.contains('USERpasswordAgain'))   { USERclearHint('Passwords')}
      else if (Classlist.contains('USERreveal'))          { USERrevealPassword(Event.target)}
      else if (Classlist.contains('USERhide'))            { USERhidePassword(Event.target)}
      else if (Classlist.contains('USERcancel'))          { POPpopoutClose(Card)}
      else if (Classlist.contains('USERsave')) {
         const FormData = USERupdateExtractData(Username, IsAdmin)
         if(USERupdateCheckAllFields(FormData.password, FormData.passwordAgain)) { USERupdateUser(FormData, Card)}
      }
   });
   CardElement.addEventListener('focusin', function(Event){
      const Classlist = Event.target.classList;
      Event.stopPropagation();
      if(Event.target.classList.contains('FORMinput')) {
         if      (Classlist.contains('USERnewPassword'))     { USERclearHint('Password')}
         else if (Classlist.contains('USERcurrentPassword')) { USERclearHint('Wrong password')}
         else if (Classlist.contains('USERpasswordAgain'))   { USERclearHint('Passwords')}
      }
   });
   CardElement.addEventListener('focusout', function(Event){
      const Classlist = Event.target.classList;
      Event.stopPropagation();
      if(Event.target.classList.contains('FORMinput')) {
         if      (Classlist.contains('USERnewPassword'))   { USERcheckPassAndShowErrors(Event.target.value);}
         else if (Classlist.contains('USERpasswordAgain')) { USERpassMatchAndShowErrors(Event.target.value, USERpasswordValue())}
      }
   });
}