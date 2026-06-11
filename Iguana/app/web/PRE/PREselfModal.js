/* ==========================================================================
   PRE Self Views
   - command/connect + settings/authorized_peers/connect: Invite + connection details modal
   - command/details (no peer): Local server details modal
========================================================================== */

var PRE_SELF_MODE_CONNECT = "connect";
var PRE_SELF_MODE_DETAILS = "details";

function PREselfDetailsScreen() { PREselfBaseScreen(); }
function PREselfConnectScreen() { PREselfBaseScreen(); }

function PREselfOpenShareModal() {
   location.hash = PRErouteConnect();
}

function PREselfModalRouteMatch(Page) {
   var root = document.querySelector('.PREselfModal');
   if (!root) return false;
   return PREselfModeFromRoot(root) === PREselfModeFromRoute(Page);
}

function PREselfBaseScreen() {
   if (!PREselfModalRouteMatch()) PAGEremovePopouts();
   PREselfModelInit();
   MODELforcePoll('PREselfModel');
}

function PREselfModelInit() {
   MODELpollerAdd('PREselfModel', 'PREselfData', 60, 'iguanas/me', {});
   MODELpollerAdd('PREselfModel', 'PREselfResources', 3, 'resources/current', {});
   MODELviewAdd('PREselfModel', 'PREselfRender');
}

function PREselfModeFromRoute(Page) {
   var page = String(Page || PAGEhash() || "");
   if (page === PRErouteConnectPath(page)) return PRE_SELF_MODE_CONNECT;
   return PRE_SELF_MODE_DETAILS;
}

function PREselfModeFromRoot(Root) {
   if (Root && Root.classList.contains("PREselfConnectModal")) return PRE_SELF_MODE_CONNECT;
   return PRE_SELF_MODE_DETAILS;
}

function PREselfRouteActive(Page) {
   var page = String(Page || PAGEhash() || "");
   if (page === PRErouteConnectPath(page)) return true;
   if (page !== PRErouteDetailsPath(page)) return false;
   return !PREpeerParam();
}

function PREselfRender() {
   var page = PAGEhash();
   if (!PREselfRouteActive(page)) return;

   var Me = PREselfGet();
   var LocalRes = PREselfResourcesGet();
   if (!Me || !Me.id) return;

   var Mode = PREselfModeFromRoute(page);
   var Root = document.querySelector('.PREselfModal');
   if (Root) {
      var rootMode = PREselfModeFromRoot(Root);
      if (rootMode !== Mode) {
         PAGEremovePopouts();
         Root = null;
      } else {
         PREselfUpdateModal(Root, Me, LocalRes, rootMode);
         return;
      }
   }
   PREshowSelfModal(Me, LocalRes, Mode);
}

function PREshowSelfModal(Me, LocalRes, Mode) {
   POPpopoutOpen({
      title: PREselfModalTitle(Me),
      content: PREselfModalHtml(Me, LocalRes, Mode),
      className: "PREpopout",
      width: "670px",
      esc_close: true,
      callback: function() { location.hash = PRErouteList(); }
   });

   var Root = document.querySelector(".PREselfModal");
   if (!Root) return;

   var CloseBtn = Root.querySelector(".PREselfCloseBtn");
   if (CloseBtn) CloseBtn.onclick = function() { location.hash = PRErouteList(); };

   var CopyBtns = Root.querySelectorAll(".PREselfCopyBtn");
   for (var i = 0; i < CopyBtns.length; i++) {
      CopyBtns[i].onclick = function() { PREselfCopyCode(PREselfGet() || Me, this); };
   }

   var CodeInput = Root.querySelector(".PREselfCodeInput");
   if (CodeInput) CodeInput.onclick = function() { this.select(); };

   PREselfUpdateModal(Root, Me, LocalRes, Mode);
}

function PREselfCopyCode(Me, Btn) {
   if (!Btn || !Me) return;
   var code = INVinviteCodeTryEncode({
      host: Me.host, port: Me.port, id: Me.id, fingerprint: Me.fingerprint
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

function PREselfModalTitle(Me) {
   var name = ESChtmlEscape(String(Me?.name || Me?.id || "Local"));
   return `<span class="PREselfTitleWrap"><span class="PREselfTitleText">${name}</span><span class="PREbadgeLocal PREselfTitleLocal">This server</span></span>`;
}

function PREselfExternalUrl(Me) {
   var protocol = Me && Me.https ? "https://" : "http://";
   return protocol + String(Me?.host || "") + ":" + String(Me?.port || "");
}

function PREselfHealthPct(Val) {
   var pct = parseFloat(Val);
   if (!Number.isFinite(pct)) return null;
   return PREpercentClamp(pct);
}

function PREselfHealthDiskPct(Res) {
   if (!Res) return null;
   var direct = parseFloat(Res.disk_percent);
   if (Number.isFinite(direct)) return PREpercentClamp(direct);
   var size = parseFloat(Res.disk_size_gb);
   var free = parseFloat(Res.disk_free_gb);
   if (!Number.isFinite(size) || size <= 0 || !Number.isFinite(free)) return null;
   return PREpercentClamp(100 - ((free / size) * 100));
}

function PREselfModalBar(Label, Pct) {
   if (Pct === null || Pct === undefined) {
      return /*html*/`
      <div class="PREmodalBarRow">
         <div class="PREmodalBarLabel">${Label}</div>
         <div class="PREmodalBarTrack">
            <div class="PREmodalBarFill" style="width:0%"></div>
         </div>
         <div class="PREmodalBarValue">--</div>
      </div>`;
   }

   var pct = PREpercentClamp(Pct);
   return /*html*/`
   <div class="PREmodalBarRow">
      <div class="PREmodalBarLabel">${Label}</div>
      <div class="PREmodalBarTrack">
         <div class="PREmodalBarFill PREmodalBarFillActive" style="width:${pct}%"></div>
      </div>
      <div class="PREmodalBarValue">${pct}%</div>
   </div>`;
}

function PREselfHealthHtml(Res) {
   var cpu = PREselfHealthPct(Res && Res.cpu_percent);
   var mem = PREselfHealthPct(PREresourceMemPercent(Res));
   var dsk = PREselfHealthDiskPct(Res);
   if (cpu === null && mem === null && dsk === null) {
      return /*html*/`<div class="PREmodalMessage">Waiting for local resource stats...</div>`;
   }
   return /*html*/`
   <div class="PREmodalResourceBars">
      ${PREselfModalBar("CPU", cpu)}
      ${PREselfModalBar("MEM", mem)}
      ${PREselfModalBar("DSK", dsk)}
   </div>`;
}

function PREselfWarningHtml(Me) {
   var isLocalHost = (Me.host === "127.0.0.1" || Me.host === "localhost" || Me.host === "0.0.0.0");
   if (!isLocalHost) return "";
   return /*html*/`
   <div class="PRElocalWarning">
      <svg class="PREwarningIcon" viewBox="0 0 24 24" fill="currentColor">
         <path d="M11 7h2v2h-2zm0 4h2v6h-2zm1-9C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8z"/>
      </svg>
      <div>
         <b>Note:</b> Remote peers need your Public or LAN IP, not <b>${ESChtmlEscape(Me.host)}</b>.
      </div>
   </div>`;
}

function PREselfModalFooterHtml() {
   return /*html*/`
      <div class="PREformActions">
         <div class="BUTTONstandard BUTTONdefault PREselfCloseBtn">Close</div>
      </div>`;
}

function PREselfConnectionDetailsRowsHtml(Me, ExternalUrl, IncludeLink) {
   var linkHtml = IncludeLink
      ? `<a href="${ExternalUrl}" target="_blank" class="PREmodalLinkBtn" title="Open ${ExternalUrl}">
            <svg viewBox="0 0 24 24"><path fill="currentColor" d="M19 19H5V5h7V3H5c-1.11 0-2 .9-2 2v14c0 1.1.89 2 2 2h14c1.1 0 2-.9 2-2v-7h-2v7zM14 3v2h3.59l-9.83 9.83 1.41 1.41L19 6.41V10h2V3h-7z"/></svg>
         </a>`
      : "";
   var addressField = IncludeLink
      ? `<div class="PREinputGroup">
            <input type="text" class="FORMinput PREselfValHost" readonly value="${ESChtmlEscape(Me.host)}">
            ${linkHtml}
         </div>`
      : `<input type="text" class="FORMinput PREselfValHost" readonly value="${ESChtmlEscape(Me.host)}">`;
   return /*html*/`
            <div class="PREformRowGrid">
               <div class="PREformLabel">Address</div>
               ${addressField}
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Port</div>
               <input type="text" class="FORMinput PREselfValPort" readonly value="${ESChtmlEscape(Me.port)}">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">ID</div>
               <input type="text" class="FORMinput PREmono PREselfValId" readonly value="${ESChtmlEscape(Me.id)}">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Fingerprint</div>
               <input type="text" class="FORMinput PREmono PREselfValFingerprint" readonly value="${ESChtmlEscape(Me.fingerprint)}">
            </div>`;
}

function PREselfConnectInviteSectionHtml(Me, Code) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeader">Invite Code</div>
         <div class="PREcard">
            <div class="PREcodeLabel">Auto-generated Code</div>
            <div class="PREcodeWrapper">
               <input type="text" class="FORMinput PREmono PREselfCodeInput PREselfInputFlex" readonly value="${ESChtmlEscape(Code)}" spellcheck="false">
            </div>
            <div class="BUTTONstandard BUTTONaction PREselfCopyBtn PREbtnFull">
               Copy Code to Clipboard
            </div>
            ${PREselfWarningHtml(Me)}
         </div>
      </div>`;
}

function PREselfConnectDetailsSectionHtml(Me) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeader">Connection Details</div>
         <div class="PREcard">
            ${PREselfConnectionDetailsRowsHtml(Me, "", false)}
         </div>
      </div>`;
}

function PREselfDetailsHealthSectionHtml(LocalRes) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeader">Health</div>
         <div class="PREcard PREselfHealthCard">
            ${PREselfHealthHtml(LocalRes)}
         </div>
      </div>`;
}

function PREselfDetailsConnectionDetailsSectionHtml(Me, ExternalUrl) {
   return /*html*/`
      <div class="PREformSection">
         <div class="PREsectionHeaderSplit">
            <span>Connection Details</span>
            <div class="PREbtnCopyCode PREselfCopyBtn">Copy Invite Code</div>
         </div>
         <div class="PREcard">
            ${PREselfConnectionDetailsRowsHtml(Me, ExternalUrl, true)}
         </div>
      </div>`;
}

function PREselfConnectModalHtml(Me) {
   var code = INVinviteCodeTryEncode({ host: Me.host, port: Me.port, id: Me.id, fingerprint: Me.fingerprint }) || "Error generating code";
   return /*html*/`
   <div class="PREaddPeerModal PREselfModal PREselfConnectModal">
      <div class="PREmodalIntro">
         Share these details so other peers can connect to you.
      </div>
      ${PREselfConnectInviteSectionHtml(Me, code)}
      ${PREselfConnectDetailsSectionHtml(Me)}
      ${PREselfModalFooterHtml()}
   </div>`;
}

function PREselfDetailsModalHtml(Me, LocalRes) {
   var externalUrl = PREselfExternalUrl(Me);
   return /*html*/`
   <div class="PREaddPeerModal PREselfModal PREselfDetailsModal">
      ${PREselfDetailsHealthSectionHtml(LocalRes)}
      ${PREselfDetailsConnectionDetailsSectionHtml(Me, externalUrl)}
      ${PREselfModalFooterHtml()}
   </div>`;
}

function PREselfUpdateModal(Root, Me, LocalRes, Mode) {
   if (!Root || !Me) return;
   var code = INVinviteCodeTryEncode({ host: Me.host, port: Me.port, id: Me.id, fingerprint: Me.fingerprint }) || "Error generating code";
   var externalUrl = PREselfExternalUrl(Me);

   PREsetElementValueIfChanged(Root.querySelector(".PREselfCodeInput"), code);
   PREsetElementValueIfChanged(Root.querySelector(".PREselfValHost"), String(Me.host || ""));
   PREsetElementValueIfChanged(Root.querySelector(".PREselfValPort"), String(Me.port || ""));
   PREsetElementValueIfChanged(Root.querySelector(".PREselfValId"), String(Me.id || ""));
   PREsetElementValueIfChanged(Root.querySelector(".PREselfValFingerprint"), String(Me.fingerprint || ""));

   var HostLink = Root.querySelector(".PREmodalLinkBtn");
   if (HostLink) {
      PREsetElementAttrIfChanged(HostLink, "href", externalUrl);
      PREsetElementAttrIfChanged(HostLink, "title", "Open " + externalUrl);
   }

   var healthCard = Root.querySelector(".PREselfHealthCard");
   if (healthCard) PREsetElementHTMLIfChanged(healthCard, PREselfHealthHtml(LocalRes));

   var TitleEl = Root.closest('.POPpopoutCard')?.querySelector('.POPpopoutCard-title h3');
   if (TitleEl) {
      var titleHtml = PREselfModalTitle(Me);
      if (TitleEl.innerHTML !== titleHtml) TitleEl.innerHTML = titleHtml;
   }
}

function PREselfModalHtml(Me, LocalRes, Mode) {
   if (Mode === "connect") return PREselfConnectModalHtml(Me);
   return PREselfDetailsModalHtml(Me, LocalRes);
}
