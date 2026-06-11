const MS_GRAPH_SCOPES = ["https://graph.microsoft.com/Directory.Read.All"];
const MSAL_SRC        = "https://alcdn.msauth.net/browser/2.38.1/js/msal-browser.min.js";

let SSO_MSAL;          // msal.PublicClientApplication
let MSAL_SCRIPT_P;     // resolves when <script> loaded
let MSAL_INIT_P;       // resolves when MSAL.initialize() completed
let HANDLE_REDIRECT_DONE = false; // ensure handleRedirectPromise runs once

async function SSOmicrosoftLogout(fullLogout = false) {
   if(!SSO_MSAL) { return false; }
   try {
      if(fullLogout) {
         const activeAccount = SSO_MSAL.getActiveAccount();
         if(activeAccount) {
            await SSO_MSAL.logoutRedirect({
               account              : activeAccount,
               postLogoutRedirectUri: "/",
            });
            return true;
         }
      }

      // Soft logout (local-only): clear app/session state and release instance
      try {
         const cache = SSO_MSAL.getTokenCache?.();
         if(cache?.clear) { await cache.clear(); }
      } catch(_) {}

      try { SSO_MSAL.setActiveAccount && SSO_MSAL.setActiveAccount(null); }
      catch(_) { }

      SSO_MSAL  = null;
      SESScache = null;
      sessionStorage.clear();
      return true;
   } catch(_) { return false; }
}

function SSOinitMicrosoftSuccess(response) {
   if(response?.success && response?.data?.client_id) { return SSOloadMicrosoftApi(response.data.client_id, response.data.organization_id); }
   if(response?.error)                                { SESSshowMicrosoftError(response.error); }
   SSOupdateMicrosoft(true);
}

async function SSOloadMicrosoftApi(clientId, organizationId) {
   SSOupdateMicrosoft(false);
   try {
      await SSOloadMsalScriptOnce();
      await SSOinitMicrosoftSignIn(clientId, organizationId);
   } catch(error) {
      SSOupdateMicrosoft(true);
      SESSshowMicrosoftError(error?.message || String(error));
   }
}

async function SSOinitMicrosoftSignIn(clientId, organizationId) {
   if(typeof msal === "undefined") { return SSOupdateMicrosoft(true); }
   await SSOmicrosoftInitMsal(clientId, organizationId);
}

async function SSOmicrosoftInitMsal(clientId, organizationId) {
   const authority = organizationId ? `https://login.microsoftonline.com/${organizationId}` : `https://login.microsoftonline.com/organizations`;

   const msalConfig = {
      auth : {clientId, authority, redirectUri: window.location.origin},
      cache: {cacheLocation: "sessionStorage", storeAuthStateInCookie: false},
   };

   if(!SSO_MSAL) { SSO_MSAL = new msal.PublicClientApplication(msalConfig); }

   // Initialize exactly once
   if(!MSAL_INIT_P) { MSAL_INIT_P = SSO_MSAL.initialize(); }
   await MSAL_INIT_P;

   SSOshowSpinner();

   // Handle redirect exactly once per page load
   let response = null;
   if(!HANDLE_REDIRECT_DONE) {
      response             = await SSO_MSAL.handleRedirectPromise();
      HANDLE_REDIRECT_DONE = true;
   }

   if(response?.account) {
      SSO_MSAL.setActiveAccount(response.account);
      return SSOhandleMicrosoftResponse(response);
   }

   // Try to find/set an account
   let account = SSO_MSAL.getActiveAccount();
   if(!account) {
      const all = SSO_MSAL.getAllAccounts();
      if(all?.length) {
         account = all[0];
         SSO_MSAL.setActiveAccount(account);
      }
   }

   if(account && SESScache?.sso_provider === "Microsoft") { await SSOacquireMicrosoftTokenSilent(); }
   else {
      SSOcreateMicrosoftButton();
      SSOhideSpinner();
   }
}

async function SSOacquireMicrosoftTokenSilent() {
   if(SSO_MSAL == null) { return SSOinit(); }

   const active = SSO_MSAL.getActiveAccount();
   if(!active) { return SSOupdateMicrosoft(false); }

   const tokenRequest = {
      scopes      : ['openid', 'profile', 'email', ...MS_GRAPH_SCOPES],
      account     : active,
      forceRefresh: true
   };

   if(!SESScache) { return SSOmicrosoftpromptLogin(tokenRequest.scopes); }

   try {
      const response = await SSO_MSAL.acquireTokenSilent(tokenRequest);
      SSO_MSAL.setActiveAccount(response.account);
      SSOhandleMicrosoftResponse(response);
   } catch(error) {
      if(typeof msal !== "undefined" && error instanceof msal.InteractionRequiredAuthError) {
         SSOmicrosoftpromptLogin(tokenRequest.scopes);
      } else {
         SESSshowMicrosoftError(error?.message || "Unable to acquire Microsoft token silently.");
         SSOhideSpinner();
         SESSlogout();
      }
   }
}

function SSOmicrosoftpromptLogin(scopes) {
   if(!SSO_MSAL) { return; }
   SSO_MSAL.loginRedirect({scopes, prompt: "select_account"});
}

function SSOhandleMicrosoftSignIn(event) {
   SESShideMicrosoftError();
   if(event) {
      event.preventDefault();
      event.stopPropagation();
   }

   if(!SSO_MSAL) { return SESSshowMicrosoftError('Microsoft authentication not properly initialized'); }
   SSOshowSpinner()
   const active = SSO_MSAL.getActiveAccount();
   if(active) { return SSOacquireMicrosoftTokenSilent(); }

   const loginRequest = {
      scopes: ['openid', 'profile', 'email', ...MS_GRAPH_SCOPES],
      prompt: 'select_account',
   };
   SSO_MSAL.loginRedirect(loginRequest);
}

function SSOhandleMicrosoftResponse(response) {
   APIcall('auth/verify', {provider: 'microsoft', credential: response.idToken, token: response.accessToken}, SSOverifyMicrosoftCallback);
}

function SSOverifyMicrosoftCallback(response) {
   SSOhideSpinner();
   if(response?.success && response?.data?.user) {
      SESScache = response.data;
      SESSonLoginSuccess(response.data.user, response);
   } else {
      SSOupdateMicrosoft(true);
      SESSshowMicrosoftError(response.error || 'Microsoft authentication failed.');
   }
}

function SSOcreateMicrosoftButton() {
   const buttonContainer = document.querySelector('.SESSmicrosoftButton');
   if(!buttonContainer) { return; }

   buttonContainer.innerHTML = `
    <button class="microsoft-signin-btn" type="button">
     ${SSOmicrosoftButtonSvg()}
      <span>Sign in with Microsoft</span>
    </button>
    <div class="microsoft-spinner" style="display: none;"> <div class="CORspinner"></div></div>
  `;

   const button = buttonContainer.querySelector(".microsoft-signin-btn");
   if(button) { button.addEventListener("click", SSOhandleMicrosoftSignIn); }
   const hasActive = !!(SSO_MSAL && SSO_MSAL.getActiveAccount() && SESScache?.sso_provider === "Microsoft");
   SSOupdateMicrosoft(hasActive);
}

function SSOmicrosoftButtonSvg() {
   return `
         <svg width="21" height="21" viewBox="0 0 21 21" fill="none" xmlns="http://www.w3.org/2000/svg">
            <path d="M0 0h10v10H0z" fill="#F25022"/>
            <path d="M11 0h10v10H11z" fill="#7FBA00"/>
            <path d="M0 11h10v10H0z" fill="#00A4EF"/>
            <path d="M11 11h10v10H11z" fill="#FFB900"/>
         </svg>
      `;
}

function SSOupdateMicrosoft(hide) {
   const microsoftContainer = document.querySelector(".SESSmicrosoftSignInContainer");
   if(microsoftContainer) {
      microsoftContainer.style.display = hide ? "none" : "flex";
   }
   SSOupdateDividerVisibility?.();
}

function SSOshowSpinner() {
   SSOcreateMicrosoftButton();
   const spinnerContainer = document.querySelector(".SESSmicrosoftButton .microsoft-spinner");
   const buttonContainer  = document.querySelector(".SESSmicrosoftButton .microsoft-signin-btn");
   if(spinnerContainer) { spinnerContainer.style.display = "block"; }
   if(buttonContainer)  { buttonContainer.style.display = "none"; }
}

function SSOhideSpinner() {
   const spinnerContainer = document.querySelector(".SESSmicrosoftButton .microsoft-spinner");
   const buttonContainer  = document.querySelector(".SESSmicrosoftButton .microsoft-signin-btn");
   if(spinnerContainer) {
      spinnerContainer.style.display = "none";
   }
   if(buttonContainer) {
      buttonContainer.style.display = "block";
   }
}

function SSOloadMsalScriptOnce() {
   if(typeof msal !== "undefined") { return Promise.resolve(); }
   if(MSAL_SCRIPT_P)               { return MSAL_SCRIPT_P; }

   const existing = document.querySelector(`script[src="${MSAL_SRC}"]`);
   if(existing) {
      MSAL_SCRIPT_P = new Promise((resolve, reject) => {
         if(existing.dataset.loaded === "true") { return resolve(); }
         existing.addEventListener("load", () => resolve());
         existing.addEventListener("error", (e) => reject(e));
      });
      return MSAL_SCRIPT_P;
   }

   MSAL_SCRIPT_P = new Promise((resolve, reject) => {
      const s   = document.createElement("script");
      s.src     = MSAL_SRC;
      s.async   = true;
      s.defer   = true;
      s.onload  = () => {
         s.dataset.loaded = "true";
         resolve();
      };
      s.onerror = (e) => reject(e);
      document.head.appendChild(s);
   });
   return MSAL_SCRIPT_P;
}
