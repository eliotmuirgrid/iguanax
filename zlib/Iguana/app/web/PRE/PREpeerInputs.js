/* ==========================================================================
   PRE Peer Inputs
   - Shared parsing, normalization, and validation for peer forms
========================================================================== */

function PREnormalizeHost(Host) {
   return String(Host == null ? "" : Host).trim().toLowerCase();
}

function PREnormalizePort(Port) {
   return String(Port == null ? "" : Port).trim();
}

function PREnormalizeFingerprint(Fingerprint) {
   return String(Fingerprint == null ? "" : Fingerprint).trim();
}

function PREnormalizeId(Id) {
   return String(Id == null ? "" : Id).trim().toUpperCase();
}

function PREfieldValue(Root, Selector) {
   var input = Root ? Root.querySelector(Selector) : null;
   return input ? String(input.value || "").trim() : "";
}

function PREvalidateConnectionInputs(Host, Port, Options) {
   var opts = Options || {};
   if (!String(Host || "").trim()) {
      return String(opts.hostRequiredMessage || "Host/IP is required.");
   }
   var p = parseInt(String(Port), 10);
   var min = Number.isFinite(opts.minPort) ? opts.minPort : 1;
   var max = Number.isFinite(opts.maxPort) ? opts.maxPort : 65535;
   if (!p || p < min || p > max) {
      return String(opts.invalidPortMessage || "Valid port is required (1-65535).");
   }
   return "";
}

function PREvalidatePeerId(Id, Options) {
   var opts = Options || {};
   var raw = String(Id == null ? "" : Id).trim();
   var value = opts.normalizeCase ? PREnormalizeId(raw) : raw;
   if (!value) return String(opts.requiredMessage || "Peer ID is required.");
   if (!/^[A-Z0-9]{16}$/.test(value)) {
      return String(opts.invalidMessage || "Peer ID must be 16 characters (A-Z, 0-9).");
   }
   return "";
}

function PREparsePeerCredentials(Credentials, ExistingPeerId, Options) {
   var opts = Options;
   var raw = String(Credentials || "").trim();
   if (!raw) return { error: String(opts.requiredMessage || "Credentials are required.") };

   var decoded = opts.decodeInvite(raw);
   if (decoded) {
      return {
         id: PREnormalizeId(decoded.id),
         fingerprint: PREnormalizeFingerprint(decoded.fingerprint),
         host: String(decoded.host || "").trim(),
         port: String(decoded.port || "").trim()
      };
   }

   var pair = raw.match(/^([A-Za-z0-9]{16})\s*[:|]\s*(.+)$/);
   if (pair) {
      return {
         id: PREnormalizeId(pair[1]),
         fingerprint: PREnormalizeFingerprint(pair[2])
      };
   }

   if (ExistingPeerId) {
      return {
         id: PREnormalizeId(ExistingPeerId),
         fingerprint: PREnormalizeFingerprint(raw)
      };
   }

   return {
      error: String(opts.newPeerFormatMessage || "For new peers, credentials must be an invite code or ID:FINGERPRINT.")
   };
}

function PREpeerSelfWarning(SelfPeer, NextPeer, Messages) {
   var self = SelfPeer || {};
   var next = NextPeer || {};
   var msg = Messages || {};

   var sh = PREnormalizeHost(self.host);
   var sp = PREnormalizePort(self.port);
   var sf = PREnormalizeFingerprint(self.fingerprint);
   var si = PREnormalizeId(self.id);

   var h = PREnormalizeHost(next.host);
   var p = PREnormalizePort(next.port);
   var f = PREnormalizeFingerprint(next.fingerprint);
   var i = PREnormalizeId(next.id);

   if (sf && f && f === sf) {
      return String(msg.selfFingerprintMessage || "You cannot authorize this instance as its own peer.");
   }
   if (si && i && i === si) {
      return String(msg.selfIdMessage || "You cannot authorize this instance as its own peer.");
   }
   if (sh && sp && h && p && h === sh && p === sp) {
      return String(msg.selfHostPortMessage || "You cannot authorize this instance as its own peer.");
   }
   return "";
}
