/* PREactions.js */
/* ==========================================================================
   PRE Actions
   - Handles Peer Actions: Retry, Connect, Disconnect, Remove
   - "Clean Wires": Actions update Data, Data updates View. No direct DOM styling.
========================================================================== */

var PRE_CONN_MS = 700;

function PREpeerConnectButtonIdleLabel(Btn) {
   if (!Btn) return "Connect";
   if (Btn.classList && Btn.classList.contains('PREbtnRetryDisconnect')) return "Disconnect";
   if (Btn.classList && Btn.classList.contains('PREpeerToggleBtn')) {
      return Btn.classList.contains('PREpeerToggleConnected') ? "Disconnect" : "Connect";
   }
   return "Connect";
}

function PREpeerConnectButtonState(Btn, IsConnecting) {
   if (!Btn) return;
   var isPeerToggle = Btn.classList && Btn.classList.contains('PREpeerToggleBtn');
   if (IsConnecting) {
      var until = Date.now() + PRE_CONN_MS;
      Btn.textContent = "Connecting...";
      Btn.classList.add('PREbtnIsConnecting');
      if (isPeerToggle) Btn.classList.remove('PREbtnIsConnectingDim');
      else Btn.classList.add('PREbtnIsConnectingDim');
      Btn.preConnectingUntil = until;
      if (isPeerToggle) {
         Btn.classList.remove('PREpeerToggleConnected');
         Btn.classList.remove('PREpeerToggleDisconnected');
         Btn.classList.add('PREpeerToggleConnecting');
         Btn.classList.remove('BUTTONaction');
         Btn.classList.add('BUTTONdefault');
      }
      return;
   }
   Btn.textContent = PREpeerConnectButtonIdleLabel(Btn);
   Btn.classList.remove('PREbtnIsConnecting');
   Btn.classList.remove('PREbtnIsConnectingDim');
   Btn.preConnectingUntil = 0;
   if (isPeerToggle) {
      Btn.classList.remove('PREpeerToggleConnecting');
      if (Btn.classList.contains('PREpeerToggleConnected')) {
         Btn.classList.remove('BUTTONaction');
         Btn.classList.add('BUTTONdefault');
      } else {
         Btn.classList.remove('BUTTONdefault');
         Btn.classList.add('BUTTONaction');
         if (!Btn.classList.contains('PREpeerToggleDisconnected')) {
            Btn.classList.add('PREpeerToggleDisconnected');
         }
      }
   }
}

function PREpeerActionRetry(Peer, Btn) {
   PREpeerActionConnect(Peer, Btn);
}

function PREpeerFinalizeConnectAttempt(Context) {
   if (!Context || Context.done) return;
   Context.done = true;
   var btn = Context.btn;
   var elapsed = Date.now() - Context.startMs;
   var waitMs = btn ? Math.max(0, PRE_CONN_MS - elapsed) : 0;
   window.setTimeout(function() {
      PREpeerConnectButtonState(btn, false);
      MODELforcePoll('PREmodel');
      MODELforcePoll('PREpeerModel');
   }, waitMs);
}

function PREpeerActionConnectSuccess(R, Context) {
   if (!R || !R.success) console.warn("Connect failed:", R);
   PREpeerFinalizeConnectAttempt(Context);
}

function PREpeerActionConnectFailure(PeerId, Context) {
   console.warn("Connect request failed for peer:", PeerId);
   PREpeerFinalizeConnectAttempt(Context);
}

function PREpeerActionConnect(Peer, Btn) {
   if (!Peer || !Peer.id) {
      PREpeerConnectButtonState(Btn, false);
      return;
   }
   var context = {
      btn: Btn || null,
      startMs: Date.now(),
      done: false
   };
   PREpeerConnectButtonState(context.btn, true);
   APIcall('iguanas/connect', { id: Peer.id }, function(R) {
      PREpeerActionConnectSuccess(R, context);
   }, function() {
      PREpeerActionConnectFailure(Peer.id, context);
   });
}

function PREpeerActionDisconnect(Peer) {
   if (!Peer || !Peer.id) return;
   var name = ESChtmlEscape(String(Peer.name || Peer.id));
   var action = "Are you sure you want to disconnect from <b>" + name + "</b>?";
   CONFIRMaction(action, function() {
      APIcall('iguanas/disconnect', { id: Peer.id }, function() {
         MODELforcePoll('PREmodel');
         MODELforcePoll('PREpeerModel');
      });
   }, "DISCONNECT", null, { destructive: true });
}

function PREpeerActionRemove(Peer) {
   if (!Peer || !Peer.id) return;
   var name = ESChtmlEscape(String(Peer.name || Peer.id));
   var action = "Are you sure you want to delete <b>" + name + "</b>?";
   CONFIRMaction(action, function() {
      APIcall('iguanas/remove', { id: Peer.id }, function() {
         location.hash = PRErouteList();
         MODELforcePoll('PREmodel');
      });
   }, "DELETE PEER", null, { destructive: true });
}
