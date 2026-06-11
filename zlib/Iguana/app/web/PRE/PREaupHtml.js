/* ==========================================================================
   PRE Authorized Peers HTML
   - Authorized Peers HTML builders
========================================================================== */

function PREauthorizedPeersGridHeaderHtml() {
   return /*html*/`
      <div class="PREgridHeader">
         <div class="PREcolPeer">Name</div>
         <div class="PREcolLink">Connection</div>
         <div class="PREcolLatency">Latency</div>
         <div class="PREcolAccess">Access</div>
         <div class="PREcolActions">Action</div>
      </div>`;
}

function PREauthorizedPeersGridHtml() {
   return PREgridScreenHtml(
      "PREscreenContainerAuthorized",
      "PREgridContentAreaAuthorized",
      "PREgridTarget",
      PREauthorizedPeersGridHeaderHtml()
   );
}

function PREauthorizedPeersUnavailableHtml() {
   return PREfeatureNoticeHtml(
      "AUTHORIZED PEERS UNAVAILABLE",
      "All peer connections for this instance are managed through the Command Center.",
      [
         { href: "#command", label: "Go to Command Center", primary: true },
         { href: "#settings", label: "Back to Settings" }
      ]
   );
}

function PREsettingsPaneModeClass(PaneMode) {
   return (String(PaneMode || "") === "manage") ? "PREpaneModeManage" : "PREpaneModeUnavailable";
}

function PREsettingsPaneHtml(PaneClass, PaneMode, HeaderClass, Title, Description, HeaderActionHtml, ContentHtml) {
   return /*html*/`
   <div class="SETsettingsPane SETsettingsPaneLayout ${PaneClass} ${PREsettingsPaneModeClass(PaneMode)}">
      <div class="${HeaderClass}">
         <div class="SETgitTitle"><span>${ESChtmlEscape(Title)}</span>${HeaderActionHtml}</div>
      </div>
      <div class="SETpageDescription">${ESChtmlEscape(Description)}</div>
      ${ContentHtml}
   </div>`;
}

function PREauthorizedPeersPaneMode(CanManage) {
   return CanManage ? "manage" : "unavailable";
}

function PREauthorizedPeersHeaderActionHtml(CanManage) {
   if (!CanManage) return "";
   return /*html*/`
      <div class="AUPauthorizedPeersHeaderActions">
         <div class="AUPauthorizedPeersShareBtn PREbtnMyDetails BUTTONstandard" title="Share My Details">
            <svg class="PREbtnMyDetailsIcon" viewBox="0 0 24 24" fill="currentColor" aria-hidden="true">
               <path d="M18 16.08c-.76 0-1.44.3-1.96.77L8.91 12.7a2.97 2.97 0 0 0 0-1.39l7-4.11A2.99 2.99 0 1 0 15 5a2.9 2.9 0 0 0 .06.59l-7.12 4.18a3 3 0 1 0 0 4.46l7.12 4.18c-.04.18-.06.38-.06.59a3 3 0 1 0 3-2.92z"/>
            </svg>
            <span>Share My Details</span>
         </div>
         <div class="BUTTONstandard BUTTONaction AUPauthorizedPeersEditBtn" title="Authorize a new peer">+ Authorize Peer</div>
      </div>`;
}

function PREauthorizedPeersContentHtml(CanManage) {
   if (CanManage) return PREauthorizedPeersGridHtml();
   return `<div class="PREgridContentAreaAuthorized">${PREauthorizedPeersUnavailableHtml()}</div>`;
}

function PREauthorizedPeersPaneHtml(CanManage) {
   var canManage = (CanManage === true);
   return PREsettingsPaneHtml(
      "AUPauthorizedPeersPane",
      PREauthorizedPeersPaneMode(canManage),
      "AUPauthorizedPeersHeader",
      "Authorized Peers",
      "Manage which remote instances are authorized to connect to and read data from this server.",
      PREauthorizedPeersHeaderActionHtml(canManage),
      PREauthorizedPeersContentHtml(canManage)
   );
}
