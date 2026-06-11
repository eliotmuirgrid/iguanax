function USERshowHideIconHtml(){
   return /*html*/`
<svg class="SETiconPassword USERreveal" width="16px" height="16px">
   <use href="SET/SETiconShowPassword.svg#SETiconShowPassword" class="USERreveal"></use>
</svg>
<svg class="SETiconPassword USERhide USERhidden" width="16px" height="16px">
   <use href="SET/SETiconHidePassword.svg#SETiconHidePassword" class="USERhide"></use>
</svg>`
}

function USERcheckName(Name) {
   let errors = [];
   if(/\s/.test(Name))   { errors.push('Username cannot have whitespace in it'); }
   if(Name.length === 0) { errors.push('Username is required'); }
   return {valid: errors.length === 0, errors: errors};
}

function USERcheckNameAndShowError(Name) {
   const Usercheck = USERcheckName(Name);
   if(Usercheck.valid) { USERclearHint('Username'); return true; }
   USERaddHint(Usercheck.errors.join('<br>'));
   return false;
}

function USERpasswordsMatch(Password, PasswordAgain) {
   let errors = [];
   if(Password !== PasswordAgain) { errors.push('Passwords do not match'); }
   return {valid: errors.length === 0, errors: errors};
}

function USERpassMatchAndShowErrors(Password, PasswordAgain) {
   const PasswordCheck = USERpasswordsMatch(Password, PasswordAgain);
   if(PasswordCheck.valid) { USERclearHint('Passwords'); return true; }
   USERaddHint(PasswordCheck.errors.join('<br>'));
   return false;
}

function USERcheckPassword(Password) {
   let errors = [];
   if(Password.trim().length === 0) { errors.push('Password cannot be empty'); }
   return {valid: errors.length === 0, errors: errors};
}

function USERcheckPassAndShowErrors(Password) {
   const PasswordCheck = USERcheckPassword(Password);
   if(PasswordCheck.valid) { USERclearHint('Passwords'); return true; }
   USERaddHint(PasswordCheck.errors.join('<br>'));
   return false;
}

function USERaddInputHints(){
   document.querySelectorAll('.FORMinput').forEach(function(Input) {
      let Hint = document.createElement('div')
      Hint.classList.add('USERfeedback', 'USERhint')
      Input.parentElement.appendChild(Hint)
   })
}

function USERaddHint(ErrorMessage, NoRedBox)   { USERaddHintToElement(USERelementFromMessage(ErrorMessage), ErrorMessage, NoRedBox)}
function USERclearHint(ErrorMessage)           { USERremoveHintFromElement(USERelementFromMessage(ErrorMessage))}
function USERpasswordValue()                   { return document.querySelector('.USERnewPassword').value}

function USERaddHintToElement(Element, ErrorMessage, NoRedBox) {
   if(!NoRedBox){Element.classList.add('USERinvalidInput')}
   let div = Element.parentNode.querySelector('.USERfeedback'); 
   if(div) div.innerHTML = ErrorMessage;
}

function USERremoveHintFromElement(Element) {
   Element.classList.remove('USERinvalidInput')
   Element.parentNode.querySelector('.USERfeedback').innerHTML = "";
}

function USERelementFromMessage(ErrorMessage){
   if     (ErrorMessage.toLowerCase().includes('username'))       { return document.querySelector('.USERname')}
   else if(ErrorMessage.toLowerCase().includes('wrong password')) { return document.querySelector('.USERcurrentPassword')}
   else if(ErrorMessage.toLowerCase().includes('passwords'))      { return document.querySelector('.USERpasswordAgain')}
   else if(ErrorMessage.toLowerCase().includes('password'))       { return document.querySelector('.USERnewPassword')}
   else if(ErrorMessage.toLowerCase().includes('description'))    { return document.querySelector('.USERtags')}
   SNCKsnackbar('error', ErrorMessage, 6000);
}

function USERrevealPassword(Element){
   const ShowHideElement = Element.closest('.USERshowHideIcon')
   ShowHideElement.querySelector('svg.USERreveal').classList.add('USERhidden')
   ShowHideElement.querySelector('svg.USERhide').classList.remove('USERhidden')
   ShowHideElement.querySelector('.FORMinput').type = 'text'
}

function USERhidePassword(Element){
   const ShowHideElement = Element.closest('.USERshowHideIcon')
   ShowHideElement.querySelector('svg.USERhide').classList.add('USERhidden')
   ShowHideElement.querySelector('svg.USERreveal').classList.remove('USERhidden')
   ShowHideElement.querySelector('.FORMinput').type = 'password'
}