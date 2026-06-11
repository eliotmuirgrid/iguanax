/* ==========================================================================
   CORmodel.js
   - Handles data polling and view updates
   ========================================================================== */

/* -------------------------------------------------------
   FAST MODEL
   High-priority polling for critical UI layout & Identity
   ------------------------------------------------------- */
function CORfastModelInit() {
   // Fast polling for instant UI feedback
   let ModelTime = 100;

   // 1. Poll Critical Data
   MODELpollerAdd('CORfastModel', 'CORlicenseData', ModelTime, 'license/get', {});
   MODELpollerAdd('CORfastModel', 'CORserverInfo', ModelTime, 'header', {});

   // 2. Update Layout & Identity immediately
   MODELviewAdd('CORfastModel', 'CORcommandBadgeUpdate'); // Show/Hide Command Icon
   MODELviewAdd('CORfastModel', 'CORauthorizedPeersBadgeUpdate'); // Inverse settings nav visibility
   MODELviewAdd('CORfastModel', 'CORserverColour');       // Sidebar Background Color
   MODELviewAdd('CORfastModel', 'CORserverLabel');        // Server Name Text
   MODELviewAdd('CORfastModel', 'CORiconsColour');        // Icon Colors (Contrast adjustment)
   MODELviewAdd('CORfastModel', 'CORserverBorder');       // Green border for peers

   // 3. Start the fast loop
   MODELforcePoll('CORfastModel');
   MODELstart('CORfastModel');
}

/* -------------------------------------------------------
   MAIN MODEL
   Standard polling for general application state
   ------------------------------------------------------- */
function CORmodelInit() {
   let ModelTime = 20;
   COL_TRC("Menu model update time =", ModelTime);

   // --- High Frequency / Standard Pollers ---
   MODELpollerAdd('CORmodel', 'CORnotifications', ModelTime, 'notifications/enabled', {});
   // MOVED: CORserverInfo is now in CORfastModel

   // --- Low Frequency / Heavy Pollers ---
   MODELpollerAdd('CORmodel', 'CORreleaseInfo', 43200, 'release/info', {}); // 12 hours
   MODELpollerAdd('CORmodel', 'CORautoUpgrade', 3600, 'upgrade/check_auto', {}); // every hour
   MODELpollerAdd('CORmodel', 'CORisService', 900, 'release/is_service', {});

   // --- Git & Server Operations (Can be slow) ---
   MODELpollerAdd('CORmodel', 'CORgitSshInfo', 900, 'git/check_install', {});
   MODELpollerAdd('CORmodel', 'CORgitCredential', 900, 'git/credentials', {});
   MODELpollerAdd('CORmodel', 'CORserviceData', 100, 'git/username');
   MODELpollerAdd('CORmodel', 'CORinstanceModule', 100, 'git/verify_instance_modules');
   MODELpollerAdd('CORmodel', 'CORsshKeyData', 100, 'git/private_key_path');
   MODELpollerAdd('CORmodel', 'CORserverData', 100, 'git/server');
   MODELpollerAdd('CORmodel', 'CORserversData', 100, 'git/servers/load');
   // MOVED: CORlicenseData is now in CORfastModel
   MODELpollerAdd('CORmodel', 'CORlastPush', 100, 'instance/last_push');
   MODELpollerAdd('CORmodel', 'CORorgsSet', 100, 'git/owners');

   // --- View Updates ---
   MODELviewAdd('CORmodel', 'CORtabTitleUpdate');
   // MOVED: CORserverLabel -> CORfastModel
   // MOVED: CORserverColour -> CORfastModel
   // MOVED: CORiconsColour -> CORfastModel
   // MOVED: CORserverBorder -> CORfastModel
   MODELviewAdd('CORmodel', 'CORsettingsIconDot');
   MODELviewAdd('CORmodel', 'CORsettingsAboutDot');
   MODELviewAdd('CORmodel', 'CORsettingsMyGitDot');
   MODELviewAdd('CORmodel', 'CORautoUpgradeWarning');
   MODELviewAdd('CORmodel', 'CORsettingsLicenseDot');
   MODELviewAdd('CORmodel', 'CORsettingsInstanceDot');
   MODELviewAdd('CORmodel', 'CORupdateHrefs');
   MODELviewAdd('CORmodel', 'CORlogoutTooltip');
   // MOVED: CORcommandBadgeUpdate -> CORfastModel

   MODELforcePoll('CORmodel');
   MODELstart('CORmodel');

   // Initialize the fast model alongside the main one
   CORfastModelInit();
}

/* ==========================================================================
   View & Helper Functions
   ========================================================================== */

function CORtabTitleUpdate() {
   let ServerLabel = MODELdata?.CORserverInfo?.data?.server_description;
   let Title = "IguanaX";
   if (ServerLabel) Title = ServerLabel;
   if (document.title !== Title) document.title = Title;
   if (STAtestChanglog) console.warn("You are running with STAtestChanglog enabled! \n\nTell Tyler if you see this or turn it off and commit it yourself hah.")
}

function CORserverLabel() {
   let LabelDiv = document.querySelector(".CORheaderServerLabel");
   if (!LabelDiv) return;
   let ServerLabel = MODELdata?.CORserverInfo?.data?.server_description;
   if (typeof ServerLabel !== "string" || !ServerLabel.trim()) return;
   if (LabelDiv.innerHTML !== ServerLabel) LabelDiv.innerHTML = ServerLabel;
}

function CORserverColour() {
   let Div = document.querySelector(".CORheaderGrid");
   if (!Div) return;
   let Colour = MODELdata?.CORserverInfo?.data?.server_color;
   // Safety check for undefined color to prevent white flash
   if (Colour && Div.style.backgroundColor !== Colour) Div.style.backgroundColor = Colour;
}

function CORupdateColour(div, colour) {
   if (!div) return;
   if (div.style.color !== colour) div.style.color = colour;
}

function CORiconsColour() {
   let Colour = MODELdata?.CORserverInfo?.data?.server_color;
   if (!Colour) return;

   let newColour = CORlightenColour(Colour);
   let divs = document.querySelectorAll(".CORheaderNavLinkStyle");
   for (icon of divs) { CORupdateColour(icon, newColour); }

   let div = document.querySelector(".CORheaderServerLabelStyle");
   CORupdateColour(div, newColour);
   div = document.querySelector(".CORheaderUserProfileStyle");
   CORupdateColour(div, newColour);
   div = document.querySelector(".CORheaderUserProfileIconStyle");
   if (!div) return;
   if (div.style.fill !== newColour) div.style.fill = newColour;
}

function CORserverBorder() {
   let Div = document.querySelector(".CORmain");
   if (!Div) return;
   let Class = "CORpeerBorder";
   let Peer = PEERisRemote();
   let Contains = Div.classList.contains(Class);
   if (Peer && !Contains) Div.classList.add(Class);
   if (!Peer && Contains) Div.classList.remove(Class);
}

function CORupgradeAvailable() { return MODELdata?.CORreleaseInfo?.data?.upgrade_available; }
function CORreleaseVersion() { return MODELdata?.CORreleaseInfo?.data?.version; }
function CORcannotConnect() {
   let Code = MODELdata?.CORreleaseInfo?.data?.code;
   return Code && (Code !== 200);
}
function CORisService() { return MODELdata?.CORisService?.data; }

function CORgitCreds() {
   return MODELdata?.CORgitCredential?.data || {};
}

function CORgitUsername() {
   return MODELdata?.CORserviceData?.data?.username || {};
}

function CORsshKeyData() {
   return MODELdata?.CORsshKeyData?.data || {};
}

function CORgitSshInfoData() {
   let Data = MODELdata?.CORgitSshInfo?.data;
   return Data;
}

function CORserverData() {
   return MODELdata?.CORserverData?.data?.server || "";
}

function CORserversData() {
   return MODELdata?.CORserversData?.data || {};
}

function CORlicenseData() {
   return MODELdata?.CORlicenseData?.data || {};
}

function CORgitService() {
   return MODELdata?.CORserverData?.data?.provider || "";
}

function CORgitSshProblem(Data, KeyGen) {
   let GitW = !Data?.installed;
   let SshW = !Data?.ssh_path;
   let KeygenW = !Data?.sshkeygen_path;
   return GitW || SshW || KeyGen && KeygenW;
}

function CORlicenseExpire() {
   return MODELdata?.CORlicenseData?.data?.license_message?.includes("Your existing license has expired");
}

function CORinstanceModuleWarning() {
   return MODELdata?.CORinstanceModule?.data?.error || "";
}

function CORlicenseLoadError() {
   return MODELdata?.CORlicenseData?.data?.license_error?.includes("License load error.");
}

function CORpushError() {
   return MODELdata?.CORlastPush?.data?.last_error !== "";
}

function CORpushWarningError() {
   let WarningDismissed = STORElocalRead("GINpushWarningDismissed");
   let NewWarning = MODELdata?.CORlastPush?.data?.last_warning;
   return NewWarning && (NewWarning !== WarningDismissed);
}

function CORwarningIcon(Class, On) {
   let Nav = document.querySelector('.' + Class);
   if (Nav == null) { console.warn("Couldn't find an option with that title"); }
   else if (On) { CORinsertWarningDiv(Nav); }
   else { CORremoveWarningDiv(Nav); }
}

function CORinsertWarningDiv(Nav) {
   let WarningDiv = Nav.querySelector(".CORwarningIcon");
   if (!WarningDiv) {
      let Div = document.createElement('div');
      Div.classList.add('CORwarningIcon');
      Div.classList.add('CORwarningIconOn');
      let Span = Nav.querySelector("span");
      Nav.insertBefore(Div, Span);
   }
}

function CORremoveWarningDiv(Nav) {
   let WarningDiv = Nav.querySelector(".CORwarningIcon");
   if (!WarningDiv) return;
   Nav.removeChild(WarningDiv);
}

function CORaboutWarning() {
   let Data = CORgitSshInfoData();
   return CORupgradeAvailable() || CORcannotConnect() || CORgitSshProblem(Data, true);
}

function CORsettingsAboutDot() {
   let Element = document.querySelector('.STAabout');
   if (!Element) return;
   if (CORaboutWarning()) Element.classList.add('SETwarningIcon');
   else Element.classList.remove('SETwarningIcon');
}

function CORlicenseWarning() {
   return CORlicenseExpire() || CORlicenseLoadError();
}

function CORsettingsLicenseDot() {
   let Element = document.querySelector('.STLlicense');
   if (!Element) return;
   if (CORlicenseWarning()) Element.classList.add('SETwarningIcon');
   else Element.classList.remove('SETwarningIcon');
}

function CORinstanceWarning() {
   return CORpushError() || CORpushWarningError() || CORinstanceModuleWarning();
}

function CORsettingsInstanceDot() {
   let Element = document.querySelector('.GINheader');
   if (!Element) return;
   if (CORinstanceWarning()) Element.classList.add('SETwarningIcon');
   else Element.classList.remove('SETwarningIcon');
}

function CORsettingsIconDot() {
   let AboutW = CORaboutWarning() || CORmyGitWarning() || CORlicenseExpire() || CORinstanceWarning() || CORlicenseLoadError();
   CORwarningIcon("CORsettingsIcon", AboutW)
}

function CORsettingsMyGitDot() {
   let Element = document.querySelector('.GITHgitHost');
   if (!Element) return;
   if (CORmyGitWarning()) Element.classList.add('SETwarningIcon');
   else Element.classList.remove('SETwarningIcon');
}

function CORserverError() {
   return !MODELdata?.CORserverData?.success;
}

function CORmyGitWarning() {
   return CORcredWarning() || CORcloudWarning() || CORuserWarning() || CORtokenWarning() || CORkeyFilesWarning() || CORsshWarning() || CORorgsWarning();
}

function CORcloudWarning() {
   return CORserverError();
}

function CORcredWarning() {
   let Data = CORgitCreds();
   return !Data?.name?.length || !Data?.email?.length;
}

function CORuserWarning() {
   let Service = CORgitService();
   let NeedsUsername = GITHneedsUsername(Service);
   let Username = CORgitUsername();
   return NeedsUsername && !Username?.length;
}

function CORkeyFilesWarning() {
   let Data = CORsshKeyData();
   return !Data?.private_key_exists || !Data?.public_key_exists;
}

function CORthemeCheckAndApply() {
   const username = SESScache?.username;
   const View = THMgetUserTheme(username);
   if (View === "dark") {
      document.body.classList.add("CORdarkTheme");
   } else if (View === "light") {
      document.body.classList.remove("CORdarkTheme");
   } else if (View === "browser") {
      const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
      if (prefersDark) {
         document.body.classList.add("CORdarkTheme");
      } else {
         document.body.classList.remove("CORdarkTheme");
      }
   }
   return View === "dark" || (View === "browser" && window.matchMedia('(prefers-color-scheme: dark)').matches);
}

function CORlogoutTooltip() {
   const user = document.querySelector(".CORheaderUser");
   content = `<div class="CORheaderTooltip">
               <div class="CORheaderLogoutUser CORheaderLogoutUserStyle">
                  <svg class="CORheaderLogoutUserIcon CORheaderLogoutUserIconStyle" width="20" height="20">
                     <use href="COR/CORiconLogoutUser.svg#CORiconLogoutUser"></use>
                  </svg>
                  Log Out
               </div>
         </div>`;
   POPtooltip({
      target: user,
      content: content,
      position: POPtooltipPosition.RIGHT,
      event: POPtooltipEvent.CLICK,
      style: 'CORlogoutTooltip',
      autoShow: false,
      callback: function () {
         CORlogoutAction();
         CORlogoutReposition();
      }
   });
}

function CORlogoutReposition() {
   const tooltip = document.querySelector('.CORlogoutTooltip');
   if (tooltip) {
      const width = tooltip.offsetWidth;
      tooltip.style.transform = `translateX(-${(width / 2) - 40}px)`;
   }
}
