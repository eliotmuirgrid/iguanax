/* ==========================================================================
   PRE Listeners
   - Shared peer-grid event delegation for Command Center and Authorized Peers
   - Routes clicks to Actions or URL changes
========================================================================== */

function PREpeerGridListenersInit() {
   var Container = document.querySelector('.PREscreenContainer, .PREscreenContainerAuthorized');
   if (!Container) return;
   Container.removeEventListener('click', PREpeerGridClick);
   Container.addEventListener('click', PREpeerGridClick);
   Container.removeEventListener('dblclick', PREpeerGridDblClick);
   Container.addEventListener('dblclick', PREpeerGridDblClick);
   window.removeEventListener('hashchange', PREpeerGridHashChangeSyncView);
   window.addEventListener('hashchange', PREpeerGridHashChangeSyncView);
}

function PREpeerGridSyncCommandNavHref() {
   if (!PREcommandCenterMode()) return;
   var Link = document.querySelector('.CORcommandIcon');
   if (!Link) return;
   PREsetElementAttrIfChanged(Link, 'href', PREformatCommandUrl());
}

function PREpeerGridHashChangeSyncView() {
   PREsaveViewArgs();
   PREpeerGridSyncCommandNavHref();
   if (document.querySelector('.PREgridTarget')) {
      PREpeerGridRenderViews();
   }
}

function PREpeerGridCommitNodeSearch(Value) {
   var args = PAGEhashParameters();
   var next = String(Value || "").trim();
   delete args.peer;
   delete args.sort;
   delete args.order;
   if (next) args.q = next;
   else delete args.q;
   var hash = "#" + PRErouteListPath() + PAGEformatParams(args);
   if (window.location.hash === hash) return;
   window.location.hash = hash;
}

function PREpeerGridPeerScope(Peer) {
   return (Peer && Peer.__gridScope === "local") ? "local" : "peer";
}

function PREpeerGridDetailsRouteForPeer(Peer) {
   if (PREpeerGridPeerScope(Peer) === "local") return PRErouteDetails();
   return PRErouteDetails(Peer.id);
}

function PREpeerGridFindDisplayPeer(RowKey) {
   var key = String(RowKey || "");
   if (!key) return null;
   var List = PREpeerGridDisplayPeerList() || [];
   for (var i = 0; i < List.length; i++) {
      if (PREpeerGridRowKey(List[i]) === key) return List[i];
   }
   return null;
}

function PREpeerGridPeerFromRow(Wrapper) {
   if (!Wrapper) return null;
   return PREpeerGridFindDisplayPeer(Wrapper.getAttribute('data-grid-row-key'));
}

function PREpeerGridClick(e) {
   if (e.target.closest('.PREbtnClearNodeSearch')) return PREpeerGridCommitNodeSearch('');
   if (e.target.closest('.PREbtnAdd')) return location.hash = PRErouteAdd();
   if (e.target.closest('.PREbtnMyDetails')) return location.hash = PRErouteConnect();
   var Wrapper = e.target.closest('.PREpeerWrapper');
   if (!Wrapper) return;
   var Peer = PREpeerGridPeerFromRow(Wrapper);
   if (!Peer) return;
   var isLocalRow = PREpeerGridPeerScope(Peer) === "local";
   if (e.target.closest('a')) return;
   if (e.target.closest('.PREaccessBadge')) return;
   if (e.target.closest('.PREpeerHostLink')) return;
   var RetryBtn = e.target.closest('.PREbtnRetry');
   if (RetryBtn) {
      if (isLocalRow) return;
      if (RetryBtn.classList.contains('PREbtnRetryDisconnect')) return PREpeerActionDisconnect(Peer);
      return PREpeerActionRetry(Peer, RetryBtn);
   }
   if (e.target.closest('.PREbtnCopyCode')) return;
   // Plain row clicks should not open details; double-click handles row-open behavior.
   return;
}

function PREpeerGridDblClick(e) {
   if (!PREcommandCenterMode()) return;
   var Wrapper = e.target.closest('.PREpeerWrapper');
   if (!Wrapper) return;
   if (e.target.closest('a') || e.target.closest('.BUTTONstandard')) return;
   var Peer = PREpeerGridPeerFromRow(Wrapper);
   if (Peer) location.hash = PREpeerGridDetailsRouteForPeer(Peer);
}
