function AUTHlisteners() {
   let div = document.querySelector(".AUTHmodal");
   if(!div) { return; }
   div.removeEventListener("click", AUTHclick);
   div.addEventListener("click", AUTHclick);
}

function AUTHclick(e) {
   if(e.target.closest(".AUTHsave")) { AUTHsave(); }
}

function AUTHcombinedSelected() {
   let div = document.querySelector(".AUTHcombinedSelect");
   return div?.value || "Disabled";
}

function AUTHclientIdInputed() {
   let div = document.querySelector(".AUTHclientIdInput");
   return div?.value || "";
}

function AUTHorganizationIdInputed() {
   let div = document.querySelector(".AUTHorganizationIdInput");
   return div?.value || "";
}

function AUTHentityIdInputed() {
   let div = document.querySelector(".AUTHentityIdInput");
   return div?.value || "";
}

function AUTHacsUrlInputed() {
   let div     = document.querySelector(".AUTHacsUrlInput");
   let baseUrl = div?.value || "";
   if(!baseUrl) { return ""; }
   const suffix = '/saml/verify';
   return baseUrl.endsWith(suffix) ? baseUrl : baseUrl + suffix;
}

function AUTHmetadataPathInputed() {
   let div = document.querySelector(".AUTHmetadataPathInput");
   return div?.value || "";
}

function AUTHsave() {
   let CombinedValue = AUTHcombinedSelected();
   let Config        = AUTHgetConfigFromValue(CombinedValue);
   let Args          = {mode: Config.mode, provider: Config.provider};

   if(Config.mode === 'Client') {
      Args.client_id       = AUTHclientIdInputed();
      Args.organization_id = AUTHorganizationIdInputed();
   } else if(Config.mode === 'SAML') {
      Args.entity_id     = AUTHentityIdInputed();
      Args.acs_url       = AUTHacsUrlInputed();
      Args.metadata_path = AUTHmetadataPathInputed();
   }
   const sameData = AUTHcompareData(Args);
   APIcall("auth/set", Args, function(R) {
      if(!R.success) {
         SNCKsnackbar('error', R?.error);
      } else {
         if(!sameData) {
            let Message = GINsetSsoProvider();
            GINaddAndCommit({message: Message, settings: true});
            SSOifSsoUser(SESSlogout);
         }
      }
      window.location = "#settings/authentication";
   });
}