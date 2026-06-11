/* ==========================================================================
   PRE Peer Details View
   - Dedicated "PREpeerModel" for independent polling
   - Fetches List, Status, AND Resources
========================================================================== */

var PRE_PV = {
   latencyById: {}
};

function PREpeerScreen() {
   PAGEremovePopouts();
   PREpeerTryWarmOpen();
   PREpeerModelInit();
}

function PREpeerModelInit() {
   // We define a separate model just for the modal
   // allowing independent polling intervals (e.g. faster updates while modal is open)
   MODELpollerAdd('PREpeerModel', 'PREpeerList',      3, 'iguanas/list',      {});
   MODELpollerAdd('PREpeerModel', 'PREpeerStatus',    1, 'iguanas/status',    {});
   MODELpollerAdd('PREpeerModel', 'PREpeerResources', 3, 'iguanas/resources', {});
   MODELviewAdd('PREpeerModel', 'PREpeerRender');
   MODELforcePoll('PREpeerModel');
   MODELstart('PREpeerModel');
}

function PREpeerFromSources(id, list, statusMap, resMap) {
   var peerConfig = null;
   var peerId = String(id || "");
   if (!peerId) return null;
   if (!list || !list.length) return null;

   for (var i = 0; i < list.length; i++) {
      if (String(list[i].id || "") === peerId) {
         peerConfig = list[i];
         break;
      }
   }
   if (!peerConfig) return null;

   var liveStatus = (statusMap && statusMap[peerId]) || {};
   var liveRes = (resMap && resMap[peerId]) || {};
   return { 
      ...peerConfig, 
      ...liveStatus, 
      ...liveRes 
   };
}

function PREpeerModalCacheGet(id) {
   return PREpeerFromSources(id, PREpeerModalListGet(), PREpeerModalStatusGet(), PREpeerModalResourcesGet());
}

function PREpeerMainCacheGet(id) {
   return PREpeerFromSources(id, PREpeerGridPeerList(), PREpeerGridStatusGet(), PREpeerGridResourcesGet());
}

function PREpeerGet(id) {
   return PREpeerModalCacheGet(id) || PREpeerMainCacheGet(id);
}

function PREpeerTryWarmOpen() {
   if (PAGEhash() !== PRErouteDetailsPath(PAGEhash())) return;
   if (document.querySelector('.PREpeerModal')) return;
   var peerId = PREpeerParam();
   if (!peerId) return;
   var warmPeer = PREpeerMainCacheGet(peerId) || PREpeerModalCacheGet(peerId);
   if (!warmPeer) return;
   PREshowPeerModal(warmPeer);
}

/* --- View Controller --- */

function PREpeerRender() {
   if (PAGEhash() !== PRErouteDetailsPath(PAGEhash())) return;
   var peerId = PREpeerParam();
   if (!peerId) return;

   var Peer = PREpeerGet(peerId);
   if (!Peer) return; 

   var Root = document.querySelector('.PREpeerModal');
   if (Root) {
      PREpeerUpdate(Root, Peer);
      return;
   }
   PREshowPeerModal(Peer);
}

function PREpeerResourceBarsTryUpdate(Container, Peer) {
   if (Peer.error_code == 403) return false;
   var isConnected = (Peer.connected === true || Peer.connected === "true");
   if (!isConnected) return false;
   if (Peer.cpu_percent === undefined || Peer.cpu_percent === null) return false;

   var rows = Container.querySelectorAll('.PREmodalBarRow');
   if (!rows || rows.length < 3) return false;

   var pcts = [
      PREpercentClamp(Peer.cpu_percent),
      PREpercentClamp(PREresourceMemPercent(Peer)),
      PREpercentClamp(PREpeerGridDiskPct(Peer))
   ];
   var labels = ["CPU", "MEM", "DSK"];

   for (var i = 0; i < 3; i++) {
      var row = rows[i];
      if (!row) return false;
      var track = row.querySelector('.PREmodalBarTrack');
      var fill = row.querySelector('.PREmodalBarFill');
      var val = row.querySelector('.PREmodalBarValue');
      if (!track || !fill || !val) return false;

      var pct = pcts[i];
      PREsetElementClassIfChanged(fill, "PREmodalBarFill PREmodalBarFillActive");
      PREsetElementClassIfChanged(val, "PREmodalBarValue");
      PREsetElementAttrIfChanged(fill, 'style', "width:" + String(pct) + "%");
      PREsetElementTextIfChanged(val, String(pct) + "%");
   }

   return true;
}

function PREpeerModalTitleUpdate(Root, Peer) {
   var modal = Root ? Root.closest('.POPpopoutCard') : null;
   var modalTitle = modal ? modal.querySelector('.POPpopoutCard-title h3') : null;
   PREsetElementTextIfChanged(modalTitle, String(Peer.name || Peer.id || "Peer Details"));
}

function PREpeerStatusConnected(Peer) {
   return (Peer.connected === true || Peer.connected === "true");
}

function PREpeerStateRowUpdate(Root, IsConnected) {
   var stateDot = Root.querySelector('.PREpeerStateDot');
   var stateText = Root.querySelector('.PREpeerStateText');
   if (!stateDot || !stateText) return;
   PREsetElementClassIfChanged(stateDot, "PREstateDot PREpeerStateDot " + (IsConnected ? "PREstateDotConnected" : "PREstateDotOffline"));
   PREsetElementClassIfChanged(stateText, "PREpeerStateText " + (IsConnected ? "PREpeerStateTextConnected" : "PREpeerStateTextOffline"));
   PREsetElementTextIfChanged(stateText, IsConnected ? "Connected" : "Disconnected");
}

function PREpeerLatencySnapshot(Peer, IsConnected) {
   var lat = parseFloat(Peer.latency_ms);
   var peerId = String(Peer.id || "");
   var latTier = PREpeerGridLatencyTierWithHysteresis(PRE_PV.latencyById, peerId, IsConnected, lat);
   return {
      latDisp: (IsConnected && Number.isFinite(lat)) ? (Math.round(lat) + " ms") : "—",
      latTier: latTier,
      latClass: PREpeerGridLatencyTextClassForTier(latTier)
   };
}

function PREpeerLatencyUpdate(Root, Peer, IsConnected) {
   var snap = PREpeerLatencySnapshot(Peer, IsConnected);
   var latDot = Root.querySelector('.PRElatencyDot');
   if (latDot) {
      PREsetElementClassIfChanged(latDot, "PREstatusDot PRElatencyDot " + PREpeerGridLatencyDotClassForTier(snap.latTier));
   }
   var latDiv = Root.querySelector('.PRElatencyVal');
   if (latDiv) {
      PREsetElementClassIfChanged(latDiv, "PREmono PRElatencyVal" + (snap.latClass ? (" " + snap.latClass) : ""));
   }
   PREsetElementTextIfChanged(latDiv, snap.latDisp);
}

function PREpeerResourcesUpdate(Root, Peer) {
   var resContainer = Root.querySelector('.PREresourceContainer');
   if (!resContainer) return;
   if (PREpeerResourceBarsTryUpdate(resContainer, Peer)) return;
   PREsetElementHTMLIfChanged(resContainer, PREpeerResourceHtml(Peer));
}

function PREpeerAccessBadgeUpdate(Root, Peer) {
   var accessBadge = Root.querySelector('.PREaccessBadge');
   if (!accessBadge) return;
   var access = (Peer.access_level || 'none').toLowerCase();
   var isFullAccess = (access === 'full');
   PREsetElementClassIfChanged(accessBadge, "PREaccessBadge " + (isFullAccess ? "PREbadgeGreen" : "PREbadgeGrey"));
   PREsetElementTextIfChanged(accessBadge, isFullAccess ? "Allowed" : "Blocked");
   PREsetElementAttrIfChanged(accessBadge, 'href', PREhrefAccess(Peer.id || ""));
}

function PREpeerSummaryUpdate(Root, Peer) {
   var summaryEl = Root.querySelector('.PREpeerSummaryValue');
   if (!summaryEl) return;
   var summary = PREpeerModalSummaryState(Peer);
   PREsetElementClassIfChanged(summaryEl, "PREpeerSummaryValue " + summary.className);
   PREsetElementTextIfChanged(summaryEl, summary.text);
   PREsetElementAttrIfChanged(summaryEl, "title", summary.text);
}

function PREpeerExternalUrl(Peer) {
   var protocol = Peer.https ? "https://" : "http://";
   return protocol + String(Peer.host || "") + ":" + String(Peer.port || "");
}

function PREpeerConnectionFieldsUpdate(Root, Peer) {
   var host = String(Peer.host || "");
   var port = String(Peer.port || "");
   var id = String(Peer.id || "");
   var fingerprint = String(Peer.fingerprint || "");
   var externalUrl = PREpeerExternalUrl(Peer);

   PREsetElementValueIfChanged(Root.querySelector('.PREvalHost'), host);
   PREsetElementValueIfChanged(Root.querySelector('.PREvalPort'), port);
   PREsetElementValueIfChanged(Root.querySelector('.PREvalID'), id);
   PREsetElementValueIfChanged(Root.querySelector('.PREvalFP'), fingerprint);

   var modalLinkBtn = Root.querySelector('.PREmodalLinkBtn');
   if (!modalLinkBtn) return;
   PREsetElementAttrIfChanged(modalLinkBtn, 'href', externalUrl);
   PREsetElementAttrIfChanged(modalLinkBtn, 'title', "Open " + externalUrl);
}

function PREpeerToggleConnected(ToggleBtn) {
   PREsetElementTextIfChanged(ToggleBtn, "Disconnect");
   PREsetElementClassIfChanged(ToggleBtn, "BUTTONstandard BUTTONdefault PREpeerToggleBtn PREpeerToggleConnected");
   ToggleBtn.preConnectingUntil = 0;
}

function PREpeerToggleConnecting(ToggleBtn) {
   PREsetElementTextIfChanged(ToggleBtn, "Connecting...");
   PREsetElementClassIfChanged(
      ToggleBtn,
      "BUTTONstandard BUTTONdefault PREpeerToggleBtn PREpeerToggleConnecting PREbtnIsConnecting"
   );
}

function PREpeerToggleDisconnected(ToggleBtn) {
   PREsetElementTextIfChanged(ToggleBtn, "Connect");
   PREsetElementClassIfChanged(ToggleBtn, "BUTTONstandard BUTTONaction PREpeerToggleBtn PREpeerToggleDisconnected");
   ToggleBtn.preConnectingUntil = 0;
}

function PREpeerToggleUpdate(Root, IsConnected) {
   var toggleBtn = Root.querySelector(".PREpeerToggleBtn");
   if (!toggleBtn) return;
   var connectingUntil = Number(toggleBtn.preConnectingUntil || 0);
   var isConnecting = !IsConnected && (connectingUntil > Date.now());
   if (IsConnected) return PREpeerToggleConnected(toggleBtn);
   if (isConnecting) return PREpeerToggleConnecting(toggleBtn);
   PREpeerToggleDisconnected(toggleBtn);
}

function PREpeerUpdate(Root, Peer) {
   var isConnected = PREpeerStatusConnected(Peer);
   PREpeerModalTitleUpdate(Root, Peer);
   PREpeerStateRowUpdate(Root, isConnected);
   PREpeerLatencyUpdate(Root, Peer, isConnected);
   PREpeerResourcesUpdate(Root, Peer);
   PREpeerAccessBadgeUpdate(Root, Peer);
   PREpeerSummaryUpdate(Root, Peer);
   PREpeerConnectionFieldsUpdate(Root, Peer);
   PREpeerToggleUpdate(Root, isConnected);
}

function PREpeerCurrent(Peer) {
   if (!Peer || !Peer.id) return Peer;
   return PREpeerGet(Peer.id) || Peer;
}

function PREpeerModalClose(Peer) {
   var id = String((Peer && Peer.id) || "");
   if (id) delete PRE_PV.latencyById[id];
   location.hash = PRErouteList();
}

function PREpeerModalBindClose(Root) {
   var Close = Root.querySelector(".PREpeerCloseBtn");
   if (Close) Close.onclick = function() { location.hash = PRErouteList(); };
}

function PREpeerModalBindRemove(Root, Peer) {
   var Remove = Root.querySelector(".PREpeerRemoveBtn");
   if (!Remove) return;
   Remove.onclick = function() {
      PREpeerActionRemove(PREpeerCurrent(Peer));
   };
}

function PREpeerModalBindToggle(Root, Peer) {
   var Toggle = Root.querySelector(".PREpeerToggleBtn");
   if (!Toggle) return;
   Toggle.onclick = function() {
      var currentPeer = PREpeerCurrent(Peer);
      var isConnected = Toggle.classList.contains('PREpeerToggleConnected');
      if (isConnected) PREpeerActionDisconnect(currentPeer);
      else PREpeerActionConnect(currentPeer, Toggle);
   };
}

function PREpeerModalBindCopy(Root, Peer) {
   var CopyBtn = Root.querySelector(".PREbtnCopyCode");
   if (!CopyBtn) return;
   CopyBtn.onclick = function() {
      PREpeerCopyInvite(Root, PREpeerCurrent(Peer), CopyBtn);
   };
}

function PREpeerModalBindActions(Root, Peer) {
   PREpeerModalBindClose(Root);
   PREpeerModalBindRemove(Root, Peer);
   PREpeerModalBindToggle(Root, Peer);
   PREpeerModalBindCopy(Root, Peer);
}

function PREshowPeerModal(Peer) {
   var modalTitle = String(Peer.name || Peer.id || "Peer Details");
   POPpopoutOpen({
      title: modalTitle,
      content: PREpeerModalHtml(Peer),
      className: "PREpopout",
      width: "600px",
      esc_close: true,
      callback: function() { PREpeerModalClose(Peer); }
   });

   var Root = document.querySelector(".PREpeerModal");
   if (!Root) return;
   PREpeerModalBindActions(Root, Peer);
}

/* --- HTML Generators --- */

function PREpeerModalConnectionState(Peer) {
   var isConnected = (Peer.connected === true || Peer.connected === "true");
   return {
      isConnected: isConnected,
      statusText: isConnected ? "Connected" : "Disconnected",
      statusDotClass: isConnected ? "PREstateDotConnected" : "PREstateDotOffline",
      statusTextClass: isConnected ? "PREpeerStateTextConnected" : "PREpeerStateTextOffline"
   };
}

function PREpeerModalLatencyState(Peer, IsConnected) {
   var lat = parseFloat(Peer.latency_ms);
   var latTier = PREpeerGridLatencyTierImmediate(IsConnected, lat);
   return {
      latDisp: (IsConnected && Number.isFinite(lat)) ? (Math.round(lat) + " ms") : "—",
      latDotClass: PREpeerGridLatencyDotClassForTier(latTier),
      latClass: PREpeerGridLatencyTextClassForTier(latTier)
   };
}

function PREpeerModalAccessState(Peer) {
   var access = (Peer.access_level || "none").toLowerCase();
   var isFullAccess = (access === "full");
   return {
      badgeText: isFullAccess ? "Allowed" : "Blocked",
      badgeClass: isFullAccess ? "PREbadgeGreen" : "PREbadgeGrey",
      accessHref: PREhrefAccess(Peer.id)
   };
}

function PREpeerModalSummaryState(Peer) {
   var text = PREpeerGridSummaryText(Peer);
   if (!text) text = "No active issues.";
   return {
      text: text,
      className: PREpeerGridSummaryClass(Peer)
   };
}

function PREpeerModalConnectionSectionHtml(Connection, Latency, Access, Summary) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeader">Connection</div>
         <div class="PREcard">
            <div class="PREformRowGrid">
               <div class="PREformLabel">State</div>
               <div class="PREstatusRow PREstatusRowPlain">
                  <span class="PREstateDot PREpeerStateDot ${Connection.statusDotClass}"></span>
                  <span class="PREpeerStateText ${Connection.statusTextClass}">${Connection.statusText}</span>
               </div>
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Latency</div>
               <div class="PRElatencyValWrap">
                  <span class="PREstatusDot PRElatencyDot ${Latency.latDotClass}"></span>
                  <div class="PREmono PRElatencyVal${Latency.latClass ? (" " + Latency.latClass) : ""}">${Latency.latDisp}</div>
               </div>
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Access</div>
               <div>
                  <a href="${Access.accessHref}" class="PREaccessBadge ${Access.badgeClass}">${Access.badgeText}</a>
               </div>
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Status</div>
               <div class="PREpeerSummaryValue ${Summary.className}" title="${ESChtmlEscape(Summary.text)}">${ESChtmlEscape(Summary.text)}</div>
            </div>
         </div>
      </div>`;
}

function PREpeerModalHealthSectionHtml(Peer) {
   if (!PREcommandCenterMode()) return "";
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeader">Health</div>
         <div class="PREcard PREresourceContainer">
            ${PREpeerResourceHtml(Peer)}
         </div>
      </div>`;
}

function PREpeerModalConnectionDetailsSectionHtml(Peer, ExternalUrl) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeaderSplit">
            <span>Connection Details</span>
            <div class="PREbtnCopyCode">Copy Invite Code</div>
         </div>
         <div class="PREcard">
            <div class="PREformRowGrid">
               <div class="PREformLabel">Address</div>
               <div class="PREinputGroup">
                  <input type="text" class="FORMinput PREvalHost" readonly value="${ESChtmlEscape(Peer.host)}">
                  <a href="${ExternalUrl}" target="_blank" class="PREmodalLinkBtn" title="Open ${ExternalUrl}">
                     <svg viewBox="0 0 24 24"><path fill="currentColor" d="M19 19H5V5h7V3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2v-7h-2v7zM14 3v2h3.59l-9.83 9.83 1.41 1.41L19 6.41V10h2V3h-7z"/></svg>
                  </a>
               </div>
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Port</div>
               <input type="text" class="FORMinput PREvalPort" readonly value="${ESChtmlEscape(Peer.port)}">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">ID</div>
               <input type="text" class="FORMinput PREmono PREvalID" readonly value="${ESChtmlEscape(Peer.id)}">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Fingerprint</div>
               <input type="text" class="FORMinput PREmono PREvalFP" readonly value="${ESChtmlEscape(Peer.fingerprint)}">
            </div>
         </div>
      </div>`;
}

function PREpeerModalActionsHtml(IsConnected) {
   return /*html*/`
      <div class="PREformActionsSplit">
         <div class="BUTTONstandard BUTTONdefault PREpeerRemoveBtn PREpeerDeleteBtn">Delete Peer</div>
         <div class="BUTTONgroup">
            <div class="BUTTONstandard ${IsConnected ? "BUTTONdefault PREpeerToggleConnected" : "BUTTONaction PREpeerToggleDisconnected"} PREpeerToggleBtn">
               ${IsConnected ? "Disconnect" : "Connect"}
            </div>
            <div class="BUTTONstandard BUTTONdefault PREpeerCloseBtn">Close</div>
         </div>
      </div>`;
}

function PREpeerModalHtml(Peer) {
   var connection = PREpeerModalConnectionState(Peer);
   var latency = PREpeerModalLatencyState(Peer, connection.isConnected);
   var access = PREpeerModalAccessState(Peer);
   var summary = PREpeerModalSummaryState(Peer);
   var externalUrl = PREpeerExternalUrl(Peer);

   return /*html*/`
   <div class="PREaddPeerModal PREpeerModal">
      ${PREpeerModalConnectionSectionHtml(connection, latency, access, summary)}
      ${PREpeerModalHealthSectionHtml(Peer)}
      ${PREpeerModalConnectionDetailsSectionHtml(Peer, externalUrl)}
      ${PREpeerModalActionsHtml(connection.isConnected)}
   </div>`;
}

function PREpeerResourceHtml(Peer) {
   // A. Disconnected / Error (must win over stale 403 values).
   var isConnected = (Peer.connected === true || Peer.connected === "true");
   if (!isConnected) {
      if (Peer.error_msg) {
         var errMsg = ESChtmlEscape(String(Peer.error_msg));
         return /*html*/`
         <div class="PREmodalMessage PREtextRed">
            <span>${errMsg}</span>
         </div>`;
      }
      return /*html*/`<div class="PREmodalMessage">Disconnected.</div>`;
   }

   // B. Access Denied (403) while connected.
   if (Peer.error_code == 403) {
      var msg = ESChtmlEscape(String(Peer.error_msg || 'Access Denied'));
      return /*html*/`
      <div class="PREmodalMessage">
         <svg class="PREmodalResourceIcon" viewBox="0 0 24 24" fill="currentColor">
            <path d="M12 17c1.1 0 2-.9 2-2s-.9-2-2-2-2 .9-2 2 .9 2 2 2zm6-9h-1V6c0-2.76-2.24-5-5-5S7 3.24 7 6v2H6c-1.1 0-2 .9-2 2v10c0 1.1.9 2 2 2h12c1.1 0 2-.9 2-2V10c0-1.1-.9-2-2-2zM8.9 6c0-1.71 1.39-3.1 3.1-3.1 1.71 0 3.1 1.39 3.1 3.1v2H8.9V6z"/>
         </svg>
         <span>No Read Access.</span>
      </div>`;
   }

   // C. Show Bars
   return /*html*/`
      <div class="PREmodalResourceBars">
         ${PREmodalBar("CPU", Peer.cpu_percent)}
         ${PREmodalBar("MEM", PREresourceMemPercent(Peer))}
         ${PREmodalBar("DSK", PREpeerGridDiskPct(Peer))}
      </div>
   `;
}

function PREmodalBar(Label, Val) {
   var pct = PREpercentClamp(Val);
   
   return /*html*/`
   <div class="PREmodalBarRow">
      <div class="PREmodalBarLabel">${Label}</div>
      <div class="PREmodalBarTrack">
         <div class="PREmodalBarFill PREmodalBarFillActive" style="width:${pct}%"></div>
      </div>
      <div class="PREmodalBarValue">${pct}%</div>
   </div>`;
}

function PREpeerCopyInvite(Root, Peer, Btn) {
   if (!Peer) return;
   var code = INVinviteCodeTryEncode({
      host: Peer.host,
      port: Peer.port,
      id: Peer.id,
      fingerprint: Peer.fingerprint
   });
   
   if (!code) {
      var oldText = Btn.textContent;
      Btn.textContent = "Error";
      setTimeout(function() { Btn.textContent = oldText; }, 2000);
      return;
   }

   PREcopyToClipboard(code);
   PREflashCopied(Btn);
}
