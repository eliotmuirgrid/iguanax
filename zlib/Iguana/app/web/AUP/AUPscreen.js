/* ==========================================================================
   AUP Authorized Peers Surface
   - Authorized peer screen entry points
   - Reuses shared PRE peer-grid primitives
========================================================================== */

function AUPscreen(ForcePoll) {
   PAGEremovePopouts();
   var forcePoll = (ForcePoll === true);
   var hasSettingsShell = !!document.querySelector('.SETmainContent') && !!document.querySelector('.SETcontentBody');
   if (!hasSettingsShell) {
      SETrender();
   } else {
      CORsetNavIcon('CORsettingsIcon');
      SETclearSelected();
      CORauthorizedPeersBadgeUpdate();
   }
   AUPmodelViewsInit();
   PREpeerGridModelInit(forcePoll, { mode: "aup" });
   AUPrender();
}

function AUPrender() {
   var Nav = document.querySelector('.AUPauthorizedPeersNav');
   if (Nav) Nav.classList.add('selected');
   var Root = document.querySelector('.SETcontentBody');
   if (!Root) return;
   var canManage = PREauthorizedPeersRouteAllowed();
   var expectedPaneMode = PREauthorizedPeersPaneMode(canManage);
   var expectedPaneClass = PREsettingsPaneModeClass(expectedPaneMode);
   var Pane = Root.querySelector('.AUPauthorizedPeersPane');
   var paneMatchesMode = !!Pane && Pane.classList.contains(expectedPaneClass);
   if (!Pane || !paneMatchesMode) {
      Root.innerHTML = PREauthorizedPeersPaneHtml(canManage);
   }
   var HeaderShareBtn = Root.querySelector('.AUPauthorizedPeersShareBtn');
   if (HeaderShareBtn) HeaderShareBtn.onclick = PREselfOpenShareModal;
   var HeaderAddBtn = Root.querySelector('.AUPauthorizedPeersEditBtn');
   if (HeaderAddBtn) HeaderAddBtn.onclick = function() { location.hash = PRErouteAdd(); };
   if (!canManage) return;
   PREauthorizedPeersSyncAndRender();
}

function AUPmodelViewsInit() {
   MODELviewAdd('PREmodel', 'AUPfeatureGateUpdate');
}

function AUPaccessScreen() {
   AUPscreen(true);
   if (!PREauthorizedPeersRouteAllowed()) return;
   PREaccessScreen();
}

function AUPaddPeerScreen() {
   AUPscreen(true);
   if (!PREauthorizedPeersRouteAllowed()) return;
   PREaddPeerScreen();
}

function AUPdetailsScreen() {
   AUPscreen(true);
   if (!PREauthorizedPeersRouteAllowed()) return;
   PREpeerScreen();
}

function AUPconnectScreen() {
   AUPscreen(true);
   if (!PREauthorizedPeersRouteAllowed()) return;
   PREselfConnectScreen();
}

var AUP_FEATURE_GATE_SIG = "";

function AUPfeatureGateUpdate() {
   var page = String(PAGEhash() || "");
   if (!PREisAuthorizedPeersRoute(page)) {
      AUP_FEATURE_GATE_SIG = "";
      return;
   }

   var sig = page + "|" + String(PRElicenseDashboardState());
   if (AUP_FEATURE_GATE_SIG === sig) return;
   AUP_FEATURE_GATE_SIG = sig;

   if (!document.querySelector('.SETcontentBody')) return;
   AUPrender();
   if (!PREauthorizedPeersRouteAllowed()) return;
   if (page === PRErouteAuthorizedPeersAccessPath()) return PREaccessScreen();
   if (page === PRErouteAuthorizedPeersAddPath()) return PREaddPeerScreen();
   if (page === PRErouteAuthorizedPeersDetailsPath()) return PREpeerScreen();
   if (page === PRErouteAuthorizedPeersConnectPath()) return PREselfConnectScreen();
}
