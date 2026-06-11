/* ==========================================================================
   PRE Peer Grid
   - Shared peer-grid surface helpers used by AUP and Command Center
========================================================================== */

function PREcommandGridSyncAndRender() {
   PREpeerGridListenersInit();
   PREpeerGridSyncCommandNavHref();
   PREcommandGridRenderViews();
}

function PREauthorizedPeersSyncAndRender() {
   PREpeerGridListenersInit();
   PREauthorizedPeersRenderViews();
}

function PREcommandGridRenderViews() {
   PREcommandGridUpdateList();
   PREpeerGridUpdateStatus();
   PREcommandGridUpdateResources();
}

function PREauthorizedPeersRenderViews() {
   PREauthorizedPeersUpdateList();
   PREpeerGridUpdateStatus();
}

function PREpeerGridRenderViews() {
   if (PREcommandCenterMode()) return PREcommandGridRenderViews();
   return PREauthorizedPeersRenderViews();
}
