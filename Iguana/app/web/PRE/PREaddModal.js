/* ==========================================================================
   PREaddModal.js
   - Logic for Add Peer Modal
   - Handles Invite Code parsing, progressive disclosure, and validation
========================================================================== */

function PREaddGetSelf() {
   return MODELdata?.PREaddData?.data || {};
}

function PREaddSubmit(payload, callback) {
   APIcall("iguanas/add", payload, callback);
}

function PREaddSetFullAccess(peerId, callback) {
   APIcall("iguanas/access/set", { id: peerId, access_level: "full" }, callback);
}

function PREaddActionLabel() {
   return PREcommandCenterMode() ? "Add Peer" : "Authorize Peer";
}

function PREaddAllowAccessDefault() {
   return !PREcommandCenterMode();
}

function PREaddAllowAccessVisible() {
   return PREcommandCenterMode();
}

function PREaddAllowAccessChecked(Root) {
   if (!PREaddAllowAccessVisible()) return true;
   var Input = Root ? Root.querySelector(".PREaddAllowAccessInput") : null;
   if (!Input) return PREaddAllowAccessDefault();
   return Input.checked === true;
}

function PREaddRouteBack() {
   return PRErouteList();
}

function PREaddRender() {
   if (PAGEhash() !== PRErouteAddPath(PAGEhash())) return;
   var Self = PREaddGetSelf();
   if (!Self || !Self.id) return;
   if (document.querySelector('.PREaddPeerModal')) return;
   PREshowAddModal();
}

function PREshowAddModal() {
   var actionLabel = PREaddActionLabel();
   POPpopoutOpen({
      title: actionLabel,
      content: PREaddPeerModalHtml(),
      className: "PREpopout",
      width: "664px",
      esc_close: true,
      callback: function () { location.hash = PREaddRouteBack(); }
   });
   var Root = document.querySelector(".PREaddPeerModal");
   if (!Root) return;
   PREaddPeerInit(Root);
   PREaddPeerBind(Root);
   var Invite = Root.querySelector(".PREinviteCodeInput");
   FORMfocus(Invite);
   FORMenterListener(Invite, function () { PREaddPeerSubmit(Root); });
   var Cancel = Root.querySelector(".PREaddCancelBtn");
   if (Cancel) Cancel.onclick = function () { location.hash = PREaddRouteBack(); };
}

function PREaddPeerInit(Root) {
   Root.classList.remove("PREmanualOpen");
   Root.classList.remove("PREinviteDecoded");
   PREsetParsedInvitePreviewVisible(Root, false);
   PREsetAddPeerError(Root, "");
   PRErefreshManualToggleLabel(Root);
}

function PREaddPeerBind(Root) {
   Root.addEventListener("click", function (e) {
      if (e.target.closest(".PREaddPeerBtn")) return PREaddPeerSubmit(Root);
      if (e.target.closest(".PREmanualToggle")) { e.preventDefault(); return PREtoggleManual(Root); }
   });
   
   Root.addEventListener("keyup", function (e) {
      if (e.key === "Escape") return location.hash = PREaddRouteBack();
   });
   
   Root.addEventListener("keydown", function (e) {
      var ok = e.target.closest(".PREaddHostInput") || 
               e.target.closest(".PREaddPortInput") || 
               e.target.closest(".PREaddIdInput") || 
               e.target.closest(".PREaddFingerprintInput") ||
               e.target.closest(".PREparsedHostInput"); // Catch enter on inline input
      if (!ok) return;
      return FORMkeydownListeners(e, function () { PREaddPeerSubmit(Root); });
   });
   
   Root.addEventListener("input", function (e) {
      // Sync inline preview input -> manual hidden input
      if (e.target.classList.contains("PREparsedHostInput")) {
         var manualHost = Root.querySelector(".PREaddHostInput");
         if (manualHost) manualHost.value = e.target.value;
      }
      
      // Sync manual hidden input -> inline preview input
      if (e.target.classList.contains("PREaddHostInput")) {
         var inlineHost = Root.querySelector(".PREparsedHostInput");
         if (inlineHost) inlineHost.value = e.target.value;
      }

      // Handle invite code decoding
      if (e.target.closest(".PREinviteCodeInput")) {
         PREinviteHandle(Root);
      }
   });
}

function PREaddPeerSubmit(Root) {
   PREsetAddPeerError(Root, "");

   var formData = PREaddPeerFormData(Root);
   var validationError = PREaddPeerValidationError(formData);
   if (validationError) return PREsetAddPeerError(Root, ESChtmlEscape(validationError));

   PREaddSubmit(formData, function (R) {
      if (R && R.success) return PREaddPeerSubmitSuccess(Root, formData.id);
      PREaddPeerSubmitFail(Root, R);
   });
}

function PREaddPeerFormData(Root) {
   return {
      host: PREfieldValue(Root, ".PREaddHostInput"),
      port: PREfieldValue(Root, ".PREaddPortInput"),
      id: PREfieldValue(Root, ".PREaddIdInput"),
      fingerprint: PREfieldValue(Root, ".PREaddFingerprintInput")
   };
}

function PREaddPeerValidationError(Data) {
   var data = Data || {};
   if (!data.host || !data.port || !data.id || !data.fingerprint) {
      return "Enter address, port, ID, and fingerprint.";
   }
   var err = PREaddValidateConnectionInputs(data.host, data.port) || PREaddValidatePeerId(data.id);
   if (err) return err;
   return PREaddPeerSelfWarning(data.host, data.port, data.id, data.fingerprint);
}

function PREaddPeerSubmitSuccess(Root, PeerId) {
   var peerId = String(PeerId || "");
   MODELforcePoll("PREmodel");
   if (!PREaddAllowAccessChecked(Root)) {
      location.hash = PRErouteDetails(peerId);
      return;
   }
   PREaddSetFullAccess(peerId, function(AccessR) {
      if (AccessR && AccessR.success) {
         location.hash = PRErouteDetails(peerId);
         return;
      }
      PREsetAddPeerError(
         Root,
         ESChtmlEscape((AccessR && AccessR.error) ? AccessR.error : "Peer added, but failed to grant access.")
      );
   });
}

function PREaddPeerSubmitFail(Root, Response) {
   PREsetAddPeerError(
      Root,
      ESChtmlEscape((Response && Response.error) ? Response.error : "Failed to connect to peer")
   );
}

function PREaddValidateConnectionInputs(Host, Port) {
   return PREvalidateConnectionInputs(Host, Port, {
      hostRequiredMessage: "Address is required",
      invalidPortMessage: "Valid port number is required (1-65535)"
   });
}

function PREaddValidatePeerId(Id) {
   return PREvalidatePeerId(Id, {
      normalizeCase: false,
      requiredMessage: "ID is required",
      invalidMessage: "ID must be 16 characters (A–Z, 0–9)"
   });
}

function PREtoggleManual(Root) {
   Root.classList.toggle("PREmanualOpen");
   
   // If we open manual mode, hide the "Parsed Preview" to avoid duplicate info
   if (Root.classList.contains("PREmanualOpen")) {
       PREsetParsedInvitePreviewVisible(Root, false);
   } else {
       PREsetParsedInvitePreviewVisible(Root, Root.classList.contains("PREinviteDecoded"));
   }
   PRErefreshManualToggleLabel(Root);
}

function PRErefreshManualToggleLabel(Root) {
   var t = Root.querySelector(".PREmanualToggle");
   if (!t) return;
   
   if (Root.classList.contains("PREmanualOpen")) { 
      t.textContent = "Hide manual details"; 
      return; 
   }
   
   t.textContent = Root.classList.contains("PREinviteDecoded") ? "Edit all other details" : "Enter details manually";
}

function PREinviteHandle(Root) {
   var inviteEl = Root.querySelector(".PREinviteCodeInput");
   if (!inviteEl) return;
   var code = String(inviteEl.value || "").trim();
   
   if (!code) return PREinviteReset(Root);
   
   var decoded = INVinviteCodeTryDecode(code);
   if (!decoded) return PREinviteFail(Root, "Invalid invite code (checksum mismatch or bad format).");
   
   var err = PREinviteValidateDecoded(decoded);
   if (err) return PREinviteFail(Root, err);
   
   PREinviteApply(Root, decoded);
}

function PREinviteReset(Root) {
   Root.classList.remove("PREinviteDecoded");
   PREsetParsedInvitePreviewVisible(Root, false);
   PREhideParsedInvitePreview(Root);
   PREsetAddPeerError(Root, "");
   PRErefreshManualToggleLabel(Root);
}

function PREinviteValidateDecoded(decoded) {
   if (!decoded || !decoded.host || !decoded.port || !decoded.id || !decoded.fingerprint) return "Invite code is incomplete.";
   var err = PREaddValidateConnectionInputs(decoded.host, decoded.port) || PREaddValidatePeerId(decoded.id);
   return err ? ESChtmlEscape(err) : "";
}

function PREinviteFail(Root, msg) {
   Root.classList.remove("PREinviteDecoded");
   PREsetParsedInvitePreviewVisible(Root, false);
   PREhideParsedInvitePreview(Root);
   PREsetAddPeerError(Root, msg);
   PRErefreshManualToggleLabel(Root);
}

function PREinviteApply(Root, decoded) {
   PREpopulateManualFields(Root, decoded);
   Root.classList.add("PREinviteDecoded");
   PREshowParsedInvitePreview(Root, decoded);
   
   var selfWarn = PREaddPeerSelfWarning(decoded.host, decoded.port, decoded.id, decoded.fingerprint);
   PREsetAddPeerError(Root, selfWarn ? ESChtmlEscape(selfWarn) : "");
   PRErefreshManualToggleLabel(Root);
}

function PREpopulateManualFields(Root, decoded) {
   Root.querySelector(".PREaddHostInput").value = decoded.host;
   Root.querySelector(".PREaddPortInput").value = String(decoded.port);
   Root.querySelector(".PREaddIdInput").value = decoded.id;
   Root.querySelector(".PREaddFingerprintInput").value = decoded.fingerprint;
}

function PREshowParsedInvitePreview(Root, decoded) {
   var inlineHostInput = Root.querySelector(".PREparsedHostInput");
   if (inlineHostInput) inlineHostInput.value = decoded.host;

   PREsetPreviewField(Root, ".PREparsedPort", String(decoded.port));
   PREsetPreviewField(Root, ".PREparsedId", decoded.id);
   PREsetPreviewField(Root, ".PREparsedFp", decoded.fingerprint);
   
   var isManualClosed = !Root.classList.contains("PREmanualOpen");
   PREsetParsedInvitePreviewVisible(Root, isManualClosed);

   if (isManualClosed) {
      setTimeout(function() {
         if (inlineHostInput) {
            inlineHostInput.focus();
            inlineHostInput.select();
         }
      }, 10);
   }
}

function PREhideParsedInvitePreview(Root) {
   var inlineHostInput = Root.querySelector(".PREparsedHostInput");
   if (inlineHostInput) inlineHostInput.value = "";
   
   PREsetPreviewField(Root, ".PREparsedPort", "—", true);
   PREsetPreviewField(Root, ".PREparsedId", "—", true);
   PREsetPreviewField(Root, ".PREparsedFp", "—", true);
}

function PREsetPreviewField(Root, sel, text, clearTitle) {
   var el = Root.querySelector(sel);
   if (!el) return;
   el.textContent = text;
   el.title = clearTitle ? "" : text;
}

function PREsetParsedInvitePreviewVisible(Root, on) {
   var card = Root.querySelector(".PREparsedCard");
   if (!card) return;
   card.classList.toggle("PREhidden", !on);
}

function PREsetAddPeerError(Root, msg) {
   var ErrorMsg = Root.querySelector(".PREerrorMessage");
   if (!ErrorMsg) return;
   ErrorMsg.innerHTML = msg ? ('<div class="PREerror">' + msg + "</div>") : "";
}

function PREaddPeerSelfWarning(Host, Port, Id, Fingerprint) {
   var Self = PREaddGetSelf();
   if (!Self) return "";
   return PREpeerSelfWarning(Self, {
      host: Host,
      port: Port,
      id: Id,
      fingerprint: Fingerprint
   }, {
      selfFingerprintMessage: "That invite code is for this node. You can’t add yourself.",
      selfIdMessage: "That invite code is for this node. You can’t add yourself.",
      selfHostPortMessage: "That address is this node. You can’t add yourself."
   });
}

/* --- HTML Generators --- */

function PREaddAccessToggleText() {
   return PREcommandCenterMode()
      ? "Allow this peer to read data from this server"
      : "Allow access to read data from this server";
}

function PREaddAccessToggleHint() {
   return PREcommandCenterMode()
      ? "Leave off if this peer should only be monitored and not read data from this server."
      : "Enabled by default for authorized peers.";
}

function PREaddAccessToggleHtml() {
   if (!PREaddAllowAccessVisible()) return "";
   var checked = PREaddAllowAccessDefault() ? "checked" : "";
   return /*html*/`
   <div class="PREaddAccessToggle">
      <label class="PREaddAccessToggleLabel">
         <input type="checkbox" class="PREaddAllowAccessInput" ${checked}>
         <span>${PREaddAccessToggleText()}</span>
      </label>
      <div class="PREaddAccessToggleHint">${PREaddAccessToggleHint()}</div>
   </div>`;
}

function PREaddPeerModalHtml() {
   var actionLabel = PREaddActionLabel();
   var intro = PREcommandCenterMode()
      ? "Monitor a new instance. Adding a peer attempts to connect immediately."
      : "Grant an instance access to read this server's data. Authorizing a peer attempts to connect immediately.";
   var mutual = PREcommandCenterMode()
      ? "<br>Connections are mutual — the other Peer must also add you (share your invite code)."
      : "";
   return /*html*/`
   <div class="PREaddPeerModal">
      <div class="PREmodalIntro">
         ${intro}${mutual}
      </div>
      ${PREaddPeerInviteSectionHtml()}
      <div class="PREpreviewActions">
         <a class="PREmanualToggle" href="#">Enter details manually</a>
      </div>
      ${PREaddPeerManualSectionHtml()}
      ${PREaddAccessToggleHtml()}
      <div class="PREerrorMessage"></div>
      <div class="PREformActions">
         <div class="BUTTONstandard BUTTONdefault PREaddCancelBtn">Cancel</div>
         <div class="BUTTONstandard BUTTONaction PREaddPeerBtn">${actionLabel}</div>
      </div>
   </div>`;
}

function PREaddPeerInviteSectionHtml() {
   return /*html*/`
   <div class="PREformSection">
      <div class="PREsectionHeader">
         Invite Code <span class="PREpill">Recommended</span>
      </div>
      <div class="PREcard">
         <div class="PREcardHelper">Paste the invite code from the other Peer.</div>
         <input type="text" class="FORMinput PREinviteCodeInput" spellcheck="false" placeholder="Paste invite code here" autocomplete="off" autocapitalize="off"/>
         
         <div class="PREparsedCard PREhidden">
            <div class="PREparsedGrid">
               <div class="PREparsedKey">Address</div>
               <div class="PREparsedVal">
                  <input type="text" class="FORMinput PREparsedHostInput" spellcheck="false" autocomplete="off" autocapitalize="off">
               </div>
               
               <div class="PREformHintBox PREinlineHint">
                  <div class="PREhintIcon">
                     <svg viewBox="0 0 24 24"><path fill="currentColor" d="M11 7h2v2h-2zm0 4h2v6h-2zm1-9C6.48 2 2 6.48 2 12s4.48 10 10 10 10-4.48 10-10S17.52 2 12 2zm0 18c-4.41 0-8-3.59-8-8s3.59-8 8-8 8 3.59 8 8-3.59 8-8 8z"/></svg>
                  </div>
                  <div class="PREhintText">
                     Address may need to be changed depending on network/DNS.
                  </div>
               </div>

               <div class="PREparsedKey">Port</div>
               <div class="PREparsedVal PREparsedPort">—</div>
               <div class="PREparsedKey">ID</div>
               <div class="PREparsedVal PREparsedId">—</div>
               <div class="PREparsedKey">Fingerprint</div>
               <div class="PREparsedVal PREparsedFp">—</div>
            </div>
         </div>
      </div>
   </div>`;
}

function PREaddPeerManualSectionHtml() {
   return /*html*/`
   <div class="PREmanualSection">
      <div class="PREformSection">
         <div class="PREsectionHeader">Manual Connection Details</div>
         <div class="PREcard">
            <div class="PREformRowGrid">
               <div class="PREformLabel">Address</div>
               <input type="text" class="FORMinput PREaddHostInput" placeholder="e.g. 192.168.1.100" spellcheck="false">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Port</div>
               <input type="number" class="FORMinput PREaddPortInput" placeholder="e.g. 7552" min="1" max="65535">
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">ID</div>
               <input type="text" class="FORMinput PREmono PREaddIdInput" spellcheck="false" placeholder="e.g. SALPGVZLMEKL7VMD" autocomplete="off" autocapitalize="off"/>
            </div>
            <div class="PREformRowGrid">
               <div class="PREformLabel">Fingerprint</div>
               <input type="text" class="FORMinput PREmono PREaddFingerprintInput" spellcheck="false" placeholder="Paste fingerprint here" autocomplete="off" autocapitalize="off"/>
            </div>
         </div>
      </div>
   </div>`;
}
