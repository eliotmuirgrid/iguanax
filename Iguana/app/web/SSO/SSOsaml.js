let SAML_CONFIG = null;

function SSOinitSamlSuccess(response) {
   if(response?.success && response?.data) {
      SAML_CONFIG = response.data;
      return SSOloadSamlLogin();
   }
   if(response?.error) { SESSshowSamlError(response.error); }
   SSOupdateSaml(true);
}

function SSOloadSamlLogin() {
   SSOupdateSaml(false);
   SSOcreateSamlButton();
}

function SSOcreateSamlButton() {
   const buttonContainer = document.querySelector('.SESSsamlButton');
   if(!buttonContainer) { return; }

   const providerName  = SAML_CONFIG?.provider || "SAML";
   const providerLower = providerName.toLowerCase();
   let svg             = '';
   let buttonText      = '';
   
   if(providerLower === 'google') {
      svg = SSOgoogleButtonSvg();
      buttonText = 'Sign in with Google';
   } else if(providerLower === 'microsoft') {
      svg = SSOmicrosoftButtonSvg();
      buttonText = 'Sign in with Microsoft';
   } else if(providerLower === 'okta') {
      svg = SSOoktaButtonSvg();
      buttonText = 'Sign in with Okta';
   } else {
      svg = SSOgenericButtonSvg();
      buttonText = `Sign in with ${providerName}`;
   }
   
   buttonContainer.innerHTML = `
      <button class="saml-signin-btn" type="button">
         ${svg}
         <span>${buttonText}</span>
      </button>
      <div class="saml-spinner" style="display: none;">
         <div class="CORspinner"></div>
      </div>
   `;

   const button = buttonContainer.querySelector(".saml-signin-btn");
   if(button) { button.addEventListener("click", SSOhandleSamlSignIn); }

   const hasActive = (SESScache?.sso_provider === "SAML");
   SSOupdateSaml(hasActive);
}

function SSOhandleSamlSignIn(event) {
   SESShideSamlError();
   if(event) {
      event.preventDefault();
      event.stopPropagation();
   }

   if(!SAML_CONFIG) {
      return SESSshowSamlError('SAML authentication not properly configured');
   }

   SSOshowSamlSpinner();
   SSOinitiateSamlLogin();
}

function SSOinitiateSamlLogin() {
   APIcall('auth/saml/initiate', {}, function(response) {
      if(response?.success && response?.data?.redirect_url) {
         window.location.href = response.data.redirect_url;
      } else {
         SSOhideSamlSpinner();
         SESSshowSamlError(response?.error || 'Failed to initiate SAML login');
      }
   });
}

async function SSOsamlLogout() {
   if(!SAML_CONFIG) { return false; }
   SAML_CONFIG = null;
   SESScache   = null;
   sessionStorage.clear();
   return true;
}

function SSOupdateSaml(hide) {
   const samlContainer = document.querySelector(".SESSSamlSignInContainer");
   if(samlContainer) { samlContainer.style.display = hide ? "none" : "flex"; }
   SSOupdateDividerVisibility?.();
}

function SSOshowSamlSpinner() {
   SSOcreateSamlButton();
   const spinnerContainer = document.querySelector(".SESSsamlButton .saml-spinner");
   const buttonContainer  = document.querySelector(".SESSsamlButton .saml-signin-btn");
   if(spinnerContainer) { spinnerContainer.style.display = "block"; }
   if(buttonContainer) { buttonContainer.style.display = "none"; }
}

function SSOhideSamlSpinner() {
   const spinnerContainer = document.querySelector(".SESSsamlButton .saml-spinner");
   const buttonContainer  = document.querySelector(".SESSsamlButton .saml-signin-btn");
   if(spinnerContainer) { spinnerContainer.style.display = "none"; }
   if(buttonContainer) { buttonContainer.style.display = "block"; }
}

function SSOgenericButtonSvg() {
   return `
         <svg width="21" height="21" viewBox="0 0 21 21" fill="none" xmlns="http://www.w3.org/2000/svg">
            <rect x="2" y="2" width="17" height="17" rx="2" stroke="#666" stroke-width="2" fill="none"/>
            <path d="M6 10.5 L9 13.5 L15 7.5" stroke="#666" stroke-width="2" stroke-linecap="round" stroke-linejoin="round" fill="none"/>
         </svg>
      `;
}