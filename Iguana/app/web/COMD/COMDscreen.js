/* ==========================================================================
   COMD Command Center
   - Command-center route orchestration and top-level rendering
========================================================================== */

var COMD_FEATURE_GATE_SIG = "";

function COMDmodelViewsInit() {
   MODELviewAdd('PREmodel', 'COMDfeatureGateUpdate');
}

function COMDcommandUnavailableScreen() {
   PAGEremovePopouts();
   if (!document.querySelector('.PREscreenContainer, .PREscreenContainerAuthorized')) {
      PAGEresetPage();
      CORpreparePage();
      CORheader();
   }
   CORsetNavIcon("CORcommandIcon");
   COMDmodelViewsInit();
   PREpeerGridModelInit(true, { mode: "comd" });
   COMDrender();
}

function COMDscreen() {
   PAGEremovePopouts();
   if (!PREcommandRouteAllowed()) return COMDcommandUnavailableScreen();
   if (document.querySelector('.PREscreenContainer') && document.querySelector('.PREmainHeader')) {
      COMDmodelViewsInit();
      PREpeerGridModelInit(false, { mode: "comd" });
      return;
   }
   PAGEresetPage();
   CORpreparePage();
   CORheader();
   CORsetNavIcon("CORcommandIcon");
   COMDmodelViewsInit();
   PREpeerGridModelInit(true, { mode: "comd" });
   COMDrender();
}

function COMDaccessScreen() {
   COMDscreen();
   if (!PREcommandRouteAllowed()) return;
   PREaccessScreen();
}

function COMDaddPeerScreen() {
   COMDscreen();
   if (!PREcommandRouteAllowed()) return;
   PREaddPeerScreen();
}

function COMDdetailsScreen() {
   COMDscreen();
   if (!PREcommandRouteAllowed()) return;
   if (PREpeerParam()) return PREpeerScreen();
   return PREselfDetailsScreen();
}

function COMDconnectScreen() {
   COMDscreen();
   if (!PREcommandRouteAllowed()) return;
   PREselfConnectScreen();
}

function COMDrender() {
   var Root = document.querySelector('.CORmain');
   if (!Root) return;
   var routeAllowed = PREcommandRouteAllowed();
   var hasCommandShell = !!Root.querySelector('.PREscreenContainer') &&
      !Root.querySelector('.PREfeatureNoticeScreen');
   var hasUnavailableShell = !!Root.querySelector('.PREfeatureNoticeScreen');
   if ((routeAllowed && !hasCommandShell) || (!routeAllowed && !hasUnavailableShell)) {
      Root.innerHTML = routeAllowed ? PREcommandScreenHtml() : PREcommandUnavailableScreenHtml();
   }
   if (routeAllowed) PREcommandGridSyncAndRender();
}

function COMDfeatureGateSig(Page) {
   return String(Page || "") + "|" + String(PRElicenseDashboardState());
}

function COMDfeatureGateUpdate() {
   var page = String(PAGEhash() || "");
   if (!PREisCommandRoute(page)) {
      COMD_FEATURE_GATE_SIG = "";
      return;
   }
   var sig = COMDfeatureGateSig(page);
   if (COMD_FEATURE_GATE_SIG === sig) return;
   COMD_FEATURE_GATE_SIG = sig;

   if (!document.querySelector('.CORmain')) return;
   COMDrender();
}
