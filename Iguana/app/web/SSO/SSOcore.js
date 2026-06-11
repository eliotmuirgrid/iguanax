function SSOinit() {
   APIcall('auth/get', {}, function(r) {
      if(r?.data?.license === false) { return SSOhideBecauseLicense(); }
      if(r?.data?.mode === "Client" && r?.data?.provider === "Microsoft") { return SSOinitMicrosoftSuccess(r); }
      if(r?.data?.mode === "SAML") { return SSOinitSamlSuccess(r); }
      SSOupdateDividerVisibility();
   });
}

function SSOhideBecauseLicense() {
   SSOupdateMicrosoft(true);
   SSOupdateSaml(true);
}


function SSOupdateDividerVisibility() {
   const divider            = document.querySelector('.SESSloginDivider');
   const ssoContainer       = document.querySelector('.SESSssoContainer');
   const microsoftContainer = document.querySelector('.SESSmicrosoftSignInContainer');
   const samlContainer      = document.querySelector('.SESSSamlSignInContainer');
   if(!divider) { return; }
   const microsoftHidden      = !microsoftContainer || window.getComputedStyle(microsoftContainer).display === 'none';
   const samlHidden           = !samlContainer || window.getComputedStyle(samlContainer).display === 'none';
   const allHidden            = microsoftHidden && samlHidden;
   divider.style.display      = allHidden ? 'none' : '';
   ssoContainer.style.display = allHidden ? 'none' : '';
}

async function SSOlogout() {
   await SSOmicrosoftLogout();
   await SSOsamlLogout();
}

function SSOifSsoUser(callbackYes, callbackNo) {
   APIcall('user/whoami', {}, function(r) {
      if(r?.data?.external && callbackYes) { return callbackYes(); }
      if(!r?.data?.external && callbackNo) { return callbackNo(); }
   });
}
