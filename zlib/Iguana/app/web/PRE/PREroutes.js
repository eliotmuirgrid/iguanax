/* ==========================================================================
   PRE Routes
   - Shared route path, route classification, and peer-grid view params
========================================================================== */

var PREpreviousView = {};

function PRErouteCommandMainPath()             { return "command"; }
function PRErouteAuthorizedPeersMainPath()     { return "settings/authorized_peers"; }
function PRErouteAuthorizedPeersAddPath()      { return "settings/authorized_peers/add"; }
function PRErouteAuthorizedPeersAccessPath()   { return "settings/authorized_peers/access"; }
function PRErouteAuthorizedPeersDetailsPath()  { return "settings/authorized_peers/details"; }
function PRErouteAuthorizedPeersConnectPath()  { return "settings/authorized_peers/connect"; }
function PRErouteCommandAddPath()              { return "command/add"; }
function PRErouteCommandAccessPath()           { return "command/access"; }
function PRErouteCommandDetailsPath()          { return "command/details"; }
function PRErouteCommandConnectPath()          { return "command/connect"; }
function PRErouteConnectPath(Page)             { return PREisAuthorizedPeersRoute(Page) ? PRErouteAuthorizedPeersConnectPath() : PRErouteCommandConnectPath(); }

function PREisAuthorizedPeersRoute(Page) {
   var page = String(Page || PAGEhash() || "");
   return (
      page === PRErouteAuthorizedPeersMainPath() ||
      page === PRErouteAuthorizedPeersAddPath() ||
      page === PRErouteAuthorizedPeersAccessPath() ||
      page === PRErouteAuthorizedPeersDetailsPath() ||
      page === PRErouteAuthorizedPeersConnectPath()
   );
}

function PREisCommandRoute(Page) {
   var page = String(Page || PAGEhash() || "");
   return (
      page === PRErouteCommandMainPath() ||
      page === PRErouteCommandAddPath() ||
      page === PRErouteCommandDetailsPath() ||
      page === PRErouteCommandConnectPath() ||
      page === PRErouteCommandAccessPath()
   );
}

function PREcommandCenterMode(Page) { return !PREisAuthorizedPeersRoute(Page);}

function PRErouteListPath(Page) {
   return PREisAuthorizedPeersRoute(Page) ? PRErouteAuthorizedPeersMainPath() : PRErouteCommandMainPath();
}

function PRErouteAddPath(Page) {
   return PREisAuthorizedPeersRoute(Page) ? PRErouteAuthorizedPeersAddPath() : PRErouteCommandAddPath();
}

function PRErouteDetailsPath(Page) {
   return PREisAuthorizedPeersRoute(Page) ? PRErouteAuthorizedPeersDetailsPath() : PRErouteCommandDetailsPath();
}

function PRErouteAccessPath(Page) {
   return PREisAuthorizedPeersRoute(Page) ? PRErouteAuthorizedPeersAccessPath() : PRErouteCommandAccessPath();
}

function PREnodeSearchValue(Args) {
   if (!PREcommandCenterMode()) return "";
   var raw = "";
   if (Args && Args.q !== undefined) raw = Args.q;
   else raw = PAGEhashParameter("q");
   return String(raw || "").trim();
}

function PREnodeSearchKey(Args) {
   return PREnodeSearchValue(Args).toLowerCase();
}

function PREviewParams() {
   var params = {};
   var search = PREnodeSearchValue();
   if (search) params.q = search;
   return params;
}

function PREsaveViewArgs() {
   if (!PREisCommandRoute()) return;
   PREpreviousView = {};
   var search = PREnodeSearchValue();
   if (search) PREpreviousView.q = search;
}

function PREformatCommandUrl() {
   return "#command" + PAGEformatParams(PREpreviousView);
}

function PRErouteList() {
   return PRErouteListPath() + PAGEformatParams(PREviewParams());
}

function PREpeerParam() {
   var peerId = PAGEhashParameter("peer");
   if (peerId == null) return "";
   peerId = String(peerId).trim().toUpperCase();
   if (!peerId) return "";
   if (!/^[A-Z0-9]{16}$/.test(peerId)) return "";
   return peerId;
}

function PRErouteWithPeer(Path, PeerId) {
   var params = PREviewParams();
   if (PeerId) params.peer = PeerId;
   var suffix = PAGEformatParams(params);
   return suffix ? (Path + suffix) : Path;
}

function PRErouteAdd()             { return PRErouteWithPeer(PRErouteAddPath(PAGEhash())); }
function PRErouteDetails(PeerId)   { return PRErouteWithPeer(PRErouteDetailsPath(PAGEhash()), PeerId); }
function PRErouteConnect()         { return PRErouteWithPeer(PRErouteConnectPath(PAGEhash())); }
function PRErouteAccess(PeerId)    { return PRErouteWithPeer(PRErouteAccessPath(PAGEhash()), PeerId); }
function PREhref(Path)             { return "#" + Path; }
function PREhrefDetails(PeerId)    { return PREhref(PRErouteDetails(PeerId)); }
function PREhrefAccess(PeerId)     { return PREhref(PRErouteAccess(PeerId)); }
