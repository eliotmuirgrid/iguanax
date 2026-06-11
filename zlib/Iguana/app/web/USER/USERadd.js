function USERaddUserHtml() {
   return /*html*/`
<div class="USERaddUserCard">
   <div class="SETuserForm">
      <div class="SETformFullWidth">
         <div class="SETuserInfo">Username:</div>
         <input class="FORMinput USERname" type="text" value="" placeholder="Enter username"/>
      </div>
      <div class="SETformFullWidth">
         <div class="SETuserInfo">Roles:</div>
         <input class="FORMinput USERtags" type="text" value="" placeholder="Enter user roles"/>
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
         <div class="SETpasswordLabel">Confirm Password:</div>
         <div class="SETinputFieldIcon USERshowHideIcon">` +
      USERshowHideIconHtml() + /*html*/`
            <input class="FORMinput USERpasswordAgain" type="password" value="" placeholder="Enter new password again"/>
         </div>
      </div>
   </div>
   <div class="USERbuttonGroup">
      <div class="USERcancel USERbutton USERbuttonCancel">CANCEL</div>
      <div class="USERsave USERbutton USERbuttonAction">CONFIRM</div>
   </div>
</div>`;
};

function USERaddCheckAllFields(NewPassword, PasswordAgain, Username){
   // Saving in different variables to make sure all checker functions
   // gets executed - they have side effect of adding input hints
   const NameIsGood     = USERcheckNameAndShowError(Username)
   const PasswordIsGood = USERcheckPassAndShowErrors(NewPassword);
   const PasswordsMatch = USERpassMatchAndShowErrors (NewPassword, PasswordAgain)
   return NameIsGood && PasswordIsGood && PasswordsMatch;
}

function USERaddExtractData(){
   const TagString = document.querySelector(".USERtags").value.trim();
   return {
      'username'     : document.querySelector(".USERname").value.trim(),
      'password'     : document.querySelector(".USERnewPassword").value,
      'passwordAgain': document.querySelector(".USERpasswordAgain").value,
      'roles'         : TagString ? TagString.split(' ') : []
   };
}

function USERaddUser(Data, Card) {
   APIcall('user/add', Data, function(Response) {
      if(Response.success === true) {
         let Message = GINaddUser(Data.username);
         GINaddAndCommit({message: Message, users: true});
         MODELforcePoll('USERmodel');
         POPpopoutClose(Card);
         SNCKsnackbar('success', 'User saved', 6000);
      } else{ USERaddHint(Response['error'])}
   });
}

function USERaddNewUserCard() {
   let Card = POPpopoutOpen({
      className: 'SETuserPopoutCard', 
      title: 'Add a user', 
      content: USERaddUserHtml(), 
      esc_close : true
   });
   USERaddInputHints();
   USERaddRegisterEventListener(Card);
   FORMfocus(document.querySelector(".USERname"));
}

function USERaddRegisterEventListener(Card) {
   const CardElement = document.querySelector('.POPpopoutCard--anchor')
   CardElement.addEventListener('click', function(Event){
      Event.stopPropagation();
      if      (Event.target.classList.contains('USERreveal'))          { USERrevealPassword(Event.target)}
      else if (Event.target.classList.contains('USERhide'))            { USERhidePassword(Event.target)}
      else if (Event.target.classList.contains('USERcancel'))          { POPpopoutClose(Card); }
      else if (Event.target.classList.contains('USERsave')) {
         const FormData = USERaddExtractData()
         if(USERaddCheckAllFields(FormData.password, FormData.passwordAgain, FormData.username)) { USERaddUser(FormData, Card)}
      }
   });
   CardElement.addEventListener('focusin', function(Event){
      Event.stopPropagation();
      if(Event.target.classList.contains('FORMinput')) {
         if      (Event.target.classList.contains('USERname'))            { USERclearHint('Username')}
         else if (Event.target.classList.contains('USERnewPassword'))     { USERclearHint('Password')}
         else if (Event.target.classList.contains('USERcurrentPassword')) { USERclearHint('Wrong password')}
         else if (Event.target.classList.contains('USERpasswordAgain'))   { USERclearHint('Passwords')}
      }
   });
   CardElement.addEventListener('focusout', function(Event){
      Event.stopPropagation();
      if(Event.target.classList.contains('FORMinput')) {
         if     (Event.target.classList.contains('USERname'))          { USERcheckName(Event.target.value)}
         else if(Event.target.classList.contains('USERnewPassword'))   { USERcheckPassAndShowErrors(Event.target.value)}
         else if(Event.target.classList.contains('USERpasswordAgain')) { USERpassMatchAndShowErrors(Event.target.value, USERpasswordValue())}
      }
   });
}