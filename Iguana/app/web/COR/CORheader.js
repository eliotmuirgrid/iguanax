/* ==========================================================================
   CORheader.js
   - Generates the left sidebar navigation
   - Handles Local vs Remote Peer header states
========================================================================== */

/* New helper for the visual divider */
function CORheaderSeparator() {
   return /*html*/`<div class="CORheaderSeparator"></div>`;
}

function CORsectionIcon(Label, Hash, Icon, Class){
   return /*html*/`
   <a class="CORheaderNavLink CORheaderNavLinkStyle ${Class}" href="${Hash}" draggable="false">
      <svg class="CORheaderNavIcon" width="24" height="24" draggable="false">
         <use href="COR/${Icon}.svg#${Icon}"></use>
      </svg>
      <span class="CORheaderNavTitleStyle">${Label}</span>
   </a>`;
}

/* Special helper for Command Icon to handle default hidden state */
function CORcommandIconHtml() {
   return /*html*/`
   <a class="CORheaderNavLink CORheaderNavLinkStyle CORcommandIcon" href="#command" draggable="false" style="display:none">
      <svg class="CORheaderNavIcon" width="26" height="24" draggable="false">
         <use href="COR/CORiconCommand.svg#CORiconCommand"></use>
      </svg>
      <span class="CORheaderNavTitleStyle">COMMAND</span>
   </a>`;
}

function CORsectionHelp() {
   return /*html*/`
   <a class="CORheaderNavLink CORheaderNavLinkStyle CORhelpIcon" href="${LINKhelp}" target="_blank" draggable="false">
      <svg class="CORheaderNavIcon" width="24" height="24" draggable="false">
         <use href="COR/CORiconHelp.svg#CORiconHelp"></use>
      </svg>
      <span class="CORheaderNavTitleStyle">HELP</span>
   </a>`;
}

function CORlogoutIcon(){
   return /*html*/`
   <div class="CORheaderUser CORheaderUserStyle">
      <div class="CORheaderUserProfile CORheaderUserProfileStyle">
         <svg class="CORheaderUserProfileIcon CORheaderUserProfileIconStyle" width="24" height="24">
            <use href="COR/CORiconUserProfile.svg#CORiconUserProfile"></use>
         </svg>
         <span class="CORheaderUserProfileIconArrow">&blacktriangleright;</span>
      </div>
   </div>`;
}

function CORappIconDiv(){
   return /*html*/`
   <div class="CORheaderLogo CORheaderLogoStyle">
      <div class="CORheaderLogoContainer CORheaderLogoContainerStyle">
         <img src="COR/CORiguanaAppIcon.png" class="CORheaderLogoImageStyle" />
         <a class="CORheaderGlobalNotificationContainer CORheaderGlobalNotificationContainerStyle">
            <div class="CORheaderGlobalNotificationStyle"></div>
         </a>
      </div>
   </div>`;
}

function CORpeerHeaderHtml(){
   return /*html*/`
   <header class="CORheaderGrid">
      <div class="CORheaderWrap">
         ${CORappIconDiv()}
         ${CORheaderSeparator()}
         <nav class="CORheaderNav CORheaderNavStyle">
            ${CORsectionIcon("DASHBOARD",  "#dashboard",  "CORiconDashboard", "CORdashboardIcon")}
            ${CORsectionIcon("TRANSLATOR", "#translator", "CORiconTranslator", "CORtranslatorIcon")}
            ${CORsectionIcon("LOGS",       "#logs",       "CORiconLogs",      "CORlogIcon")}
            ${CORsectionIcon("SETTINGS",   "#settings",   "CORiconSettings",  "CORsettingsIcon")}
            ${CORsectionIcon("CLOSE",      "#grid",       "CORiconClose",     "CORiconClose")}
            ${CORsectionHelp()}
         </nav>
      </div>
      <div class="CORheaderServerLabelContainer CORheaderServerLabelContainerStyle">
         <div class="CORheaderServerLabel CORheaderServerLabelStyle"></div>
      </div>
      <div class="CORpeerPadding"></div>
   </header>`;
}

function CORpeerOfflineHeaderHtml(){
   return /*html*/`
   <header class="CORheaderGrid">
      <div class="CORheaderWrap">
         ${CORappIconDiv()}
         ${CORheaderSeparator()}
         <nav class="CORheaderNav CORheaderNavStyle">  
         </nav>
      </div>
      <div class="CORheaderServerLabelContainer CORheaderServerLabelContainerStyle">
         <div class="CORheaderServerLabel CORheaderServerLabelStyle"></div>
      </div>
      <div class="CORpeerPadding"></div>
   </header>`;
}

function CORnormalHeaderHtml(){
   return /*html*/`
   <header class="CORheaderGrid">
      <div class="CORheaderWrap">
         ${CORappIconDiv()}
         ${CORheaderSeparator()}
         <nav class="CORheaderNav CORheaderNavStyle">` +
            `${CORcommandIconHtml()}
            ${CORsectionIcon("DASHBOARD",  "#dashboard",  "CORiconDashboard", "CORdashboardIcon")}
            ${CORsectionIcon("TRANSLATOR", "#translator", "CORiconTranslator", "CORtranslatorIcon")}
            ${CORsectionIcon("LOGS",       "#logs",       "CORiconLogs",      "CORlogIcon")}` + 
            // ${CORsectionIcon("IGUANAS",    "#iguanas",    "CORiconCluster",   "CORclusterIcon")} // IX-4335 disabling for release
            `${CORsectionIcon("SETTINGS",   "#settings",   "CORiconSettings",  "CORsettingsIcon")}
            ${CORsectionHelp()}
         </nav>
      </div>
      <div class="CORheaderServerLabelContainer CORheaderServerLabelContainerStyle">
         <div class="CORheaderServerLabel CORheaderServerLabelStyle"></div>
      </div>
      ${CORlogoutIcon()}
   </header>`;
}

function CORheaderHtml(){
   if (PEERisRemote()){
      return CORpeerHeaderHtml();
   } else {
      return CORnormalHeaderHtml();
   }
}

/* ==========================================================================
   View Updates
   ========================================================================== */

function CORglobalDashboardLicenseBoolParse(Value) {
   if (Value === true || Value === false) return Value;
   if (typeof Value === "number") {
      if (Value === 1) return true;
      if (Value === 0) return false;
      return null;
   }
   if (typeof Value === "string") {
      let normalized = Value.trim().toLowerCase();
      if (normalized === "true" || normalized === "1" || normalized === "yes" || normalized === "on" || normalized === "enabled") return true;
      if (normalized === "false" || normalized === "0" || normalized === "no" || normalized === "off" || normalized === "disabled") return false;
   }
   return null;
}

function CORglobalDashboardLicenseCacheKey() { return "CORglobal_dashboard:" + String(window.location.host || ""); }
function CORglobalDashboardLicenseCacheRead() { return CORglobalDashboardLicenseBoolParse(STORElocalRead(CORglobalDashboardLicenseCacheKey())); }
function CORglobalDashboardLicenseCacheWrite(Value) {
   let parsed = CORglobalDashboardLicenseBoolParse(Value);
   if (parsed !== true && parsed !== false) return;
   STORElocalCreate(CORglobalDashboardLicenseCacheKey(), parsed ? "1" : "0");
}

function CORglobalDashboardFeatureState(Data, FeatureKey) {
   let data = Data || {};
   let license = data.license || {};
   let values = [
      license[FeatureKey],
      data[FeatureKey],
      license.features && license.features[FeatureKey],
      data.features && data.features[FeatureKey]
   ];
   for (let i = 0; i < values.length; i++) {
      let parsed = CORglobalDashboardLicenseBoolParse(values[i]);
      if (parsed === true || parsed === false) return parsed;
   }
   let featureList = license.features || data.features;
   if (Array.isArray(featureList)) {
      for (let j = 0; j < featureList.length; j++) {
         let feature = String(featureList[j] || "").trim().toLowerCase().replace(/\s+/g, "_");
         if (feature === FeatureKey) return true;
      }
   }
   return null;
}

function CORglobalDashboardLicenseStateFromPayload(Data){
   let data = Data || {};
   let federated = CORglobalDashboardFeatureState(data, "federated_dashboard");
   if (federated === true || federated === false) return federated;
   let globalDash = CORglobalDashboardFeatureState(data, "global_dashboard");
   if (globalDash === true || globalDash === false) return globalDash;

   let status = String((data.license && data.license.license_status) || "").toLowerCase();
   if (status === "no license found" || status === "license expired") return false;

   return null;
}

function CORglobalDashboardLicenseState(){
   let live = CORglobalDashboardLicenseStateFromPayload(CORlicenseData());
   if (live === true || live === false) {
      CORglobalDashboardLicenseCacheWrite(live);
      return live;
   }
   return CORglobalDashboardLicenseCacheRead();
}
function CORcommandFeatureEnabled() { let state = CORglobalDashboardLicenseState(); return (state === true) ? true : (state === false ? false : null); }
function CORauthorizedPeersFeatureEnabled() { let state = CORglobalDashboardLicenseState(); return (state === false) ? true : (state === true ? false : null); }

/* Checks license and toggles the Command icon visibility */
function CORcommandBadgeUpdate(){
   // 1. Find the element
   let commandIcon = document.querySelector('.CORcommandIcon');
   if (!commandIcon) return;

   // 2. Check the license data
   const hasLicense = CORcommandFeatureEnabled();
   if (hasLicense !== true && hasLicense !== false) return;

   // 3. Toggle Visibility
   // 'flex' matches the display style of other nav icons
   if (hasLicense) {
      if(commandIcon.style.display === 'none') commandIcon.style.display = 'flex';
      if (commandIcon.getAttribute('href') !== '#command') commandIcon.setAttribute('href', '#command');
   } else {
      if(commandIcon.style.display !== 'none') commandIcon.style.display = 'none';
      if (commandIcon.getAttribute('href') !== '#dashboard') commandIcon.setAttribute('href', '#dashboard');
   }
}

/* Inverse of command licensing: target-only Settings entry */
function CORauthorizedPeersBadgeUpdate() {
   let link = document.querySelector('.AUPauthorizedPeersNav');
   if (!link) return;
   let row = link.closest('.SETsettingsNavPageLayout') || link.parentElement;
   if (!row) return;

   let enabled = CORauthorizedPeersFeatureEnabled();
   if (enabled !== true && enabled !== false) return;
   let nextDisplay = enabled ? 'flex' : 'none';
   if (row.style.display !== nextDisplay) row.style.display = nextDisplay;
   if (!enabled && link.classList.contains('selected')) {
      link.classList.remove('selected');
   }
}
