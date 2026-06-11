/* ==========================================================================
IGX1 Invite Code (condensed binary payload, base64url, robust)
Public shape: { host, port, id, fingerprint }

fingerprint (public) is expected to be:
   base64/base64url of ASCII-hex (like: "M0ZGNjMwQjhE...")

id (public) is expected to look like:
   "SALPGVZLMEKL7VMD"  (16 chars, A–Z / 0–9)

Payload format (bytes):
   [0]     hostLen uint8 (1..255)
   [...]   host UTF-8 bytes (hostLen)
   next2   port uint16 BE
   [..]    idLen uint8 (1..255)
   [...]   id UTF-8 bytes (idLen)
   [..]    fpLen uint8 (1..255)
   [...]   fpBytes raw bytes (decoded from fingerprint's hex)
   last2   crc16 uint16 BE (CRC16-CCITT of everything before crc16)

Notes:
- Prefix compare is case-insensitive; payload is case-sensitive
========================================================================== */

var INV_INVITE_PREFIX = "IGX1:";

/* ----------------------------- safe wrappers ----------------------------- */
function INVinviteCodeTryEncode(peerConfig){
   try{
      return INVinviteCodeEncode(peerConfig);
   }catch(err){
      // Keep logging centralized + consistent
      console.warn("Invite encode failed:", peerConfig, err);
      return null;
   }
}
function INVinviteCodeTryDecode(inviteCode){
   try{
      return INVinviteCodeDecode(inviteCode);
   }catch(err){
      // console.warn("Invite decode failed:", inviteCode, err);
      return null;
   }
}

function INVinviteCodeEncode(peerConfig){
   var cfg = INVpeerConfigNormalize(peerConfig);

   var hostBytes = INVutf8Encode(cfg.host);
   if(hostBytes.length < 1 || hostBytes.length > 255){
      throw new Error("host is too long");
   }

   var idBytes = INVutf8Encode(cfg.id);
   if(idBytes.length < 1 || idBytes.length > 255){
      throw new Error("id is too long");
   }

   var fpBytes = INVfingerprintDecodeBase64HexToBytes(cfg.fingerprint);
   if(fpBytes.length < 1 || fpBytes.length > 255){
      throw new Error("fingerprint is too long");
   }

   // hostLen(1) + host + port(2) + idLen(1) + id + fpLen(1) + fpBytes + crc16(2)
   var totalLen = 1 + hostBytes.length + 2 + 1 + idBytes.length + 1 + fpBytes.length + 2;
   var out = new Uint8Array(totalLen);
   var o = 0;

   out[o++] = hostBytes.length;
   out.set(hostBytes, o); o += hostBytes.length;

   // port uint16 BE
   out[o++] = (cfg.port >> 8) & 0xff;
   out[o++] = (cfg.port     ) & 0xff;

   // id
   out[o++] = idBytes.length;
   out.set(idBytes, o); o += idBytes.length;

   // fingerprint
   out[o++] = fpBytes.length;
   out.set(fpBytes, o); o += fpBytes.length;

   // crc16 over everything so far
   var crc = INVcrc16Ccitt(out.subarray(0, o));
   out[o++] = (crc >> 8) & 0xff;
   out[o++] = (crc     ) & 0xff;

   return INV_INVITE_PREFIX + INVbase64UrlEncodeBytes(out);
}

function INVinviteCodeDecode(inviteCode){
   if(typeof inviteCode !== "string"){
      throw new Error("Invalid invite code: not a string");
   }
   // Copy/paste resilience: ignore whitespace/newlines
   inviteCode = inviteCode.replace(/\s+/g, "");

   var head = inviteCode.slice(0, INV_INVITE_PREFIX.length);
   if(head.toUpperCase() !== INV_INVITE_PREFIX){
      throw new Error("Invalid invite code: bad prefix");
   }

   var b64u = inviteCode.slice(INV_INVITE_PREFIX.length);
   var bytes = INVbase64UrlDecodeToBytes(b64u);

   // Must have at least: hostLen(1) + port(2) + idLen(1) + fpLen(1) + crc16(2) = 7 bytes
   if(bytes.length < 7){
      throw new Error("Invalid invite code: payload too short");
   }

   // Verify CRC16 before parsing fields
   var dataLen = bytes.length - 2;
   var given = ((bytes[dataLen] << 8) | bytes[dataLen + 1]) & 0xFFFF;
   var calc = INVcrc16Ccitt(bytes.subarray(0, dataLen));
   if(given !== calc){
      throw new Error("Invalid invite code: checksum mismatch");
   }

   var o = 0;

   var hostLen = bytes[o++];
   if(hostLen < 1){
      throw new Error("Invalid invite code: bad host length");
   }
   if(dataLen < 1 + hostLen + 2 + 1 + 1){
      // need at least hostLen + port + idLen + fpLen bytes (not including their variable payloads)
      throw new Error("Invalid invite code: payload truncated");
   }

   var host = INVutf8Decode(bytes.subarray(o, o + hostLen));
   o += hostLen;

   var port = (bytes[o++] << 8) | bytes[o++];

   var idLen = bytes[o++];
   if(idLen < 1){
      throw new Error("Invalid invite code: bad id length");
   }
   if(o + idLen + 1 > dataLen){
      // need id bytes + fpLen byte
      throw new Error("Invalid invite code: payload truncated");
   }

   var id = INVutf8Decode(bytes.subarray(o, o + idLen));
   o += idLen;

   var fpLen = bytes[o++];
   if(fpLen < 1){
      throw new Error("Invalid invite code: bad fingerprint length");
   }
   if(o + fpLen !== dataLen){
      throw new Error("Invalid invite code: bad fingerprint length");
   }

   var fpBytes = bytes.subarray(o, o + fpLen);

   return INVpeerConfigNormalize({
      host: host,
      port: port,
      id: id,
      fingerprint: INVfingerprintEncodeBytesToBase64Hex(fpBytes)
   });
}

/* ----------------------------- fingerprint helpers ----------------------------- */
function INVfingerprintDecodeBase64HexToBytes(fpStr){
   // fingerprint string is base64/base64url of ASCII hex
   var decoded = INVbase64AnyDecodeToBytes(fpStr);
   if(!INVbytesAreAsciiHex(decoded) || (decoded.length % 2 !== 0)){
      throw new Error("fingerprint is not base64-of-hex");
   }
   var hexAscii = INVasciiBytesToString(decoded);
   return INVhexToBytes(hexAscii);
}
function INVfingerprintEncodeBytesToBase64Hex(fpBytes){
   var hex = INVbytesToHexUpper(fpBytes);
   var hexAsciiBytes = INVasciiStringToBytes(hex);
   return INVbase64EncodeBytes(hexAsciiBytes);
}

/* ----------------------------- normalize ----------------------------- */
function INVpeerConfigNormalize(peerConfig){
   if(!peerConfig || typeof peerConfig !== "object"){
      throw new Error("peerConfig must be an object");
   }

   var host = (peerConfig.host == null ? "" : String(peerConfig.host)).trim();
   var fingerprint = (peerConfig.fingerprint == null ? "" : String(peerConfig.fingerprint)).trim();

   // Accept either `id` or legacy `iguana` input, but output uses `id`
   var id = (peerConfig.id == null ? "" : String(peerConfig.id)).trim();
   if(!id && peerConfig.iguana != null){
      id = String(peerConfig.iguana).trim();
   }

   var portNum = (typeof peerConfig.port === "number") ? peerConfig.port : parseInt(String(peerConfig.port), 10);

   if(!host){
      throw new Error("host is required");
   }
   if(!Number.isFinite(portNum) || portNum < 1 || portNum > 65535){
      throw new Error("port must be an integer between 1 and 65535");
   }
   if(!id){
      throw new Error("id is required");
   }
   // "looks like SALPGVZLMEKL7VMD" => 16 chars, A-Z / 0-9
   if(!/^[A-Z0-9]{16}$/.test(id)){
      throw new Error("id must be 16 chars A-Z0-9");
   }
   if(!fingerprint){
      throw new Error("fingerprint is required");
   }

   return { host: host, port: portNum, id: id, fingerprint: fingerprint };
}

/* ----------------------------- base64url for bytes ----------------------------- */
function INVbase64UrlEncodeBytes(u8){
   var b64 = INVbase64EncodeBytes(u8);
   return b64.replace(/\+/g, "-").replace(/\//g, "_").replace(/=+$/g, "");
}
function INVbase64UrlDecodeToBytes(b64url){
   var b64 = String(b64url).replace(/-/g, "+").replace(/_/g, "/");
   while(b64.length % 4 !== 0){
      b64 += "=";
   }
   return INVbase64DecodeToBytes(b64);
}

/* ----------------------------- base64 (bytes) ----------------------------- */
function INVbase64AnyDecodeToBytes(b64Any){
   var s = String(b64Any).trim().replace(/\s+/g, "");
   if(s.indexOf("-") !== -1 || s.indexOf("_") !== -1){
      return INVbase64UrlDecodeToBytes(s);
   }
   return INVbase64DecodeToBytes(s);
}
function INVbase64EncodeBytes(u8){
   if(typeof Buffer !== "undefined"){
      return Buffer.from(u8).toString("base64");
   }
   if(typeof btoa === "function"){
      return btoa(INVbytesToBinaryString(u8));
   }
   throw new Error("No base64 encoder available in this environment");
}
function INVbase64DecodeToBytes(b64){
   if(typeof Buffer !== "undefined"){
      var buf = Buffer.from(String(b64), "base64");
      return new Uint8Array(buf.buffer, buf.byteOffset, buf.byteLength);
   }
   if(typeof atob === "function"){
      var bin = atob(String(b64));
      var out = new Uint8Array(bin.length);
      for(var i = 0; i < bin.length; i++){
         out[i] = bin.charCodeAt(i) & 0xff;
      }
      return out;
   }
   throw new Error("No base64 decoder available in this environment");
}
function INVbytesToBinaryString(u8){
   var s = "";
   for(var i = 0; i < u8.length; i++){
      s += String.fromCharCode(u8[i]);
   }
   return s;
}

/* ----------------------------- utf8 helpers ----------------------------- */
function INVutf8Encode(str){
   if(typeof TextEncoder !== "undefined"){
      return new TextEncoder().encode(String(str));
   }
   if(typeof Buffer !== "undefined"){
      var b = Buffer.from(String(str), "utf8");
      return new Uint8Array(b.buffer, b.byteOffset, b.byteLength);
   }
   var esc = unescape(encodeURIComponent(String(str)));
   var out = new Uint8Array(esc.length);
   for(var i = 0; i < esc.length; i++){
      out[i] = esc.charCodeAt(i) & 0xff;
   }
   return out;
}
function INVutf8Decode(u8){
   if(typeof TextDecoder !== "undefined"){
      return new TextDecoder("utf-8").decode(u8);
   }
   if(typeof Buffer !== "undefined"){
      return Buffer.from(u8).toString("utf8");
   }
   var s = "";
   for(var i = 0; i < u8.length; i++){
      s += String.fromCharCode(u8[i]);
   }
   return decodeURIComponent(escape(s));
}

/* ----------------------------- hex helpers ----------------------------- */
function INVbytesAreAsciiHex(u8){
   for(var i = 0; i < u8.length; i++){
      var c = u8[i];
      var isNum = (c >= 48 && c <= 57);
      var isA_F = (c >= 65 && c <= 70);
      var isa_f = (c >= 97 && c <= 102);
      if(!isNum && !isA_F && !isa_f){
         return false;
      }
   }
   return true;
}
function INVasciiBytesToString(u8){
   var s = "";
   for(var i = 0; i < u8.length; i++){
      s += String.fromCharCode(u8[i]);
   }
   return s;
}
function INVasciiStringToBytes(str){
   str = String(str);
   var out = new Uint8Array(str.length);
   for(var i = 0; i < str.length; i++){
      out[i] = str.charCodeAt(i) & 0xff;
   }
   return out;
}
function INVhexToBytes(hex){
   hex = String(hex).trim();
   if(hex.length % 2 !== 0){
      throw new Error("Invalid hex length");
   }
   var out = new Uint8Array(hex.length / 2);
   for(var i = 0; i < out.length; i++){
      var b = parseInt(hex.substr(i * 2, 2), 16);
      if(!Number.isFinite(b)){
         throw new Error("Invalid hex");
      }
      out[i] = b;
   }
   return out;
}
function INVbytesToHexUpper(u8){
   var hex = "";
   for(var i = 0; i < u8.length; i++){
      var s = u8[i].toString(16).toUpperCase();
      if(s.length === 1){ s = "0" + s; }
      hex += s;
   }
   return hex;
}

/* ----------------------------- CRC16-CCITT ----------------------------- */
function INVcrc16Ccitt(u8){
   // CRC-CCITT (0x1021), init 0xFFFF
   var crc = 0xFFFF;
   for(var i = 0; i < u8.length; i++){
      crc ^= (u8[i] & 0xFF) << 8;
      for(var k = 0; k < 8; k++){
         if(crc & 0x8000){
            crc = ((crc << 1) ^ 0x1021) & 0xFFFF;
         }else{
            crc = (crc << 1) & 0xFFFF;
         }
      }
   }
   return crc & 0xFFFF;
}

/* ----------------------------- example ----------------------------- */
// var INVinviteCodeExample = INVinviteCodeEncode({
//    host: "10.0.0.12",
//    port: 443,
//    id: "SALPGVZLMEKL7VMD",
//    fingerprint: "M0ZGNjMwQjhEMzJGMkYxRkZGMDc5NDM0QTA1QzJEODY="
// });
// var INVdecodedExample = INVinviteCodeDecode(INVinviteCodeExample);
