/* ==========================================================================
   PRE Peer Grid Data
   - Shared model payload readers for command and authorized-peer contexts
========================================================================== */

function PREpeerGridListDataArgs() {
   return {};
}

function PREpeerGridPollDataReady(Name) {
   return !!(MODELdata && Object.prototype.hasOwnProperty.call(MODELdata, Name));
}

function PREpeerGridListDataReady() {
   return PREpeerGridPollDataReady('PRElistData');
}

function PREpeerGridLocalDataReady() {
   if (!PREcommandCenterMode()) return true;
   return PREpeerGridPollDataReady('PRElocalData');
}

function PREpeerGridPeerList() {
   return MODELdata?.PRElistData?.data || [];
}

function PREpeerGridStatusGet() {
   return MODELdata?.PREstatusData?.data || {};
}

function PREpeerGridResourcesGet() {
   return MODELdata?.PREresourcesData?.data || {};
}

function PREpeerGridLocalGet() {
   return MODELdata?.PRElocalData?.data || {};
}

function PREpeerGridLocalResourcesGet() {
   return MODELdata?.PRElocalResourcesData?.data || {};
}

function PREpeerGridNodeStatus(StatusMap, Id, IsLocal) {
   var id = String(Id || "");
   var map = StatusMap || {};
   if (id && map[id]) return map[id];
   if (IsLocal) return { connected: true, latency_ms: 0 };
   return { connected: false };
}

function PREpeerGridLicenseDataGet() {
   return MODELdata?.PRElicenseData?.data || {};
}

function PREpeerGridLicenseDataReady() {
   return PREpeerGridPollDataReady('PRElicenseData');
}

function PREselfGet() {
   return MODELdata?.PREselfData?.data || null;
}

function PREselfResourcesGet() {
   return MODELdata?.PREselfResources?.data || MODELdata?.PRElocalResourcesData?.data || {};
}

function PREaccessListGet() {
   return MODELdata?.PREaccessList?.data || [];
}

function PREpeerModalListGet() {
   return MODELdata?.PREpeerList?.data || [];
}

function PREpeerModalStatusGet() {
   return MODELdata?.PREpeerStatus?.data || {};
}

function PREpeerModalResourcesGet() {
   return MODELdata?.PREpeerResources?.data || {};
}

function PREnodeConnected(Obj) {
   return (Obj?.connected === true || Obj?.connected === "true");
}

function PREnodeMatchesSearch(Obj, QueryKey) {
   var key = String(QueryKey || "").trim().toLowerCase();
   if (!key) return true;
   var terms = key.split(/\s+/);
   var access = String(Obj?.access_level || "").toLowerCase();
   var accessText = (access === "full") ? "allowed full" : "blocked none";
   var connectedText = (Obj?.connected === true || Obj?.connected === "true") ? "connected" : "disconnected";
   var host = String(Obj?.host || "");
   var port = String(Obj?.port || "");
   var hostPort = host && port ? (host + ":" + port) : (host || port);
   var haystack = [
      Obj?.name,
      Obj?.id,
      host,
      port,
      hostPort,
      accessText,
      connectedText
   ].join(" ").toLowerCase();

   for (var i = 0; i < terms.length; i++) {
      var term = String(terms[i] || "").trim();
      if (!term) continue;
      if (haystack.indexOf(term) === -1) return false;
   }
   return true;
}

function PREpeerGridDisplayPeerMeta(Peer, Scope) {
   var out = { ...Peer };
   var scope = (Scope === "local") ? "local" : "peer";
   var id = String(out.id || "");
   out.__gridScope = scope;
   out.__gridRowKey = scope + ":" + id;
   return out;
}

function PREcommandGridDisplayPeerList() {
   var peerList = PREpeerGridPeerList() || [];
   var local = PREpeerGridLocalGet() || null;
   var localId = String(local?.id || "");
   var searchKey = PREnodeSearchKey();
   var statusMap = PREpeerGridStatusGet();
   var resMap = PREpeerGridResourcesGet();
   var localRes = PREpeerGridLocalResourcesGet();
   var peers = peerList.slice();

   var out = [];
   if (localId) {
      var localNodeRes = (resMap && resMap[localId]) || {};
      if ((!localNodeRes || Object.keys(localNodeRes).length === 0) && localRes) localNodeRes = localRes;
      var localStatus = PREpeerGridNodeStatus(statusMap, localId, true);
      var localCombined = { ...local, ...localStatus, ...localNodeRes };
      if (PREnodeMatchesSearch(localCombined, searchKey)) {
         out.push(PREpeerGridDisplayPeerMeta(local, "local"));
      }
   }

   for (var j = 0; j < peers.length; j++) {
      var peer = peers[j];
      var peerId = String(peer?.id || "");
      if (!peerId) continue;
      if (localId && peerId === localId) continue;
      var peerNodeRes = (resMap && resMap[peerId]) || {};
      var peerStatus = PREpeerGridNodeStatus(statusMap, peerId, false);
      var combined = { ...peer, ...peerStatus, ...peerNodeRes };
      if (!PREnodeMatchesSearch(combined, searchKey)) continue;
      out.push(PREpeerGridDisplayPeerMeta(peer, "peer"));
   }
   return out;
}

function PREauthorizedPeersDisplayPeerList() {
   var peerList = PREpeerGridPeerList() || [];
   var out = [];
   for (var i = 0; i < peerList.length; i++) {
      var peer = peerList[i];
      if (!peer || !peer.id) continue;
      out.push(PREpeerGridDisplayPeerMeta(peer, "peer"));
   }
   return out;
}

function PREpeerGridDisplayPeerList() {
   return PREcommandCenterMode() ? PREcommandGridDisplayPeerList() : PREauthorizedPeersDisplayPeerList();
}
