/* PRElist.js */
/* ==========================================================================
   PRE List Manager
   - Keyed Sync by Peer ID (stale remove + in-place reorder)
   - Render state tracked in JS (no DOM-stored data keys)
========================================================================== */

var PRE_GRID = {
   renderedRows: [],
   rowsById: {},
   identityById: {}
};

function PREpeerGridRowKey(Peer) {
   var id = String(Peer?.id || "");
   if (!id) return "";
   var scope = (Peer && Peer.__gridScope === "local") ? "local" : "peer";
   return String(Peer.__gridRowKey || (scope + ":" + id));
}

function PREpeerGridListCreateRow(Peer) {
   var wrap = document.createElement('div');
   wrap.innerHTML = PREpeerGridRowHtml(Peer);
   var row = wrap.firstElementChild;
   if (row) row.setAttribute('data-grid-row-key', PREpeerGridRowKey(Peer));
   return row;
}

function PREpeerGridListResetRenderState(Target) {
   var rows = Target ? Target.querySelectorAll('.PREpeerWrapper') : [];
   var i;
   for (i = rows.length - 1; i >= 0; i--) rows[i].remove();
   PRE_GRID.renderedRows = [];
   PRE_GRID.rowsById = {};
   PRE_GRID.identityById = {};
}

function PREpeerGridListRemovePeer(Target, peerId) {
   var row = PRE_GRID.rowsById[peerId];
   if (row && row.parentElement === Target) row.remove();
   var idx = PRE_GRID.renderedRows.indexOf(peerId);
   if (idx !== -1) PRE_GRID.renderedRows.splice(idx, 1);
   delete PRE_GRID.rowsById[peerId];
   delete PRE_GRID.identityById[peerId];
}

function PREpeerGridListPruneUntrackedRows(Target) {
   var rows = Target.querySelectorAll('.PREpeerWrapper');
   var i, row, rowKey;
   for (i = 0; i < rows.length; i++) {
      row = rows[i];
      rowKey = row.getAttribute('data-grid-row-key');
      if (rowKey && PRE_GRID.rowsById[rowKey] === row) continue;
      row.remove();
   }
}

function PREpeerGridListPeerIds(List) {
   var out = [];
   var seen = {};
   var i;
   for (i = 0; i < List.length; i++) {
      var rowKey = PREpeerGridRowKey(List[i]);
      if (!rowKey || seen[rowKey]) continue;
      seen[rowKey] = true;
      out.push(rowKey);
   }
   return out;
}

function PREpeerGridListValidMap(PeerIds) {
   var map = {};
   var i;
   for (i = 0; i < PeerIds.length; i++) map[PeerIds[i]] = true;
   return map;
}

function PREpeerGridListInsertAfter(newNode, referenceNode) {
   if (!newNode || !referenceNode || !referenceNode.parentNode) return;
   referenceNode.parentNode.insertBefore(newNode, referenceNode.nextSibling);
}

function PREpeerGridListRemoveStaleRows(Target, ValidIds) {
   var i;
   for (i = PRE_GRID.renderedRows.length - 1; i >= 0; i--) {
      var peerId = PRE_GRID.renderedRows[i];
      var row = PRE_GRID.rowsById[peerId];
      var attached = row && row.parentElement === Target;
      if (!attached || !ValidIds[peerId]) PREpeerGridListRemovePeer(Target, peerId);
   }
}

function PREpeerGridListHandleRowContent(Target, List) {
   var i;
   for (i = 0; i < List.length; i++) {
      var Peer = List[i];
      var rowKey = PREpeerGridRowKey(Peer);
      if (!rowKey) continue;

      var Row = PRE_GRID.rowsById[rowKey];
      if (!Row || Row.parentElement !== Target) {
         Row = PREpeerGridListCreateRow(Peer);
         if (!Row) continue;
         Target.appendChild(Row);
         PRE_GRID.rowsById[rowKey] = Row;
         if (PRE_GRID.renderedRows.indexOf(rowKey) === -1) {
            PRE_GRID.renderedRows.push(rowKey);
         }
      }
      PREsetElementAttrIfChanged(Row, 'data-grid-row-key', rowKey);
      PREpeerGridUpdateIdentity(Row, Peer);
   }
}

function PREpeerGridListMovePeerAfterAnchor(Target, PeerId, AnchorId) {
   var row = PRE_GRID.rowsById[PeerId];
   if (!row || row.parentElement !== Target) return;

   var currentIdx = PRE_GRID.renderedRows.indexOf(PeerId);
   if (currentIdx === -1) return;
   var anchorIdx = AnchorId ? PRE_GRID.renderedRows.indexOf(AnchorId) : -1;
   if (currentIdx === anchorIdx + 1) return;

   if (AnchorId) {
      var anchorRow = PRE_GRID.rowsById[AnchorId];
      if (!anchorRow || anchorRow.parentElement !== Target) return;
      if (row !== anchorRow.nextElementSibling) PREpeerGridListInsertAfter(row, anchorRow);
   } else {
      if (row !== Target.firstElementChild) Target.insertBefore(row, Target.firstElementChild);
   }

   PRE_GRID.renderedRows.splice(currentIdx, 1);
   var insertIdx = AnchorId ? (PRE_GRID.renderedRows.indexOf(AnchorId) + 1) : 0;
   PRE_GRID.renderedRows.splice(insertIdx, 0, PeerId);
}

function PREpeerGridListReorganizePositions(Target, PeerIds) {
   var i;
   for (i = 0; i < PeerIds.length; i++) {
      var peerId = PeerIds[i];
      var anchorId = (i > 0) ? PeerIds[i - 1] : "";
      PREpeerGridListMovePeerAfterAnchor(Target, peerId, anchorId);
   }
}

function PREpeerGridListSyncHeaderGutter(Target) {
   if (!Target) return;
   var Header = document.querySelector('.PREgridHeader');
   if (!Header) return;
   var gutter = Target.offsetWidth - Target.clientWidth;
   if (!Number.isFinite(gutter) || gutter < 0) gutter = 0;
   var next = String(gutter) + "px";
   if (Header.style.getPropertyValue('--PREheaderGutter') === next) return;
   Header.style.setProperty('--PREheaderGutter', next);
}

function PREpeerGridHasLocalRow(List) {
   if (!Array.isArray(List)) return false;
   for (var i = 0; i < List.length; i++) {
      if (List[i] && List[i].__gridScope === "local") return true;
   }
   return false;
}

function PREcommandGridNoPeersNoticeNeeded(List) {
   if (!PREpeerGridHasLocalRow(List)) return false;
   return (PREpeerGridPeerList() || []).length === 0;
}

function PREcommandGridNoPeersNoticeSync(Target, List) {
   if (!Target) return;
   var Notice = Target.querySelector('.PREnoPeersNotice');
   if (!PREcommandGridNoPeersNoticeNeeded(List)) {
      if (Notice) Notice.remove();
      return;
   }
   if (Notice) return;

   var Wrap = document.createElement('div');
   Wrap.className = 'PREnoPeersNotice';
   Wrap.innerHTML = PREcommandGridNoPeersNoticeHtml();
   Target.appendChild(Wrap);
}

function PREpeerGridListReady() {
   if (!PREpeerGridListDataReady()) return false;
   var rawList = PREpeerGridPeerList() || [];
   if (!PREpeerGridLocalDataReady() && rawList.length === 0) return false;
   return true;
}

function PREpeerGridRenderList(Target, List) {
   if (!Target) return;
   if (!Array.isArray(List)) List = [];

   var empty = Target.querySelector('.PREemptyState');
   if (empty) empty.remove();

   // Mirror DASH pattern: clear stale DOM if render state was lost.
   if (Target.querySelector('.PREpeerWrapper') && PRE_GRID.renderedRows.length === 0) {
      PREpeerGridListResetRenderState(Target);
   }

   var desiredIds = PREpeerGridListPeerIds(List);
   var validMap = PREpeerGridListValidMap(desiredIds);

   PREpeerGridListRemoveStaleRows(Target, validMap);
   PREpeerGridListPruneUntrackedRows(Target);
   PREpeerGridListHandleRowContent(Target, List);
   PREpeerGridListReorganizePositions(Target, desiredIds);
   PREpeerGridListSyncHeaderGutter(Target);

   PRE_GRID.renderedRows = desiredIds.slice();
}

function PREcommandGridUpdateList() {
   var Target = document.querySelector('.PREgridTarget');
   if (!Target) return;
   if (!PREpeerGridListReady()) return;

   var List = PREcommandGridDisplayPeerList() || [];
   var searchValue = PREnodeSearchValue();
   if (List.length === 0) {
      PREpeerGridListResetRenderState(Target);
      Target.innerHTML = searchValue
         ? PREcommandGridSearchEmptyHtml(searchValue)
         : PREcommandGridEmptyHtml();
      return;
   }

   PREpeerGridRenderList(Target, List);
   PREcommandGridNoPeersNoticeSync(Target, List);
}

function PREauthorizedPeersUpdateList() {
   var Target = document.querySelector('.PREgridTarget');
   if (!Target) return;
   if (!PREpeerGridListReady()) return;

   var List = PREauthorizedPeersDisplayPeerList() || [];
   if (List.length === 0) {
      PREpeerGridListResetRenderState(Target);
      Target.innerHTML = PREauthorizedPeersEmptyHtml();
      return;
   }

   PREpeerGridRenderList(Target, List);
}

function PREpeerGridRowAt(Target, RowKey) {
   if (!Target) return null;
   var row = PRE_GRID.rowsById[RowKey];
   if (row && row.parentElement === Target) return row;
   var rows = Target.querySelectorAll('.PREpeerWrapper');
   for (var i = 0; i < rows.length; i++) {
      if (rows[i].getAttribute('data-grid-row-key') !== RowKey) continue;
      PRE_GRID.rowsById[RowKey] = rows[i];
      return rows[i];
   }
   return null;
}

function PREpeerGridLiveStatus(Config, StatusMap, IsLocalRow) {
   return PREpeerGridNodeStatus(StatusMap, Config.id, IsLocalRow);
}

function PREpeerGridCombinedWithStatus(Config, LiveStatus) {
   return { ...Config, ...LiveStatus };
}

function PREpeerGridUpdateStatusColumns(Row, Combined) {
   PREpeerGridUpdateRowSeverityClass(Row, Combined);

   var LinkCol = Row.querySelector('.PREcolLink');
   if (LinkCol && !PREpeerGridStatusTryUpdate(LinkCol, Combined)) {
      PREsetElementHTMLIfChanged(LinkCol, PREpeerGridStatusCellHtml(Combined));
   }
   var LatCol = Row.querySelector('.PREcolLatency');
   if (LatCol && !PREpeerGridLatencyTryUpdate(LatCol, Combined)) {
      PREsetElementHTMLIfChanged(LatCol, PREpeerGridLatencyCellHtml(Combined));
   }

   var AccessCol = Row.querySelector('.PREcolAccess');
   if (AccessCol && !PREpeerGridAccessTryUpdate(AccessCol, Combined)) {
      PREsetElementHTMLIfChanged(AccessCol, PREpeerGridAccessCellHtml(Combined));
   }

   var RateCol = Row.querySelector('.PREcolRate');
   if (RateCol) PREsetElementHTMLIfChanged(RateCol, PREpeerGridMetricValueHtml(PREpeerGridRateMetricText(Combined)));
   var ErrCol = Row.querySelector('.PREcolErrors');
   if (ErrCol) {
      PREsetElementHTMLIfChanged(ErrCol, PREpeerGridMetricValueHtml(PREpeerGridErrorMetricText(Combined)));
      var errClass = PREpeerGridErrorMetricClass(Combined);
      PREsetElementClassIfChanged(ErrCol, "PREcolErrors PREmetricCell" + (errClass ? (" " + errClass) : ""));
   }
   var QueueCol = Row.querySelector('.PREcolQueue');
   if (QueueCol) PREsetElementHTMLIfChanged(QueueCol, PREpeerGridMetricValueHtml(PREpeerGridQueueMetricText(Combined)));
}

function PREpeerGridUpdateRowSeverityClass(Row, Combined) {
   if (!Row) return;
   var scope = (Combined && Combined.__gridScope === "local") ? "local" : "peer";
   PREsetElementClassIfChanged(Row, PREpeerGridRowWrapperClass(scope, Combined));
}

function PREpeerGridRetryButtonSetConnecting(Btn) {
   var connectingUntil = Number(Btn && Btn.preConnectingUntil || 0);
   if (connectingUntil <= Date.now()) return false;
   if (Btn.textContent !== "Connecting...") Btn.textContent = "Connecting...";
   Btn.classList.add('PREbtnIsConnecting');
   Btn.classList.add('PREbtnIsConnectingDim');
   return true;
}

function PREpeerGridRetryButtonSetIdle(Btn, Label) {
   var label = String(Label || (Btn.classList.contains('PREbtnRetryDisconnect') ? "Disconnect" : "Connect"));
   if (Btn.textContent !== label) Btn.textContent = label;
   Btn.classList.remove('PREbtnIsConnecting');
   Btn.classList.remove('PREbtnIsConnectingDim');
   Btn.preConnectingUntil = 0;
}

function PREpeerGridActionTryUpdate(Row, Combined) {
   var ActionsCol = Row.querySelector('.PREcolActionsRow');
   if (!ActionsCol) return true;
   var isLocalRow = (Combined && Combined.__gridScope === "local");
   var isConnected = PREnodeConnected(Combined);
   var shouldShow = !isLocalRow;
   var Btn = ActionsCol.querySelector('.PREbtnRetry');
   var Placeholder = ActionsCol.querySelector('.PREactionPlaceholder');
   var label = isConnected ? "Disconnect" : "Connect";
   var title = label;
   var cls = isConnected
      ? "PREbtnRetry PREbtnRetryDanger PREbtnRetryDisconnect"
      : "PREbtnRetry PREbtnRetryGhost PREbtnRetryConnect";

   if (!shouldShow) {
      if (Btn) Btn.remove();
      if (!Placeholder) {
         var span = document.createElement('span');
         span.className = 'PREactionPlaceholder';
         span.setAttribute('title', 'No action available');
         span.textContent = '--';
         ActionsCol.appendChild(span);
      }
      return true;
   }

   if (Placeholder) Placeholder.remove();

   if (!Btn) {
      Btn = document.createElement('div');
      Btn.className = cls;
      Btn.setAttribute('title', title);
      Btn.textContent = label;
      ActionsCol.appendChild(Btn);
      return true;
   }

   PREsetElementClassIfChanged(Btn, cls);
   PREsetElementAttrIfChanged(Btn, "title", title);
   if (!isConnected) {
      if (!PREpeerGridRetryButtonSetConnecting(Btn)) PREpeerGridRetryButtonSetIdle(Btn, label);
      return true;
   }
   PREpeerGridRetryButtonSetIdle(Btn, label);
   return true;
}

function PREpeerGridUpdateStatus() {
   var Target = document.querySelector('.PREgridTarget');
   if (!Target) return;
   var List = PREpeerGridDisplayPeerList() || [];
   var StatusMap = PREpeerGridStatusGet();
   for (var i = 0; i < List.length; i++) {
      var Config = List[i];
      var rowKey = PREpeerGridRowKey(Config);
      var Row = PREpeerGridRowAt(Target, rowKey);
      if (!Row) continue;
      var isLocalRow = (Config && Config.__gridScope === "local");
      var LiveStatus = PREpeerGridLiveStatus(Config, StatusMap, isLocalRow);
      var Combined = PREpeerGridCombinedWithStatus(Config, LiveStatus);
      PREpeerGridUpdateStatusColumns(Row, Combined);
      PREpeerGridActionTryUpdate(Row, Combined);
   }
}

function PREpeerGridStatusTryUpdate(Container, Combined) {
   var stateKey = PREpeerGridStateKey(Combined);
   var stateCell = Container.querySelector('.PREstateCell');
   var stateDot = Container.querySelector('.PREstateDot');
   var stateText = Container.querySelector('.PREstateText');
   if (stateCell && stateDot && stateText) {
      PREsetElementClassIfChanged(stateDot, "PREstateDot " + PREpeerGridStateDotClass(stateKey));
      PREsetElementClassIfChanged(stateText, "PREstateText " + PREpeerGridStateTextClass(stateKey, Combined));
      PREsetElementTextIfChanged(stateText, PREpeerGridStateText(stateKey));
      PREsetElementAttrIfChanged(stateCell, "title", PREpeerGridStateTitle(Combined));
      return true;
   }
   return false;
}

function PREpeerGridLiveLatencyInfo(Combined) {
   var isConnected = (Combined.connected === true || Combined.connected === "true");
   var lat = parseFloat(Combined.latency_ms);
   var text = (isConnected && Number.isFinite(lat)) ? (Math.round(lat) + " ms") : "--";
   return {
      text: text,
      className: "",
      dotClass: ""
   };
}

function PREpeerGridLatencyTryUpdate(Container, Combined) {
   var latencyEl = Container.querySelector('.PRElatencyValue');
   if (!latencyEl) return false;
   var lat = PREpeerGridLiveLatencyInfo(Combined);
   PREsetElementClassIfChanged(latencyEl, "PRElatencyValue" + (lat.className ? (" " + lat.className) : ""));
   PREsetElementTextIfChanged(latencyEl, lat.text);
   return true;
}

function PREpeerGridAccessTryUpdate(Container, Combined) {
   var legacyBadge = Container.querySelector('.PREaccessBadge');
   var legacyMutedLink = Container.querySelector('.PREaccessMutedLink');
   var legacyPlainText = Container.querySelector('.PREaccessPlain');
   var link = Container.querySelector('.PREaccessLink');
   var text = Container.querySelector('.PREaccessText');
   var isLocalRow = (Combined && Combined.__gridScope === "local");
   if (legacyBadge) legacyBadge.remove();
   if (legacyMutedLink) legacyMutedLink.remove();
   if (legacyPlainText) legacyPlainText.remove();

   if (isLocalRow) {
      if (link) link.remove();
      if (!text) return false;
      PREsetElementClassIfChanged(text, "PREaccessText PREaccessAllowed");
      PREsetElementTextIfChanged(text, "Allowed");
      PREsetElementAttrIfChanged(text, "title", "Read Access: Allowed");
      return true;
   }
   if (text) text.remove();

   if (!link) return false;
   var state = PREpeerGridAccessState(Combined);
   var label = PREpeerGridAccessLabel(state);
   var stateClass = PREpeerGridAccessClass(state);
   var tooltip = "Read Access: " + label + " (Click to Change)";
   var href = PREhrefAccess(Combined.id || "");
   PREsetElementClassIfChanged(link, "PREaccessLink " + stateClass);
   PREsetElementTextIfChanged(link, label);
   PREsetElementAttrIfChanged(link, "href", href);
   PREsetElementAttrIfChanged(link, "title", tooltip);
   return true;
}

function PREpeerGridHealthTryUpdate(Row, Combined) {
   var cpuEl = Row.querySelector('.PREhealthCpuVal');
   var memEl = Row.querySelector('.PREhealthMemVal');
   var dskEl = Row.querySelector('.PREhealthDskVal');
   if (!cpuEl || !memEl || !dskEl) return false;

   var cpuInfo, memInfo, dskInfo;
   if (!PREnodeConnected(Combined)) {
      var msg = String(Combined?.error_msg || "").trim();
      cpuInfo = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
      memInfo = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
      dskInfo = PREpeerGridHealthEmptyCellInfo(msg || "Disconnected");
   } else if (PREpeerGridHealthNoAccess(Combined)) {
      var tip = String(Combined?.error_msg || "No read access");
      cpuInfo = { text: "N/A", title: tip, className: "PREhealthMetricMuted" };
      memInfo = { text: "N/A", title: tip, className: "PREhealthMetricMuted" };
      dskInfo = { text: "N/A", title: tip, className: "PREhealthMetricMuted" };
   } else {
      cpuInfo = PREpeerGridHealthMetricInfo("CPU", Combined);
      memInfo = PREpeerGridHealthMetricInfo("MEM", Combined);
      dskInfo = PREpeerGridHealthMetricInfo("DSK", Combined);
   }

   PREsetElementClassIfChanged(cpuEl, "PREhealthMetric PREhealthCpuVal " + cpuInfo.className);
   PREsetElementTextIfChanged(cpuEl, cpuInfo.text);
   PREsetElementAttrIfChanged(cpuEl, "title", cpuInfo.title);

   PREsetElementClassIfChanged(memEl, "PREhealthMetric PREhealthMemVal " + memInfo.className);
   PREsetElementTextIfChanged(memEl, memInfo.text);
   PREsetElementAttrIfChanged(memEl, "title", memInfo.title);

   PREsetElementClassIfChanged(dskEl, "PREhealthMetric PREhealthDskVal " + dskInfo.className);
   PREsetElementTextIfChanged(dskEl, dskInfo.text);
   PREsetElementAttrIfChanged(dskEl, "title", dskInfo.title);

   return true;
}

function PREcommandGridUpdateResources() {
   var Target = document.querySelector('.PREgridTarget');
   if (!Target) return;
   var List = PREcommandGridDisplayPeerList() || [];
   var StatusMap = PREpeerGridStatusGet();
   var ResMap = PREpeerGridResourcesGet();
   var LocalRes = PREpeerGridLocalResourcesGet();
   for (var i = 0; i < List.length; i++) {
      var Config = List[i];
      var rowKey = PREpeerGridRowKey(Config);
      var Row = PREpeerGridRowAt(Target, rowKey);
      if (!Row) continue;
      var isLocalRow = (Config && Config.__gridScope === "local");
      var LiveStatus = PREpeerGridNodeStatus(StatusMap, Config.id, isLocalRow);
      var LiveRes = ResMap[Config.id] || {};
      if (isLocalRow && (!LiveRes || Object.keys(LiveRes).length === 0)) LiveRes = LocalRes || {};
      var Combined = { ...Config, ...LiveStatus, ...LiveRes };
      PREpeerGridUpdateRowSeverityClass(Row, Combined);
      if (!PREpeerGridHealthTryUpdate(Row, Combined)) {
         PREcommandGridUpdateList();
         return;
      }
   }
}

function PREpeerGridUpdateIdentity(Row, Peer) {
   var peerId = String(Peer.id || "");
   var rowKey = PREpeerGridRowKey(Peer);
   var isLocalRow = (Peer && Peer.__gridScope === "local");
   var NameLink = Row.querySelector('.PREpeerNameLink');
   var NameText = Row.querySelector('.PREpeerNameText');
   var HostLink = Row.querySelector('.PREpeerHostLink');
   var IdValue = Row.querySelector('.PREpeerIdText');
   var name = String(Peer.name || Peer.id || "Unknown");
   var host = String(Peer.host || "") + ':' + String(Peer.port || "");
   var escHost = ESChtmlEscape(host);
   var detailsHref = isLocalRow ? PREhrefDetails() : PREhrefDetails(Peer.id || '');
   var protocol = Peer.https ? "https://" : "http://";
   var externalUrl = protocol + String(Peer.host || "") + ":" + String(Peer.port || "");
   var hostKey = host + "|" + protocol;
   var showId = !!String(peerId || "").trim() && !PREpeerGridNameMatchesId(name, peerId);
   var idClass = showId ? "PREpeerIdText" : "PREpeerIdText PREpeerIdTextHidden";
   var idText = showId ? ("ID: " + (peerId || "Unknown")) : "";
   var state = PRE_GRID.identityById[rowKey] || {};

   if (NameLink) {
      if (state.name !== name) PREsetElementTextIfChanged(NameLink, name);
      if (state.detailsHref !== detailsHref) PREsetElementAttrIfChanged(NameLink, 'href', detailsHref);
   }
   if (NameText && state.name !== name) PREsetElementTextIfChanged(NameText, name);

   if (HostLink) {
      if (state.hostKey !== hostKey) {
         var svg = '<svg class="PRElinkIcon" viewBox="0 0 24 24"><path fill="currentColor" d="M19 19H5V5h7V3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2v-7h-2v7zM14 3v2h3.59l-9.83 9.83 1.41 1.41L19 6.41V10h2V3h-7z"/></svg>';
         PREsetElementHTMLIfChanged(HostLink, '<span class="PREpeerHostText">' + escHost + '</span>' + svg);
      }

      if (state.externalUrl !== externalUrl) PREsetElementAttrIfChanged(HostLink, 'href', externalUrl);
      var hostTitle = "Open " + externalUrl;
      if (state.hostTitle !== hostTitle) PREsetElementAttrIfChanged(HostLink, 'title', hostTitle);
   }
   if (IdValue) {
      if (state.idText !== idText) PREsetElementTextIfChanged(IdValue, idText);
      if (state.idClass !== idClass) PREsetElementClassIfChanged(IdValue, idClass);
   }

   PRE_GRID.identityById[rowKey] = {
      name: name,
      detailsHref: detailsHref,
      hostKey: hostKey,
      externalUrl: externalUrl,
      hostTitle: "Open " + externalUrl,
      idText: idText,
      idClass: idClass
   };
}
