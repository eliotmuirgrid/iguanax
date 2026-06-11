/* ==========================================================================
   PRE Peer Grid Model
   - Shared peer-grid polling model used by Command Center and Authorized Peers
========================================================================== */

function PREcommandGridModelViewsInit() {
   MODELviewAdd('PREmodel', 'PREcommandGridUpdateList');
   MODELviewAdd('PREmodel', 'PREpeerGridUpdateStatus');
   MODELviewAdd('PREmodel', 'PREcommandGridUpdateResources');
}

function PREauthorizedPeersModelViewsInit() {
   MODELviewAdd('PREmodel', 'PREauthorizedPeersUpdateList');
   MODELviewAdd('PREmodel', 'PREpeerGridUpdateStatus');
}

function PREpeerGridModelInit(ForcePoll, Config) {
   var forcePoll = (ForcePoll === true);
   var cfg = Config || {};
   var mode = String(cfg.mode || "");
   MODELpollerAdd('PREmodel', 'PRElistData', 3, 'iguanas/list', PREpeerGridListDataArgs);
   MODELpollerAdd('PREmodel', 'PREstatusData', 3, 'iguanas/status', {});
   MODELpollerAdd('PREmodel', 'PRElicenseData', 3, 'license/get', {});
   if (mode === "aup") {
      PREauthorizedPeersModelViewsInit();
   } else {
      MODELpollerAdd('PREmodel', 'PREresourcesData', 3, 'iguanas/resources', {});
      MODELpollerAdd('PREmodel', 'PRElocalData', 3, 'iguanas/me', {});
      MODELpollerAdd('PREmodel', 'PRElocalResourcesData', 3, 'resources/current', {});
      PREcommandGridModelViewsInit();
   }
   if (forcePoll) MODELforcePoll('PREmodel');
   MODELstart('PREmodel');
}
