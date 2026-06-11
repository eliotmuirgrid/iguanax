/* ==========================================================================
   PRE Access Modal (Route: #command/access?peer=...)
   - Entry point: PREaccessScreen()
   - Logic: Dedicated model fetches peer list to verify/set access
========================================================================== */

var PRE_ACC_SAVE_MS = 250;

function PREaccessIsAuthorizedModalRoute(Page) {
   var page = String(Page || PAGEhash() || '');
   return (page === PRErouteAuthorizedPeersAccessPath());
}

function PREaccessFindPeer(List, PeerId) {
   var id = String(PeerId || '');
   if (!id || !Array.isArray(List)) return null;
   for (var i = 0; i < List.length; i++) {
      if (String(List[i].id || '') === id) return List[i];
   }
   return null;
}

function PREaccessRender() {
   var page = PAGEhash();
   if (PREcommandCenterMode(page)) return PREaccessRenderCommandRoute(page);
   PREaccessRenderAuthorizedRoute(page);
}

function PREaccessOpenModalIfNeeded(Peer) {
   if (!Peer) return;
   if (document.querySelector('.PREaccessModal')) return;
   PREshowAccessModal(Peer, PREaccessModalConfig());
}

function PREaccessRenderAuthorizedRoute(Page) {
   if (!PREaccessIsAuthorizedModalRoute(Page)) return;
   var list = PREaccessListGet();
   var peer = PREaccessFindPeer(list, PREpeerParam());
   PREaccessOpenModalIfNeeded(peer);
}

function PREaccessRenderCommandRoute(Page) {
   if (Page !== PRErouteCommandAccessPath()) return;
   var peer = PREaccessFindPeer(PREaccessListGet(), PREpeerParam());
   PREaccessOpenModalIfNeeded(peer);
}

/* --- View & Interaction --- */

function PREaccessModalRouteBack() {
   return PRErouteList();
}

function PREaccessSubmitError(Message) {
   SNCKsnackbar('error', String(Message || 'Failed to set access level.'));
}

function PREaccessModalSubmit(Root, Peer, Level) {
   PREaccessSubmit(Root, Peer, Level, {
      setSaveState: PREaccessSetSaveState,
      normalizeLevel: PREaccessNormalizeLevel,
      updateSaveButton: PREaccessUpdateSaveButton,
      routeBack: PREaccessModalRouteBack,
      onError: PREaccessSubmitError,
      pollModels: ['PREaccessModel', 'PREmodel'],
      saveFeedbackMs: PRE_ACC_SAVE_MS,
      apiPath: 'iguanas/access/set'
   });
}

function PREaccessModalConfig() {
   return {
      title: 'Access Control',
      className: 'PREpopout',
      width: '640px',
      rootSelector: '.PREaccessModal',
      saveButtonSelector: '.PREbtnSave',
      cancelButtonSelector: '.PREbtnCancel',
      radioSelector: 'input[name="access_level"]',
      radioCheckedSelector: 'input[name="access_level"]:checked',
      modalHtml: PREaccessModalHtml,
      routeBack: PREaccessModalRouteBack,
      normalizeLevel: PREaccessNormalizeLevel,
      hasPendingChange: PREaccessHasPendingChange,
      submit: PREaccessModalSubmit,
      updateHelper: PREaccessUpdateHelper,
      updateSaveButton: PREaccessUpdateSaveButton
   };
}

function PREaccessNormalizeLevel(Level) {
   return (String(Level || '').toLowerCase() === 'full') ? 'full' : 'none';
}

function PREaccessSelectedLevel(Root) {
   var selected = Root.querySelector('input[name="access_level"]:checked');
   return selected ? PREaccessNormalizeLevel(selected.value) : 'none';
}

function PREaccessHasPendingChange(Root) {
   return PREaccessSelectedLevel(Root) !== PREaccessNormalizeLevel(PREaccessInitialLevelGet());
}

function PREaccessSetSaveState(Root, State) {
   var Btn = Root.querySelector('.PREbtnSave');
   if (!Btn) return;

   if (State === 'saving') {
      Btn.textContent = 'Saving...';
      Btn.classList.add('BUTTONdisabled');
      Btn.style.pointerEvents = 'none';
      Btn.setAttribute('aria-disabled', 'true');
      return;
   }

   if (State === 'saved') {
      Btn.textContent = 'Saved';
      Btn.classList.add('BUTTONdisabled');
      Btn.style.pointerEvents = 'none';
      Btn.setAttribute('aria-disabled', 'true');
      return;
   }

   if (State === 'dirty') {
      Btn.textContent = 'Save Changes';
      Btn.classList.remove('BUTTONdisabled');
      Btn.style.pointerEvents = '';
      Btn.setAttribute('aria-disabled', 'false');
      return;
   }

   Btn.textContent = 'Save Changes';
   Btn.classList.add('BUTTONdisabled');
   Btn.style.pointerEvents = 'none';
   Btn.setAttribute('aria-disabled', 'true');
}

function PREaccessUpdateSaveButton(Root) {
   if (PREaccessSavingGet()) return;
   PREaccessSetSaveState(Root, PREaccessHasPendingChange(Root) ? 'dirty' : 'disabled');
}

function PREaccessUpdateHelper(Root) {
   var selected = Root.querySelector('input[name="access_level"]:checked');

   var cards = Root.querySelectorAll('.PREaccessCard');
   for (var i = 0; i < cards.length; i++) cards[i].classList.remove('PREselected');
   if (selected) selected.closest('.PREaccessCard').classList.add('PREselected');
}

/* --- Authorized Mode: Add/Edit Peer + Access --- */

function PREaccessAuthorizedCanSubmit(Root) {
   if (PREaccessSavingGet()) return false;
   return PREaccessAuthorizedHasPendingChange(Root);
}

function PREaccessAuthorizedOnInput(Root, IsAdd, Event) {
   if (Event.target && Event.target.classList && Event.target.classList.contains('PREaccessCredentialsInput')) {
      PREaccessAuthorizedTryInviteDefaults(Root, IsAdd);
   }
   PREaccessAuthorizedSetError(Root, '');
   PREaccessUpdateHelper(Root);
   PREaccessAuthorizedUpdateSaveButton(Root);
}

function PREaccessAuthorizedOnChange(Root) {
   PREaccessAuthorizedSetError(Root, '');
   PREaccessUpdateHelper(Root);
   PREaccessAuthorizedUpdateSaveButton(Root);
}

function PREaccessAuthorizedOnKeydown(Root, Peer, Event) {
   if (Event.key !== 'Enter') return;
   if (!Event.target.closest('.PREaccessHostInput') &&
       !Event.target.closest('.PREaccessPortInput') &&
       !Event.target.closest('.PREaccessCredentialsInput')) {
      return;
   }
   Event.preventDefault();
   if (!PREaccessAuthorizedCanSubmit(Root)) return;
   PREaccessAuthorizedSubmit(Root, Peer);
}

function PREaccessAuthorizedBind(Root, Peer, IsAdd) {
   PREaccessSavingSet(false);
   PREaccessAuthorizedInitialStateSet(PREaccessAuthorizedSnapshot(Root));

   var SaveBtn = Root.querySelector('.PREbtnSave');
   if (SaveBtn) {
      SaveBtn.onclick = function() {
         if (!PREaccessAuthorizedCanSubmit(Root)) return;
         PREaccessAuthorizedSubmit(Root, Peer);
      };
   }

   var CancelBtn = Root.querySelector('.PREbtnCancel');
   if (CancelBtn) {
      CancelBtn.onclick = function() { location.hash = PRErouteList(); };
   }

   Root.addEventListener('input', function(e) { PREaccessAuthorizedOnInput(Root, IsAdd, e); });
   Root.addEventListener('change', function() { PREaccessAuthorizedOnChange(Root); });
   Root.addEventListener('keydown', function(e) { PREaccessAuthorizedOnKeydown(Root, Peer, e); });

   PREaccessUpdateHelper(Root);
   PREaccessAuthorizedUpdateSaveButton(Root);
}

function PREaccessAuthorizedSnapshot(Root) {
   return JSON.stringify({
      host: PREfieldValue(Root, '.PREaccessHostInput').toLowerCase(),
      port: PREfieldValue(Root, '.PREaccessPortInput'),
      credentials: PREfieldValue(Root, '.PREaccessCredentialsInput'),
      access_level: PREaccessSelectedLevel(Root)
   });
}

function PREaccessAuthorizedHasPendingChange(Root) {
   return PREaccessAuthorizedSnapshot(Root) !== PREaccessAuthorizedInitialStateGet();
}

function PREaccessAuthorizedUpdateSaveButton(Root) {
   if (PREaccessSavingGet()) return;
   PREaccessSetSaveState(Root, PREaccessAuthorizedHasPendingChange(Root) ? 'dirty' : 'disabled');
}

function PREaccessAuthorizedSetError(Root, Message) {
   var ErrorEl = Root.querySelector('.PREaccessError');
   if (!ErrorEl) return;
   if (!Message) {
      ErrorEl.innerHTML = '';
      return;
   }
   ErrorEl.innerHTML = '<div class="PREerror">' + ESChtmlEscape(Message) + '</div>';
}

function PREaccessAuthorizedTryInviteDefaults(Root, IsAdd) {
   if (!IsAdd) return;
   var credentials = PREfieldValue(Root, '.PREaccessCredentialsInput');
   if (!credentials) return;
   var decoded = INVinviteCodeTryDecode(credentials);
   if (!decoded) return;

   var HostInput = Root.querySelector('.PREaccessHostInput');
   var PortInput = Root.querySelector('.PREaccessPortInput');

   if (HostInput && !String(HostInput.value || '').trim()) HostInput.value = decoded.host;
   if (PortInput && !String(PortInput.value || '').trim()) PortInput.value = String(decoded.port);
}

function PREaccessValidateConnectionInputs(Host, Port) {
   return PREvalidateConnectionInputs(Host, Port, {
      hostRequiredMessage: 'Host/IP is required.',
      invalidPortMessage: 'Valid port is required (1-65535).'
   });
}

function PREaccessValidatePeerId(Id) {
   return PREvalidatePeerId(Id, {
      normalizeCase: true,
      requiredMessage: 'Peer ID is required in credentials.',
      invalidMessage: 'Peer ID must be 16 characters (A-Z, 0-9).'
   });
}

function PREaccessAuthorizedParseCredentials(Credentials, ExistingPeerId) {
   return PREparsePeerCredentials(Credentials, ExistingPeerId, {
      requiredMessage: 'Credentials are required.',
      newPeerFormatMessage: 'For new peers, credentials must be an invite code or ID:FINGERPRINT.',
      decodeInvite: INVinviteCodeTryDecode
   });
}

function PREaccessAuthorizedSelfWarning(Host, Port, Id, Fingerprint) {
   var Self = PREpeerGridLocalGet() || {};
   return PREpeerSelfWarning(Self, {
      host: Host,
      port: Port,
      id: Id,
      fingerprint: Fingerprint
   }, {
      selfFingerprintMessage: 'You cannot authorize this instance as its own peer.',
      selfIdMessage: 'You cannot authorize this instance as its own peer.',
      selfHostPortMessage: 'You cannot authorize this instance as its own peer.'
   });
}

function PREaccessAuthorizedBuildPayload(Root, Peer) {
   var host = PREfieldValue(Root, '.PREaccessHostInput');
   var port = PREfieldValue(Root, '.PREaccessPortInput');
   var credentials = PREfieldValue(Root, '.PREaccessCredentialsInput');
   var accessLevel = PREaccessSelectedLevel(Root);

   var existingId = PREnormalizeId(Peer && Peer.id);
   var parsed = PREaccessAuthorizedParseCredentials(credentials, existingId);
   if (parsed.error) return { error: parsed.error };

   if (!host && parsed.host) host = String(parsed.host || '').trim();
   if (!port && parsed.port) port = String(parsed.port || '').trim();

   var connError = PREaccessValidateConnectionInputs(host, port);
   if (connError) return { error: connError };

   var peerId = existingId || PREnormalizeId(parsed.id);
   if (existingId && peerId !== existingId) {
      return { error: 'Credentials ID must match this peer ID.' };
   }

   var idError = PREaccessValidatePeerId(peerId);
   if (idError) return { error: idError };

   var fingerprint = PREnormalizeFingerprint(parsed.fingerprint);
   if (!fingerprint) return { error: 'Credentials are required.' };

   var selfWarn = PREaccessAuthorizedSelfWarning(host, port, peerId, fingerprint);
   if (selfWarn) return { error: selfWarn };

   return {
      data: {
         id: peerId,
         host: host,
         port: String(parseInt(port, 10)),
         fingerprint: fingerprint,
         access_level: PREaccessNormalizeLevel(accessLevel)
      }
   };
}

function PREaccessAuthorizedConnectionChanged(Peer, Next) {
   if (!Peer || !Peer.id) return true;
   if (PREnormalizeHost(Peer.host) !== PREnormalizeHost(Next.host)) return true;
   if (PREnormalizePort(Peer.port) !== PREnormalizePort(Next.port)) return true;
   if (PREnormalizeFingerprint(Peer.fingerprint) !== PREnormalizeFingerprint(Next.fingerprint)) return true;
   return false;
}

function PREaccessAuthorizedSubmitFail(Root, Message) {
   PREaccessSavingSet(false);
   PREaccessAuthorizedSetError(Root, Message || 'Unable to save peer settings.');
   PREaccessAuthorizedUpdateSaveButton(Root);
}

function PREaccessAuthorizedSubmitFinish(Root) {
   PREaccessAuthorizedInitialStateSet(PREaccessAuthorizedSnapshot(Root));
   PREaccessSetSaveState(Root, 'saved');
   MODELforcePoll('PREaccessModel');
   MODELforcePoll('PREmodel');
   setTimeout(function() {
      location.hash = PRErouteList();
   }, PRE_ACC_SAVE_MS);
}

function PREaccessAuthorizedSubmitSetAccessOrFinish(Root, Data, ShouldSetAccess) {
   if (!ShouldSetAccess) return PREaccessAuthorizedSubmitFinish(Root);
   PREaccessAuthorizedSetAccess(Data, Root);
}

function PREaccessAuthorizedAddPeer(Data, Root, ShouldSetAccess) {
   APIcall('iguanas/add', {
      id: Data.id,
      host: Data.host,
      port: Data.port,
      fingerprint: Data.fingerprint
   }, function(R) {
      if (R && R.success) return PREaccessAuthorizedSubmitSetAccessOrFinish(Root, Data, ShouldSetAccess);
      return PREaccessAuthorizedSubmitFail(Root, (R && R.error) ? R.error : 'Failed to authorize peer.');
   });
}

function PREaccessAuthorizedDisconnectPeer(PeerId, Root, Data, ShouldSetAccess) {
   APIcall(
      'iguanas/disconnect',
      { id: PeerId },
      function() { PREaccessAuthorizedAddPeer(Data, Root, ShouldSetAccess); },
      function() { PREaccessAuthorizedAddPeer(Data, Root, ShouldSetAccess); }
   );
}

function PREaccessAuthorizedSetAccess(Data, Root) {
   APIcall('iguanas/access/set', { id: Data.id, access_level: Data.access_level }, function(R) {
      if (R && R.success) return PREaccessAuthorizedSubmitFinish(Root);
      return PREaccessAuthorizedSubmitFail(Root, (R && R.error) ? R.error : 'Failed to update access level.');
   });
}

function PREaccessAuthorizedChangeFlags(Peer, Data) {
   var hasPeer = !!(Peer && Peer.id);
   var connectionChanged = !hasPeer || PREaccessAuthorizedConnectionChanged(Peer, Data);
   var desiredAccess = PREaccessNormalizeLevel(Data.access_level);
   var accessChanged = hasPeer
      ? (desiredAccess !== PREaccessNormalizeLevel(Peer.access_level))
      : (desiredAccess === 'full');
   return {
      hasPeer: hasPeer,
      connectionChanged: connectionChanged,
      desiredAccess: desiredAccess,
      accessChanged: accessChanged
   };
}

function PREaccessAuthorizedShouldSetAccess(Flags) {
   if (!Flags.hasPeer) return (Flags.desiredAccess === 'full');
   if (Flags.connectionChanged) return (Flags.desiredAccess === 'full');
   return Flags.accessChanged;
}

function PREaccessAuthorizedSubmit(Root, Peer) {
   if (PREaccessSavingGet()) return;

   PREaccessAuthorizedSetError(Root, '');
   var built = PREaccessAuthorizedBuildPayload(Root, Peer);
   if (built.error) {
      PREaccessAuthorizedSetError(Root, built.error);
      PREaccessAuthorizedUpdateSaveButton(Root);
      return;
   }

   var Data = built.data;
   var flags = PREaccessAuthorizedChangeFlags(Peer, Data);
   if (!flags.connectionChanged && !flags.accessChanged) {
      PREaccessAuthorizedUpdateSaveButton(Root);
      return;
   }
   var shouldSetAccess = PREaccessAuthorizedShouldSetAccess(flags);

   PREaccessSavingSet(true);
   PREaccessSetSaveState(Root, 'saving');

   if (!flags.hasPeer) {
      PREaccessAuthorizedAddPeer(Data, Root, shouldSetAccess);
      return;
   }

   if (!flags.connectionChanged) {
      PREaccessAuthorizedSubmitSetAccessOrFinish(Root, Data, shouldSetAccess);
      return;
   }

   PREaccessAuthorizedDisconnectPeer(Data.id, Root, Data, shouldSetAccess);
}

/* --- HTML Generation --- */

function PREaccessLevelCardHtml(IsSelected, Value, Title, Description) {
   var checked = IsSelected ? "checked" : "";
   var selectedClass = IsSelected ? "PREselected" : "";
   return /*html*/`
            <label class="PREaccessCard ${selectedClass}">
               <div class="PREradioInput">
                  <input type="radio" name="access_level" value="${Value}" ${checked}>
               </div>
               <div class="PREradioText">
                  <div class="PREradioTitle">${Title}</div>
                  <div class="PREradioDesc">${Description}</div>
               </div>
            </label>`;
}

function PREaccessModalActionsHtml() {
   return /*html*/`
      <div class="PREaccessActions">
         <div class="BUTTONstandard BUTTONdefault PREbtnCancel">Cancel</div>
         <div class="BUTTONstandard BUTTONaction PREbtnSave">Save Changes</div>
      </div>`;
}

function PREaccessModalHeaderHtml(Name, Meta) {
   return /*html*/`
      <div class="PREaccessHeader">
         <div class="PREaccessName">${Name}</div>
         <div class="PREaccessMeta">${Meta}</div>
      </div>
      <div class="PREaccessDivider"></div>`;
}

function PREaccessModalHtml(Peer) {
   var current = (Peer.access_level || 'none').toLowerCase();
   var isFull = (current === 'full');

   var name = ESChtmlEscape(Peer.name || Peer.id || 'Unknown Peer');
   var host = ESChtmlEscape(Peer.host || 'Unknown Host');
   var id = ESChtmlEscape(Peer.id || 'Unknown ID');
   var meta = host + ' | ID: ' + id;

   return /*html*/`
   <div class="PREaccessModal">
      ${PREaccessModalHeaderHtml(name, meta)}

      <div class="PREaccessControls">
         <div class="PREaccessLabel">Read Access</div>
         <div class="PREaccessHint">Choose whether this peer can read data from this server.</div>

         <div class="PREaccessRadioGroup">
            ${PREaccessLevelCardHtml(!isFull, "none", "Blocked", "This peer cannot pull data from this server.")}
            ${PREaccessLevelCardHtml(isFull, "full", "Allowed", "This peer can pull data from this server.")}
         </div>
      </div>

      ${PREaccessModalActionsHtml()}
   </div>`;
}

function PREaccessAuthorizedModalContext(Peer, IsAdd) {
   var hasPeer = !!(Peer && Peer.id);
   var current = hasPeer ? PREaccessNormalizeLevel(Peer.access_level) : 'full';
   var title = hasPeer
      ? ESChtmlEscape(Peer.name || Peer.id || 'Authorized Peer')
      : 'Authorize a New Peer';
   var meta = hasPeer
      ? ('Peer ID: ' + ESChtmlEscape(Peer.id || ''))
      : 'Enter connection details and choose read access.';
   return {
      hasPeer: hasPeer,
      isFull: (current === "full"),
      title: title,
      meta: meta,
      host: ESChtmlEscape(hasPeer ? String(Peer.host || '') : ''),
      port: ESChtmlEscape(hasPeer ? String(Peer.port || '') : ''),
      credentials: ESChtmlEscape(hasPeer ? String(Peer.fingerprint || '') : ''),
      credentialsHelp: IsAdd
         ? 'Use an invite code, or enter ID:FINGERPRINT.'
         : 'Use a fingerprint or invite code. Peer ID remains fixed.'
   };
}

function PREaccessAuthorizedConnectionSectionHtml(Context) {
   return /*html*/`
         <div class="PREaccessSection">
            <div class="PREaccessLabel">Connection Details</div>
            <div class="PREaccessFieldGrid">
               <label class="PREaccessField">
                  <span class="PREaccessFieldLabel">Host / IP</span>
                  <input type="text" class="FORMinput PREaccessHostInput" value="${Context.host}" placeholder="e.g. 192.168.1.100" spellcheck="false" autocomplete="off">
               </label>
               <label class="PREaccessField">
                  <span class="PREaccessFieldLabel">Port</span>
                  <input type="number" class="FORMinput PREaccessPortInput" value="${Context.port}" placeholder="e.g. 7552" min="1" max="65535">
               </label>
               <label class="PREaccessField PREaccessFieldFull">
                  <span class="PREaccessFieldLabel">Credentials</span>
                  <input type="text" class="FORMinput PREmono PREaccessCredentialsInput" value="${Context.credentials}" placeholder="Invite code or ID:FINGERPRINT" spellcheck="false" autocomplete="off" autocapitalize="off">
               </label>
            </div>
            <div class="PREaccessHint PREaccessHintTight">${Context.credentialsHelp}</div>
         </div>`;
}

function PREaccessAuthorizedAccessSectionHtml(IsFull) {
   return /*html*/`
         <div class="PREaccessSection">
            <div class="PREaccessLabel">Access Level</div>
            <div class="PREaccessRadioGroup">
               ${PREaccessLevelCardHtml(!IsFull, "none", "Blocked", "Prevent this peer from pulling metrics.")}
               ${PREaccessLevelCardHtml(IsFull, "full", "Allowed", "Allow this peer to pull metrics from this server.")}
            </div>
         </div>`;
}

function PREaccessAuthorizedModalHtml(Peer, IsAdd) {
   var ctx = PREaccessAuthorizedModalContext(Peer, IsAdd);

   return /*html*/`
   <div class="PREaccessModal PREaccessModalAuthorized">
      ${PREaccessModalHeaderHtml(ctx.title, ctx.meta)}

      <div class="PREaccessControls">
         ${PREaccessAuthorizedConnectionSectionHtml(ctx)}
         ${PREaccessAuthorizedAccessSectionHtml(ctx.isFull)}
      </div>

      <div class="PREaccessError"></div>

      ${PREaccessModalActionsHtml()}

   </div>`;
}
