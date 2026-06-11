/* PRErows.js */
/* ==========================================================================
   PRE Row Views
   - "Pure" functions: Data In -> HTML Out
========================================================================== */

function PREpeerGridRowScope(Peer) {
   return (Peer && Peer.__gridScope === "local") ? "local" : "peer";
}

function PREpeerGridRowInactiveClass(Scope, Peer) {
   if (!PREcommandCenterMode()) return "";
   if (Scope === "local") return "";
   return PREnodeConnected(Peer) ? "" : "PREpeerWrapperInactive";
}

function PREpeerGridRowWrapperClass(Scope, Peer) {
   var base = (Scope === "local")
      ? "PREpeerWrapper PREpeerWrapperLocal"
      : "PREpeerWrapper PREpeerWrapperRemote";
   var inactive = PREpeerGridRowInactiveClass(Scope, Peer);
   return [base, inactive].filter(Boolean).join(" ");
}

function PREpeerGridRowDetailsHref(Scope, PeerId) {
   return (Scope === "local") ? PREhrefDetails() : PREhrefDetails(PeerId);
}

function PREpeerGridRowExternalUrl(Peer) {
   var protocol = Peer && Peer.https ? "https://" : "http://";
   return protocol + String(Peer.host || "") + ":" + String(Peer.port || "");
}

function PREpeerGridNameShowsDistinctId(Name, PeerId) {
   var name = String(Name || "").trim();
   var id = String(PeerId || "").trim();
   if (!id) return false;
   if (!name) return true;
   return name.toLowerCase() !== id.toLowerCase();
}

function PREpeerGridNameMatchesId(NameOrPeer, PeerId) {
   var name = "";
   var id = "";
   if (NameOrPeer && typeof NameOrPeer === "object") {
      name = String(NameOrPeer.name || "").trim().toLowerCase();
      id = String(NameOrPeer.id || "").trim().toLowerCase();
   } else {
      name = String(NameOrPeer || "").trim().toLowerCase();
      id = String(PeerId || "").trim().toLowerCase();
   }
   if (!name || !id) return false;
   return name === id;
}

function PREpeerGridIsUnregisteredNode(Peer) {
   if (!PREcommandCenterMode()) return false;
   if (!Peer || PREpeerGridRowScope(Peer) === "local") return false;
   if (!PREpeerGridNameMatchesId(Peer)) return false;
   return !PREnodeConnected(Peer);
}

function PREpeerGridRowNameColHtml(Peer, Scope, PeerId) {
   var displayName = String(Peer.name || PeerId || "Unknown");
   var name = ESChtmlEscape(displayName);
   var host = ESChtmlEscape(String(Peer.host || "")) + ":" + String(Peer.port || "");
   var idText = ESChtmlEscape(PeerId || "Unknown");
   var showId = PREpeerGridNameShowsDistinctId(displayName, PeerId);
   var idClass = showId ? "PREpeerIdText" : "PREpeerIdText PREpeerIdTextHidden";
   var externalUrl = PREpeerGridRowExternalUrl(Peer);
   var detailsHref = PREpeerGridRowDetailsHref(Scope, PeerId);
   var pinnedTag = (Scope === "local") ? `<span class="PREstickyLocalTag">This server</span>` : "";
   return /*html*/`
         <div class="PREcolPeer">
            <div class="PREpeerNameRow">
               <a href="${detailsHref}" class="PREpeerNameLink">${name}</a>
               ${pinnedTag}
            </div>
            <a href="${externalUrl}" target="_blank" class="PREpeerHostLink" title="Open ${externalUrl}">
               <span class="PREpeerHostText">${host}</span>
               <svg class="PRElinkIcon" viewBox="0 0 24 24"><path fill="currentColor" d="M19 19H5V5h7V3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2v-7h-2v7zM14 3v2h3.59l-9.83 9.83 1.41 1.41L19 6.41V10h2V3h-7z"/></svg>
            </a>
            <div class="${idClass}">ID: ${idText}</div>
         </div>`;
}

function PREpeerGridMetricValueClass(Text) {
   return String(Text || "") === "--" ? "PREmetricValue PREmetricValueEmpty" : "PREmetricValue";
}

function PREpeerGridMetricValueHtml(Text) {
   var text = String(Text || "--");
   return `<span class="${PREpeerGridMetricValueClass(text)}">${ESChtmlEscape(text)}</span>`;
}

function PREpeerGridRowMetricsHtml(Peer, ShowMetrics) {
   if (!ShowMetrics) return "";
   var errClass = PREpeerGridErrorMetricClass(Peer);
   var healthCols = PREpeerGridHealthCellsHtml(Peer);
   var errText = PREpeerGridErrorMetricText(Peer);
   var queueText = PREpeerGridQueueMetricText(Peer);
   var rateText = PREpeerGridRateMetricText(Peer);
   return /*html*/`
         <div class="PREcolQueue PREmetricCell">${PREpeerGridMetricValueHtml(queueText)}</div>
         <div class="PREcolErrors PREmetricCell${errClass ? (" " + errClass) : ""}">${PREpeerGridMetricValueHtml(errText)}</div>
         <div class="PREcolRate PREmetricCell">${PREpeerGridMetricValueHtml(rateText)}</div>
         ${healthCols}`;
}

function PREpeerGridActionPlaceholderHtml() {
   return `<span class="PREactionPlaceholder" title="No action available">--</span>`;
}

function PREpeerGridActionConfig(Peer) {
   var scope = PREpeerGridRowScope(Peer);
   if (scope === "local") return null;
   if (PREnodeConnected(Peer)) {
      return {
         label: "Disconnect",
         title: "Disconnect",
         buttonClass: "PREbtnRetry PREbtnRetryDanger PREbtnRetryDisconnect"
      };
   }
   return {
      label: "Connect",
      title: "Connect",
      buttonClass: "PREbtnRetry PREbtnRetryGhost PREbtnRetryConnect"
   };
}

function PREpeerGridConnectActionCellHtml(Peer) {
   var cfg = PREpeerGridActionConfig(Peer);
   if (!cfg) {
      return /*html*/`
      <div class="PREcolActions PREcolActionsRow">
         ${PREpeerGridActionPlaceholderHtml()}
      </div>`;
   }

   return /*html*/`
   <div class="PREcolActions PREcolActionsRow">
      <div class="${cfg.buttonClass}" title="${cfg.title}">
         ${cfg.label}
      </div>
   </div>`;
}

function PREcommandGridRowHtml(Peer){
   var peer = Peer || {};
   var scope = PREpeerGridRowScope(peer);
   var peerId = String(peer.id || "");
   var wrapperClass = PREpeerGridRowWrapperClass(scope, peer);
   var nameCol = PREpeerGridRowNameColHtml(peer, scope, peerId);
   var metricsHtml = PREpeerGridRowMetricsHtml(peer, true);
   var statusCol = PREpeerGridStatusCellHtml(peer);
   var latencyCol = `<div class="PREcolLatency">${PREpeerGridLatencyCellHtml(peer)}</div>`;
   var accessCol = `<div class="PREcolAccess">${PREpeerGridAccessCellHtml(peer)}</div>`;
   var actionCol = PREpeerGridConnectActionCellHtml(peer);
   var commandCols = latencyCol + accessCol + metricsHtml + actionCol;

   return /*html*/`
   <div class="${wrapperClass}">
      <div class="PREpeerRow">
         ${nameCol}

         <div class="PREcolLink">
            ${statusCol}
         </div>
         ${commandCols}
      </div>
   </div>`;
}

function PREauthorizedPeersRowHtml(Peer) {
   var peer = Peer || {};
   var scope = PREpeerGridRowScope(peer);
   var peerId = String(peer.id || "");
   var wrapperClass = PREpeerGridRowWrapperClass(scope, peer);
   var nameCol = PREpeerGridRowNameColHtml(peer, scope, peerId);
   var statusCol = PREpeerGridStatusCellHtml(peer);
   var latencyCol = `<div class="PREcolLatency">${PREpeerGridLatencyCellHtml(peer)}</div>`;
   var accessCol = `<div class="PREcolAccess">${PREpeerGridAccessCellHtml(peer)}</div>`;
   var actionCol = PREpeerGridConnectActionCellHtml(peer);

   return /*html*/`
   <div class="${wrapperClass}">
      <div class="PREpeerRow">
         ${nameCol}

         <div class="PREcolLink">
            ${statusCol}
         </div>
         ${latencyCol}${accessCol}${actionCol}
      </div>
   </div>`;
}

function PREpeerGridRowHtml(Peer){
   return PREcommandCenterMode() ? PREcommandGridRowHtml(Peer) : PREauthorizedPeersRowHtml(Peer);
}

function PREpeerGridMetricNumber(Obj, Keys) {
   if (!Obj || !Keys || !Keys.length) return null;
   for (var i = 0; i < Keys.length; i++) {
      var key = Keys[i];
      if (!(key in Obj)) continue;
      var num = parseFloat(Obj[key]);
      if (Number.isFinite(num)) return num;
   }
   return null;
}

function PREpeerGridMetricIntText(Obj, Keys) {
   var num = PREpeerGridMetricNumber(Obj, Keys);
   if (!Number.isFinite(num)) return "--";
   if (num < 0) num = 0;
   return String(Math.round(num));
}

function PREpeerGridHasLiveMetrics(Obj) {
   return PREpeerGridRowScope(Obj) === "local" || PREnodeConnected(Obj);
}

function PREpeerGridRateMetricText(Obj) {
   if (!PREpeerGridHasLiveMetrics(Obj)) return "--";
   var num = PREpeerGridMetricNumber(Obj, [
      "rate",
      "msg_per_sec",
      "msgs_per_sec",
      "messages_per_sec",
      "throughput",
      "throughput_rate",
      "throughput_msg_per_sec"
   ]);
   if (!Number.isFinite(num)) return "--";
   if (num < 0) num = 0;
   var rounded = (num >= 100 || Math.abs(num - Math.round(num)) < 0.05) ? Math.round(num) : Math.round(num * 10) / 10;
   return String(rounded);
}

function PREpeerGridErrorMetricText(Obj) {
   if (!PREpeerGridHasLiveMetrics(Obj)) return "--";
   return PREpeerGridMetricIntText(Obj, [
      "component_errors",
      "component_error_count",
      "error_components",
      "components_with_errors",
      "errors",
      "error_count"
   ]);
}

function PREpeerGridErrorMetricClass(Obj) {
   if (!PREpeerGridHasLiveMetrics(Obj)) return "";
   var num = PREpeerGridMetricNumber(Obj, [
      "component_errors",
      "component_error_count",
      "error_components",
      "components_with_errors",
      "errors",
      "error_count"
   ]);
   if (!Number.isFinite(num) || num <= 0) return "";
   return "PREmetricErr";
}

function PREpeerGridQueueMetricText(Obj) {
   if (!PREpeerGridHasLiveMetrics(Obj)) return "--";
   return PREpeerGridMetricIntText(Obj, [
      "queue",
      "max_queue",
      "max_queue_depth",
      "queue_max",
      "queue_depth_max",
      "max_depth"
   ]);
}

function PREpeerGridSummaryErrorCount(Obj) {
   var num = PREpeerGridMetricNumber(Obj, [
      "component_errors",
      "component_error_count",
      "error_components",
      "components_with_errors",
      "errors",
      "error_count"
   ]);
   if (!Number.isFinite(num) || num <= 0) return 0;
   return Math.max(0, Math.round(num));
}

function PREpeerGridSummaryQueue(Obj) {
   var num = PREpeerGridMetricNumber(Obj, [
      "queue",
      "max_queue",
      "max_queue_depth",
      "queue_max",
      "queue_depth_max",
      "max_depth"
   ]);
   if (!Number.isFinite(num)) return 0;
   return Math.max(0, Math.round(num));
}

function PREpeerGridSummaryCountText(Count, Singular, Plural) {
   var n = Math.max(0, Math.round(Count || 0));
   if (n === 1) return "1 " + Singular;
   return String(n) + " " + Plural;
}

function PREpeerGridSummaryConnectedParts(Obj) {
   var parts = [];

   var errCount = PREpeerGridSummaryErrorCount(Obj);
   if (errCount > 0) {
      parts.push(PREpeerGridSummaryCountText(errCount, "component error", "component errors"));
   }

   var queue = PREpeerGridSummaryQueue(Obj);
   if (queue > 0) parts.push("queue " + queue);

   return parts;
}

function PREpeerGridSummaryConnectedText(Obj) {
   var parts = PREpeerGridSummaryConnectedParts(Obj);
   if (parts.length === 0) return "Connected";
   return parts.join(", ");
}

function PREpeerGridSummaryShortError(Msg) {
   var msg = String(Msg || "").trim();
   if (!msg) return "";
   if (msg.length > 40) return msg.slice(0, 40) + "…";
   return msg;
}

function PREpeerGridSummaryDisconnectedText(Obj) {
   if (PREpeerGridIsUnregisteredNode(Obj)) return "Awaiting initial connection";
   var msg = PREpeerGridSummaryShortError(Obj?.error_msg);
   if (msg) return msg;
   return "Disconnected";
}

function PREpeerGridSummaryText(Obj) {
   if (PREnodeConnected(Obj)) return PREpeerGridSummaryConnectedText(Obj);
   return PREpeerGridSummaryDisconnectedText(Obj);
}

function PREpeerGridSummaryClass(Obj) {
   if (PREpeerGridIsUnregisteredNode(Obj)) return "PREsummaryMuted";
   if (!PREnodeConnected(Obj)) return "PREsummaryMuted";
   if (PREpeerGridSummaryErrorCount(Obj) > 0) return "PREsummaryErr";
   return "PREsummaryOk";
}

function PREpeerGridStateKey(CombinedPeer) {
   if (PREpeerGridIsUnregisteredNode(CombinedPeer)) return "never";
   if (PREnodeConnected(CombinedPeer)) return "connected";
   return "disconnected";
}

function PREpeerGridStateText(StateKey) {
   if (StateKey === "connected") return "Connected";
   return "Offline";
}

function PREpeerGridStateTitle(CombinedPeer) {
   if (PREpeerGridIsUnregisteredNode(CombinedPeer)) return "Awaiting initial connection";
   var stateKey = PREpeerGridStateKey(CombinedPeer);
   if (stateKey === "connected") return "Connected";
   if (stateKey === "never") return "This node has not connected yet.";
   var msg = PREpeerGridSummaryShortError(CombinedPeer?.error_msg);
   if (msg) return "Disconnected: " + msg;
   return "Disconnected";
}

function PREpeerGridStateTextClass(StateKey, CombinedPeer) {
   if (StateKey === "connected") return "PREstateTextConnected";
   if (PREpeerGridIsUnregisteredNode(CombinedPeer)) return "PREstateTextMuted";
   return "PREstateTextOffline";
}

function PREpeerGridStateDotClass(StateKey) {
   if (StateKey === "connected") return "PREstateDotConnected";
   return "PREstateDotOffline";
}

function PREpeerGridStatusIndicatorHtml(DotClass, TextClass, Text, Title) {
   return /*html*/`
   <div class="PREstateCell" title="${ESChtmlEscape(Title || "")}">
      <span class="PREstateDot ${DotClass}"></span>
      <span class="PREstateText ${TextClass}">${ESChtmlEscape(Text || "")}</span>
   </div>`;
}

function PREpeerGridLatencyInfo(CombinedPeer) {
   var isConnected = (CombinedPeer.connected === true || CombinedPeer.connected === "true");
   var lat = parseFloat(CombinedPeer.latency_ms);
   var latText = (isConnected && Number.isFinite(lat)) ? (Math.round(lat) + " ms") : "--";
   return {
      text: latText,
      className: "",
      dotClass: ""
   };
}

function PREpeerGridStateCellHtml(CombinedPeer) {
   var stateKey = PREpeerGridStateKey(CombinedPeer);
   return PREpeerGridStatusIndicatorHtml(
      PREpeerGridStateDotClass(stateKey),
      PREpeerGridStateTextClass(stateKey, CombinedPeer),
      PREpeerGridStateText(stateKey),
      PREpeerGridStateTitle(CombinedPeer)
   );
}

function PREpeerGridLatencyCellHtml(CombinedPeer) {
   var lat = PREpeerGridLatencyInfo(CombinedPeer);
   return /*html*/`
   <div class="PRElatencyCell">
      <div class="PRElatencyValue${lat.className ? (" " + lat.className) : ""}">${lat.text}</div>
   </div>`;
}

function PREpeerGridStatusCellHtml(CombinedPeer) {
   return PREpeerGridStateCellHtml(CombinedPeer);
}

function PREpeerGridAccessState(CombinedPeer) {
   var access = String(CombinedPeer?.access_level || "").toLowerCase();
   if (access === "full" || access === "allowed") return "allowed";
   if (access === "revoked") return "revoked";
   return "blocked";
}

function PREpeerGridAccessLabel(State) {
   if (State === "allowed") return "Allowed";
   if (State === "revoked") return "Revoked";
   return "Blocked";
}

function PREpeerGridAccessClass(State) {
   if (State === "allowed") return "PREaccessAllowed";
   if (State === "revoked") return "PREaccessRevoked";
   return "PREaccessBlocked";
}

function PREpeerGridAccessCellHtml(CombinedPeer) {
   var isLocalRow = (CombinedPeer && CombinedPeer.__gridScope === "local");
   if (isLocalRow) {
      return /*html*/`
      <div class="PREaccessCell">
         <span class="PREaccessText PREaccessAllowed" title="Read Access: Allowed">Allowed</span>
      </div>`;
   }
   var state = PREpeerGridAccessState(CombinedPeer);
   var label = PREpeerGridAccessLabel(state);
   var stateClass = PREpeerGridAccessClass(state);
   var tooltip = "Read Access: " + label + " (Click to Change)";
   var href = PREhrefAccess(CombinedPeer.id || '');

   return /*html*/`
   <div class="PREaccessCell">
      <a href="${href}" class="PREaccessLink ${stateClass}" title="${tooltip}">
         ${label}
      </a>
   </div>`;
}

function PREpeerGridHealthMetricCellHtml(ColClass, ValClass, Info) {
   var info = Info || { text: "--", title: "No data", className: "PREhealthMetricEmpty" };
   return /*html*/`<div class="${ColClass}"><div class="PREhealthMetric ${ValClass} ${info.className}" title="${ESChtmlEscape(info.title)}">${ESChtmlEscape(info.text)}</div></div>`;
}

function PREpeerGridHealthEmptyCellInfo(Title) {
   return { text: "--", title: String(Title || "No data"), className: "PREhealthMetricEmpty" };
}

function PREpeerGridMetricPct(Obj, Keys) {
   var num = PREpeerGridMetricNumber(Obj, Keys);
   if (!Number.isFinite(num)) return null;
   return PREpercentClamp(num);
}

function PREpeerGridMemPct(Obj) {
   var pct = PREpeerGridMetricPct(Obj, ["mem_percent"]);
   if (Number.isFinite(pct)) return pct;
   var resident = PREpeerGridMetricNumber(Obj, ["mem_resident_mb"]);
   var physical = PREpeerGridMetricNumber(Obj, ["mem_physical_mb"]);
   if (Number.isFinite(resident) && Number.isFinite(physical) && physical > 0) {
      return PREpercentClamp((resident / physical) * 100);
   }
   return null;
}

function PREpeerGridDiskPctNullable(Obj) {
   var direct = PREpeerGridMetricPct(Obj, ["disk_percent", "disk_pct", "disk_used_percent"]);
   if (Number.isFinite(direct)) return direct;
   var free = PREpeerGridMetricNumber(Obj, ["disk_free_gb", "free_disk_gb"]);
   var size = PREpeerGridMetricNumber(Obj, ["disk_size_gb", "total_disk_gb"]);
   if (Number.isFinite(free) && Number.isFinite(size) && size > 0) {
      return PREpercentClamp(100 - ((free / size) * 100));
   }
   return null;
}

function PREpeerGridHealthMetricValue(Label, Obj) {
   if (Label === "CPU") return PREpeerGridMetricPct(Obj, ["cpu_percent", "cpu_pct"]);
   if (Label === "MEM") return PREpeerGridMemPct(Obj);
   return PREpeerGridDiskPctNullable(Obj);
}

function PREpeerGridHealthMetricText(Pct) {
   return Number.isFinite(Pct) ? (String(Pct) + "%") : "--";
}

function PREpeerGridHealthMetricClass(Label, Pct) {
   if (!Number.isFinite(Pct)) return "PREhealthMetricEmpty";
   return "PREhealthMetricOk";
}

function PREpeerGridHealthMetricInfo(Label, Obj) {
   var pct = PREpeerGridHealthMetricValue(Label, Obj);
   return {
      text: PREpeerGridHealthMetricText(pct),
      title: Label + " " + (Number.isFinite(pct) ? (String(pct) + "%") : "No data"),
      className: PREpeerGridHealthMetricClass(Label, pct)
   };
}

function PREpeerGridHealthHasMetricData(Obj) {
   if (Number.isFinite(PREpeerGridHealthMetricValue("CPU", Obj))) return true;
   if (Number.isFinite(PREpeerGridHealthMetricValue("MEM", Obj))) return true;
   if (Number.isFinite(PREpeerGridHealthMetricValue("DSK", Obj))) return true;
   return false;
}

function PREpeerGridHealthNoAccess(Obj) {
   return Obj?.error_code == 403 && !PREpeerGridHealthHasMetricData(Obj);
}

function PREpeerGridHealthCellsDisconnectedHtml(Obj) {
   var msg = String(Obj?.error_msg || "").trim();
   var cpu = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
   var mem = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
   var dsk = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
   return (
      PREpeerGridHealthMetricCellHtml("PREcolCpu", "PREhealthCpuVal", cpu) +
      PREpeerGridHealthMetricCellHtml("PREcolMem", "PREhealthMemVal", mem) +
      PREpeerGridHealthMetricCellHtml("PREcolDsk", "PREhealthDskVal", dsk)
   );
}

function PREpeerGridHealthCellsNoAccessHtml(Obj) {
   var msg = String(Obj?.error_msg || "No read access");
   var info = { text: "N/A", title: msg, className: "PREhealthMetricMuted" };
   return (
      PREpeerGridHealthMetricCellHtml("PREcolCpu", "PREhealthCpuVal", info) +
      PREpeerGridHealthMetricCellHtml("PREcolMem", "PREhealthMemVal", info) +
      PREpeerGridHealthMetricCellHtml("PREcolDsk", "PREhealthDskVal", info)
   );
}

function PREpeerGridHealthCellsHtml(Obj) {
   if (!PREnodeConnected(Obj)) return PREpeerGridHealthCellsDisconnectedHtml(Obj);
   if (PREpeerGridHealthNoAccess(Obj)) return PREpeerGridHealthCellsNoAccessHtml(Obj);
   var cpu = PREpeerGridHealthMetricInfo("CPU", Obj);
   var mem = PREpeerGridHealthMetricInfo("MEM", Obj);
   var dsk = PREpeerGridHealthMetricInfo("DSK", Obj);
   return (
      PREpeerGridHealthMetricCellHtml("PREcolCpu", "PREhealthCpuVal", cpu) +
      PREpeerGridHealthMetricCellHtml("PREcolMem", "PREhealthMemVal", mem) +
      PREpeerGridHealthMetricCellHtml("PREcolDsk", "PREhealthDskVal", dsk)
   );
}

function PREpeerGridDiskPct(Res){
   var free = parseFloat(Res.disk_free_gb || 0);
   var size = parseFloat(Res.disk_size_gb || 0);
   return (size > 0) ? (100 - ((free / size) * 100)) : 0;
}

function PREcommandGridEmptyHtml() {
   return /*html*/`
      <div class="PREemptyState">
         <div class="PREemptyTitle">No Peers Added</div>
         <div class="PREemptyText">Add a Peer to connect, or share your add code from the Identity section in the header.</div>
         <div class="PREemptyActions">
            <div class="BUTTONstandard BUTTONaction PREbtnAdd">+ Add Peer</div>
         </div>
      </div>`;
}

function PREauthorizedPeersEmptyHtml() {
   return /*html*/`
      <div class="PREemptyState">
         <div class="PREemptyTitle">No Authorized Peers</div>
         <div class="PREemptyText">Authorize a peer to allow secure read access from remote instances.</div>
         <div class="PREemptyActions">
            <div class="BUTTONstandard BUTTONaction PREbtnAdd">+ Authorize Peer</div>
         </div>
      </div>`;
}

function PREcommandGridSearchEmptyHtml(SearchValue) {
   var search = String(SearchValue || "").trim();
   return /*html*/`
      <div class="PREemptyState">
         <div class="PREemptyTitle">No Matching Nodes</div>
         <div class="PREemptyText">No nodes match the current search for "${ESChtmlEscape(search)}".</div>
         <div class="PREemptyActions">
            <div class="BUTTONstandard BUTTONdefault PREbtnClearNodeSearch">Clear Search</div>
         </div>
      </div>`;
}

function PREcommandGridNoPeersNoticeHtml() {
   return /*html*/`
      <div class="PREnoPeersNoticeInner">
         <div class="PREnoPeersTitle">No Peers Found</div>
         <div class="PREnoPeersText">Add a peer to start monitoring remote instances from this Command Center.</div>
         <div class="PREemptyActions">
            <div class="BUTTONstandard BUTTONaction PREbtnAdd">+ Add Peer</div>
         </div>
      </div>`;
}
