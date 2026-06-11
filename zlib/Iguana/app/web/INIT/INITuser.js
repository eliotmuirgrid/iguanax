function INITuser() {
   COL_TRC("Set up user screen.");
   MODELshutdownAll();
   INITframe(INITuserHtml());
   INITuserButtons();
   INITuserModel(INITuserFormInit);
   INITbackNextBtns('config', 'server');
   INITcheckUserNeeded();
}

function INITuserButtons(){
   let CreateButton    = document.querySelector(".INITsignupFormContainer");
   let ConfirmPassword = document.querySelector(".INITsignupFormContainer");
   if(!CreateButton || !ConfirmPassword) return;
   CreateButton.addEventListener   ('click', function(E) {INITuserOnClicked(E);});
   ConfirmPassword.addEventListener('keydown', function(E) {INITkeydownPasswordConfirm(E);});
}

function INITkeydownPasswordConfirm(e){
   if(e.key === "Enter" ) INITsubmitUser();
}

function INITuserOnClicked(E){
  COL_TRC("Clicked", E);
  E.stopPropagation();
  if (E.target.classList.contains('INITuserSubmit'))   {COL_TRC("Add User"); INITsubmitUser(); }
}

function INITuserModel(Callback){
   MODELpollerAdd('INITuserModel', 'INITuser',      6.02, 'install/ensure/admin', {});
   MODELviewAdd  ('INITuserModel', 'INITuserNext');
   MODELforcePoll('INITuserModel', function(){
      if(Callback) Callback();
   });
   MODELstart('INITuserModel');
}

function INITcheckUserNeeded() {
   APIcall('install/ensure/admin', {}, (R) => {
      console.warn(R);
      if(R.success) { return INITenableNext(); }
      INITshowError(R.error);
   });
}

function INITuserConfirmPassword() {
   const Passwords     = document.querySelectorAll('.FORMinput[name="password"]');
   const PasswordCheck = USERcheckPassword(Passwords[0].value);
   const PasswordMatch = USERpasswordsMatch(Passwords[0].value, Passwords[1].value);
   if(PasswordCheck.valid && PasswordMatch.valid) { return true; }
   const ErrorString = `${PasswordCheck.errors.join('\n')}\n${PasswordMatch.errors.join('\n')}`;
   INITshowError(ErrorString);
   return false;
}

function INITuserValidUsername() {
   const Username = document.querySelector('.FORMinput[name="username"]');
   const UserCheck = USERcheckName(Username.value);
   COL_TRC("UserCheck =", UserCheck)
   if(UserCheck.valid) { return true; }
   INITshowError(UserCheck.errors.join('\n'));
   return false;
}

function INITsubmitUser() {
   INIThideStatusDivs();
   if(!INITuserValidUsername())   { return; }
   if(!INITuserConfirmPassword()) { return; }
   const username    = document.querySelector('.FORMinput[name="username"]').value;
   const tags        = [];
   const password    = document.querySelector('.FORMinput[name="password"]').value;
   let Data = {username:username, tags : tags, password:password};
   COL_TRC("Data =", Data);
   APIcall('install/admin/create', Data, INIThandleUserResponse);
}

function INITpasswordStrength() {
   if (!INITuserNeedAccount()){
      COL_TRC("No form for user, so we don't need this code to run.");
      return;
   }
   const PasswordStrengthBar  = document.querySelector('.INITpasswordStrength');
   PasswordStrengthBar.style.width = `0`;
   document.querySelector('.FORMinput[name="password"]').addEventListener('input', INITupdatePasswordStrength); 
}

function INIThandleUserResponse(response) {
   if(!response.success) { return INITshowError(response.error); }
   INITclearUserFields();
   INITuserModel(INITuserFormInit);
   INITshowSuccess('User created successfully');
}

function INITuserNext(){
   if(INITuserNeedAccount()) INITdisableNext();
   else                      INITenableNext();
}

function INITclearUserFields() {
   document.querySelector('.FORMinput[name="username"]').value = '';
   document.querySelectorAll('.FORMinput[name="password"]').forEach(field => field.value = '');
}

function INITupdatePasswordStrength() {
   const PasswordInput        = document.querySelector('.FORMinput[name="password"]');
   const PasswordStrengthBar  = document.querySelector('.INITpasswordStrength');
   const PasswordStrengthText = document.querySelector('.INITstrengthText');

   const Password = PasswordInput.value;
   const Strength = INITcalculatePasswordStrength(Password);
   // Update the password strength bar width based on the strength percentage
   PasswordStrengthBar.style.width = `${Strength}%`;
   if(Strength === 100) {
      PasswordStrengthBar.classList.add('full');
      PasswordStrengthText.textContent = 'Strength: Strong';
   } else {
      PasswordStrengthBar.classList.remove('full');
      PasswordStrengthText.textContent = Strength === 0 ? '' : 'Strength: Weak';
   }
}

function INITcalculatePasswordStrength(password) {
   let strength = 0;
   if (/[A-Z]/.test(password))       { strength += 25; } // Check for uppercase letter
   if (/[a-z]/.test(password))       { strength += 25; } // Check for lowercase letter
   if (/[0-9]/.test(password))       { strength += 25; } // Check for number
   if (/[!@#$%^&*.]/.test(password)) { strength += 25; } // Check for special character
   return strength;
}

function INITuserNeedAccount(){
   return !MODELdata.INITuser.data.admin_exists;
}

function INITuserFormInit(){
   let E = document.querySelector(".INITsignupFormContainer");
   E.innerHTML = INITuserFormStart();
   let F = document.querySelector(".INITproceedDiv");
   let Html =  INITuserNeedAccount() ? "" : INITpleaseProceed();
   if(F.innerHTML !== Html ) F.innerHTML = Html;
   INITpasswordStrength();
}

function INITuserFormStart(){
   if (INITuserNeedAccount()){
      return INITuserFormHtml();   
   } else {
      return /*html*/`
      <div class="INITuserExistsRow">
         <div>Admin user is setup</div>
         ${INITcheckmarkHtml()} 
      </div>`;
   }
}

function INITuserFormHtml(){
   return /*html*/`
   <form class="INITsignupForm">
      <div class="INITformGroup">
         <div class="INITuserLabelRow">
            <label class="FORMlabel" for="INITusername">Username:</label>
            <div class="INITdefaultUserValue">* default="admin"</div>
         </div>
         <input class="FORMinput" type="text" class="INITusername" name="username" required value="admin">
      </div>
      <div class="INITformGroup">
         <div class="INITuserLabelRow">
            <label class="FORMlabel" for="INITpassword">Password:</label>
            <div class="INITdefaultUserValue">* default="password"</div>
         </div>
         <input class="FORMinput" type="password" class="INITpassword" name="password" required value="password">
         <div class="INITpasswordStrength"></div>
         <span class="INITstrengthText"></span>
      </div>
      <div class="INITformGroup">
         <label class="FORMlabel" for="INITpassword">Confirm Password:</label>
         <input class="FORMinput" type="password" class="INITpassword" name="password" required value="password">
      </div>
   </form>
   <div class="INITuserError"></div>
   <div class="BUTTONgroup INITcreateRow">
      <div class="INITuserSubmit BUTTONaction">CREATE</div>
   </div>`;
}



function INITuserHtml() {
   return /*html*/`
	${INITsidebarHtml()}
	<div class="INITmodalMainArea">
		<h2 class="INITmodalHeading">
			Create Admin User
		</h2>
      <div class="INITsignupFormContainer"></div>
      <div class="INITproceedDiv"></div>
		${INITcreateBtnGroupAndStatusDivs()}
	</div>`;
}