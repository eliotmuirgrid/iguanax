const SSO_CONFIG = Object.freeze({
   DISABLED   : {value: 'Disabled', mode: 'Disabled', provider: 'None'},
   MS_CLIENT  : {value: 'Microsoft_Client', mode: 'Client', provider: 'Microsoft'},
   MS_SAML    : {value: 'Microsoft_SAML', mode: 'SAML', provider: 'Microsoft'},
   GOOGLE_SAML: {value: 'Google_SAML', mode: 'SAML', provider: 'Google'}
});

function AUTHgetCombinedValue(Mode, Provider) {
   if(Mode === 'Disabled')                           { return SSO_CONFIG.DISABLED.value; }
   if(Mode === 'Client' && Provider === 'Microsoft') { return SSO_CONFIG.MS_CLIENT.value; }
   if(Mode === 'SAML' && Provider === 'Microsoft')   { return SSO_CONFIG.MS_SAML.value; }
   if(Mode === 'SAML' && Provider === 'Google')      { return SSO_CONFIG.GOOGLE_SAML.value; }
   return SSO_CONFIG.DISABLED.value;
}

function AUTHgetConfigFromValue(Value) {
   switch(Value) {
      case SSO_CONFIG.MS_CLIENT.value:   return SSO_CONFIG.MS_CLIENT;
      case SSO_CONFIG.MS_SAML.value:     return SSO_CONFIG.MS_SAML;
      case SSO_CONFIG.GOOGLE_SAML.value: return SSO_CONFIG.GOOGLE_SAML;
      default:                           return SSO_CONFIG.DISABLED;
   }
}

function AUTHmodeBasicHtml(Mode, Provider, ClientConfig, SamlConfig, Edit) {
   const combinedValue = AUTHgetCombinedValue(Mode, Provider);
   const config        = AUTHgetConfigFromValue(combinedValue);
   return /*html*/`
   <div class="AUTHbox">
      <div class="FORMlabel AUTHlabelAuthId">Authentication Method:</div>
      <div>${AUTHcombinedSelect(combinedValue, Edit)}</div>
      ${config.mode === 'Client' ? AUTHclientFieldsHtml(ClientConfig, Edit) : ''}
      ${config.mode === 'SAML' ? AUTHsamlFieldsHtml(SamlConfig, Edit) : ''}
   </div>`;
}

function AUTHclientFieldsHtml(ClientConfig, Edit) {
   const clientId = ClientConfig?.client_id || '';
   const orgId    = ClientConfig?.organization_id || '';
   return /*html*/`
      <div class="AUTHlabelId FORMlabel">Client Id:</div>
      <div>${AUTHclientIdInputHtml(clientId, Edit)}</div>
      <div class="AUTHlabelId FORMlabel">Tenant Id:</div>
      <div>${AUTHorganizationIdInputHtml(orgId, Edit)}</div>
   `;
}

function AUTHsamlFieldsHtml(SamlConfig, Edit) {
   const entityId     = SamlConfig?.entity_id || '';
   const acsUrl       = SamlConfig?.acs_url || '';
   const metadataPath = SamlConfig?.metadata_path || '';
   return /*html*/`
      <div class="AUTHlabelId FORMlabel">Entity Id:</div>
      <div>${AUTHentityIdInputHtml(entityId, Edit)}</div>
      <div class="AUTHlabelId FORMlabel">ACS URL:</div>
      <div>${AUTHacsUrlInputHtml(acsUrl, Edit)}</div>
      <div class="AUTHlabelId FORMlabel">Metadata Path:</div>
      <div>${AUTHmetadataPathInputHtml(metadataPath, Edit)}</div>
   `;
}

function AUTHlicenseWarningBox() {
   return /*html*/`
   <div class="AUTHlicenseWarningBox">Your current license does not include SSO access. [<a href="#settings/license">Upgrade your plan</a>]</div>`;
}

function AUTHproviderHtml(Data) {
   let Edit           = AUTHeditPage();
   let Licensed       = AUTHlicensed();
   let Mode           = Data?.mode || 'Disabled';
   let Provider       = Data?.provider || 'None';
   let LicenseWarning = (!Edit && !Licensed) ? AUTHlicenseWarningBox() : '';
   let BasicHtml      = AUTHmodeBasicHtml(Mode, Provider, Data, Data, Edit);
   return LicenseWarning + BasicHtml;
}

function AUTHcombinedSelect(CombinedValue, Edit) {
   const displayText = CombinedValue === SSO_CONFIG.DISABLED.value ? 'Disabled' :
      CombinedValue === SSO_CONFIG.MS_CLIENT.value ? 'Microsoft - Client App' :
         CombinedValue === SSO_CONFIG.MS_SAML.value ? 'Microsoft - SAML' :
            CombinedValue === SSO_CONFIG.GOOGLE_SAML.value ? 'Google - SAML' : 'Disabled';

   if(!Edit) { return `<div class="AUTHmodeId">${displayText}</div>`; }

   const disabled   = CombinedValue === SSO_CONFIG.DISABLED.value ? ' selected' : '';
   const msClient   = CombinedValue === SSO_CONFIG.MS_CLIENT.value ? ' selected' : '';
   const msSaml     = CombinedValue === SSO_CONFIG.MS_SAML.value ? ' selected' : '';
   const googleSaml = CombinedValue === SSO_CONFIG.GOOGLE_SAML.value ? ' selected' : '';

   return /*html*/`
   <select class="FORMselectBox AUTHcombinedSelect">
      <option value="${SSO_CONFIG.DISABLED.value}"${disabled}>Disabled</option>
      <option value="${SSO_CONFIG.MS_CLIENT.value}"${msClient}>Microsoft - Client App</option>
      <option value="${SSO_CONFIG.MS_SAML.value}"${msSaml}>Microsoft - SAML</option>
      <option value="${SSO_CONFIG.GOOGLE_SAML.value}"${googleSaml}>Google - SAML</option>
   </select>`;
}

function AUTHclientIdInputHtml(ClientId, Edit) {
   if(!Edit) { return `<div class="AUTHlabelId">${ClientId}</div>`; }
   return `<input class="FORMinput AUTHclientIdInput" value="${ClientId}">`;
}

function AUTHorganizationIdInputHtml(OrganizationId, Edit) {
   if(!Edit) { return `<div class="AUTHlabelId">${OrganizationId}</div>`; }
   return `<input class="FORMinput AUTHorganizationIdInput" value="${OrganizationId}">`;
}

function AUTHentityIdInputHtml(EntityId, Edit) {
   if(!Edit) { return `<div class="AUTHlabelId">${EntityId}</div>`; }
   return `<input class="FORMinput AUTHentityIdInput" value="${EntityId}">`;
}

function AUTHacsUrlInputHtml(AcsUrl, Edit) {
   const suffix     = '/saml/verify';
   const baseUrl    = AcsUrl.endsWith(suffix) ? AcsUrl.slice(0, -suffix.length) : AcsUrl;
   const displayUrl = baseUrl + suffix;

   if(!Edit) { return `<div class="AUTHlabelId">${displayUrl}</div>`; }
   return /*html*/`
   <div class="FORMinputGroup AUTHacsUrlGroup">
      <input class="FORMinput AUTHacsUrlInput" value="${baseUrl}">
      <div class="AUTHacsUrlSuffix">${suffix}</div>
   </div>`;
}

function AUTHmetadataPathInputHtml(MetadataPath, Edit) {
   if(!Edit) { return `<div class="AUTHlabelId">${MetadataPath}</div>`; }
   return /*html*/`
   <div class="FORMinputGroup AUTHmetadataPathGroup">
      <input class="FORMinput AUTHmetadataPathInput" value="${MetadataPath}">
      <div class="AUTHmetadataPathBrowse BUTTONstandard BUTTONcancel">...</div>
   </div>`;
}

function AUTHupdateProvider(Data) {
   let div = document.querySelector(".AUTHtarget");
   if(!div) {
      return;
   }
   let Provider;
   if(Data) {
      Provider     = Data;
      AUTHauthData = undefined;
   } else {
      Provider = AUTHdata();
   }
   let Html = AUTHproviderHtml(Provider);
   if(!DEEPequal(AUTHauthData, Provider)) {
      div.innerHTML = Html;
   }
   AUTHauthData = Provider;
   AUTHinputSetters();
}

function AUTHinputSetters() {
   let inp = document.querySelector(".AUTHcombinedSelect");
   if(inp) {
      FORMfocus(inp);
      FORMenterListener(inp, AUTHsave);
      inp.addEventListener('change', () => {
         let CurrentData = AUTHdata();
         let CombinedValue = document.querySelector(".AUTHcombinedSelect").value;
         let Config        = AUTHgetConfigFromValue(CombinedValue);
         let UpdatedData   = {...CurrentData, mode: Config.mode, provider: Config.provider};
         AUTHupdateProvider(UpdatedData);
      });
   }

   inp = document.querySelector(".AUTHclientIdInput");
   if(inp) { FORMenterListener(inp, AUTHsave); }

   inp = document.querySelector(".AUTHorganizationIdInput");
   if(inp) { FORMenterListener(inp, AUTHsave); }

   inp = document.querySelector(".AUTHentityIdInput");
   if(inp) { FORMenterListener(inp, AUTHsave); }

   inp = document.querySelector(".AUTHacsUrlInput");
   if(inp) { FORMenterListener(inp, AUTHsave); }

   inp = document.querySelector(".AUTHmetadataPathInput");
   if(inp) { FORMenterListener(inp, AUTHsave); }

   let browseBtn = document.querySelector(".AUTHmetadataPathBrowse");
   if(browseBtn) {
      browseBtn.addEventListener('click', () => {
         let pathInput = document.querySelector(".AUTHmetadataPathInput");
         FILbrowser({
            value: pathInput.value,
            new_folder: false,
            show_hidden: true,
            file_types: ['xml'],
            callback: function(selectedPath) {
               pathInput.value = selectedPath;
            }
         });
      });
   }
}
