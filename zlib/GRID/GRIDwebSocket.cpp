//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDwebSocket.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLendian.h>
#include <COL/COLlog.h>
#include <COL/COLminimumInclude.h>
#include <GRID/GRIDconnectionMonitor.h>
#include <GRID/GRIDframe.h>
#include <GRID/GRIDserialize.h>
#include <GRID/GRIDutils.h>
#include <GRID/GRIDwebSocket.h>
#include <GRIDI/GRIDIhttp.h>
#include <GRIDS/GRIDSwebSocketAuth.h>
#include <IWEB/IWEBconfig.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKerror.h>
#include <SCKS/SCKSsslConnection.h>
#include <SCKS/SCKSsslContext.h>
#include <openssl/rand.h>
COL_LOG_MODULE;

GRIDwebSocket::GRIDwebSocket(GRIDnodeInfo& NodeInfo, const COLstring& Fingerprint, SCKloop& Loop, const bool IsServer, SCKconnection* pConnection)
    : m_NodeInfo(NodeInfo),
      m_Loop(Loop),
      m_Monitor(Loop),
      m_Connected(IsServer),
      m_Mode(IsServer ? GRIDWS_MODE_SERVER : GRIDWS_MODE_CLIENT) {
   COL_METHOD(GRIDwebSocket::GRIDwebSocket);
   GRIDinitCRC32();

   m_IsSecure = NodeInfo.m_pWebConfig->useHttps();
   m_pAuth    = new GRIDSwebSocketAuth(m_Mode, m_NodeInfo, Fingerprint);
   m_pAuth->setOnAuthenticated(COLnewClosure0(this, &GRIDwebSocket::onAuthenticationComplete));

   SCKconnection* Conn = nullptr;
   if(IsServer) {
      Conn = pConnection;
      if(m_IsSecure != pConnection->sslEnabled()) {
         COL_TRC("Expected SSL connection for HTTPS server mode but got plain connection");
         pConnection->close();
         return;
      }
   } else {
      Conn = m_IsSecure ? new SCKSsslConnection(NodeInfo.m_pContext, m_Loop) : new SCKconnection(m_Loop);
   }

   if(Conn) {
      m_ConnectionId    = Conn->m_Id;
      Conn->m_pOnData   = COLnewClosure2(this, &GRIDwebSocket::onData);
      m_pChainedOnClose = Conn->m_pOnClose;
      Conn->m_pOnClose  = COLnewClosure1(this, &GRIDwebSocket::onClose);
      if(!IsServer) { Conn->m_pOnConnect = COLnewClosure1(this, &GRIDwebSocket::performHandshake); }
   }
}

GRIDwebSocket::~GRIDwebSocket() {
   COL_METHOD(GRIDwebSocket::~GRIDwebSocket);
   COL_TRC("Destroying WebSocket, connected=" << m_Connected);
   m_pOnClose   = nullptr;
   m_pOnConnect = nullptr;
   close();
   if(m_pAuth) {
      delete m_pAuth;
      m_pAuth = nullptr;
   }
}

SCKconnection* GRIDwebSocket::getSocket() const {
   if(m_ConnectionId == 0) return nullptr;
   auto i = m_Loop.m_ConnectionMap.find(m_ConnectionId);
   return i ? i->second : nullptr;
}

void GRIDwebSocket::connect(const COLstring& Address, const int Port) const {
   COL_METHOD(GRIDwebSocket::connect);
   auto* pConnection = getSocket();
   if(pConnection) { pConnection->connect(Address, Port); }
   else            { COL_TRC("Connection pointer is null."); }
}

void GRIDwebSocket::sendJson(const COLstring& JsonString) {
   COL_METHOD(GRIDwebSocket::sendJson);
   const COLstring Frame = GRIDbuildJsonFrame(JsonString);
   sendFrame(Frame.c_str(), Frame.size());
}

void GRIDwebSocket::sendChallenge() {
   COL_METHOD(GRIDwebSocket::sendChallenge);
   m_Connected           = true;
   m_WsHandshakeComplete = true;
   startProtocol();
}

void GRIDwebSocket::close() {
   COL_METHOD(GRIDwebSocket::close);
   COL_TRC("Closing WebSocket, connected=" << m_Connected << " handshakeComplete=" << m_WsHandshakeComplete);
   if(!m_Connected && !m_WsHandshakeComplete) {
      COL_TRC("Already closed, skipping");
      return;
   }

   resetMembers();

   SCKconnection* pConn = getSocket();
   if(!pConn) {
      COL_TRC("No underlying connection to close (already cleaned up)");
      m_ConnectionId = 0;
      return;
   }

   pConn->m_pOnData  = nullptr; // Remove callbacks to prevent re-entry during close
   pConn->m_pOnClose = nullptr;

   COL_TRC("Initiating SCK close sequence, id=" << m_ConnectionId);
   pConn->hardClose();
   m_ConnectionId = 0;
   captureLastError();
   if(m_pOnClose.get()) {
      COL_TRC("Notifying onClose handler from close()");
      m_pOnClose->run();
   }
}

void GRIDwebSocket::onData(COLuint64 ConnectionId, COLstring* pData) {
   COL_METHOD(GRIDwebSocket::onData);
   COL_TRC("Received " << pData->size() << " bytes, buffer before=" << m_ReceiveBuffer.size());
   m_ReceiveBuffer.append(*pData);
   pData->clear();
   m_WsHandshakeComplete ? processWebSocketFrames() : processHandshake();
}

void GRIDwebSocket::onClose(COLuint64 ConnectionId) {
   COL_METHOD(GRIDwebSocket::onClose);
   COL_TRC("onClose called, connected=" << m_Connected << " connectionId=" << ConnectionId);

   // Prevent re-entry - if already closed, just notify chained callback
   if(!m_Connected && !m_WsHandshakeComplete) {
      COL_TRC("Already closed, notifying chained callback only");
      if(m_pChainedOnClose.get()) { m_pChainedOnClose->run(ConnectionId); }
      return;
   }

   // Save callbacks locally before any cleanup that might trigger deletion
   auto pChainedCallback = m_pChainedOnClose;
   auto pOnCloseCallback = m_pOnClose;

   m_pOnClose        = nullptr;
   m_pChainedOnClose = nullptr;
   m_ConnectionId    = 0;

   resetMembers();
   captureLastError();

   if(pOnCloseCallback.get()) {
      COL_TRC("Running onClose callback");
      pOnCloseCallback->run();
   }

   if(pChainedCallback.get()) {
      COL_TRC("Running chained onClose callback");
      pChainedCallback->run(ConnectionId);
   }
}

void GRIDwebSocket::performHandshake(COLuint64 ConnectionId) {
   COL_METHOD(GRIDwebSocket::performHandshake);
   m_Connected = true;
   sendData(buildHttpUpgradeRequest());
   if(m_pOnConnect.get()) { m_pOnConnect->run(); }
}

void GRIDwebSocket::processHandshake() {
   COL_METHOD(GRIDwebSocket::processHandshakeIfNeeded);
   const int HeaderEnd = m_ReceiveBuffer.find("\r\n\r\n");
   if(HeaderEnd == -1) {
      COL_TRC("Waiting for complete HTTP handshake, buffer size=" << m_ReceiveBuffer.size());
      return;
   }

   const COLstring HttpMessage = m_ReceiveBuffer.substr(0, HeaderEnd + 4);
   m_ReceiveBuffer             = m_ReceiveBuffer.substr(HeaderEnd + 4);
   const bool Success = m_Mode == GRIDWS_MODE_CLIENT ? parseHttpResponse(HttpMessage) : parseHttpRequest(HttpMessage);

   if(!Success) {
      COL_TRC("WebSocket handshake failed, mode=" << (m_Mode == GRIDWS_MODE_CLIENT ? "client" : "server"));
      return close();
   }
   COL_TRC("WebSocket handshake completed successfully");
}

COLstring GRIDwebSocket::buildHttpUpgradeRequest() {
   COL_METHOD(GRIDwebSocket::buildHttpUpgradeRequest);
   GRIDgenerateWebSocketKey(&m_WebSocketKey);
   return GRIDIbuildHttpUpgradeRequest(m_NodeInfo.m_IguanaId, m_WebSocketKey, "/");
}

bool GRIDwebSocket::parseHttpResponse(const COLstring& Response) {
   COL_METHOD(GRIDwebSocket::parseHttpResponse);
   const bool Success = GRIDIparseHttpResponse(Response, m_WebSocketKey);
   if(Success) {
      m_WsHandshakeComplete = true;
      startProtocol();
   }
   return Success;
}

bool GRIDwebSocket::parseHttpRequest(const COLstring& Request) {
   COL_METHOD(GRIDwebSocket::parseHttpRequest);
   COLstring ClientKey, Id;
   if(!GRIDIparseHttpRequest(Request, ClientKey)) return false;
   sendData(GRIDIbuildHttpUpgradeResponse(ClientKey, m_NodeInfo.m_IguanaId));
   m_WsHandshakeComplete = true;
   startProtocol();
   return true;
}

void GRIDwebSocket::processWebSocketFrames() {
   COL_METHOD(GRIDwebSocket::processWebSocketFrames);
   while(m_ReceiveBuffer.size() >= 2) {
      COLuint8   Opcode;
      COLuint64  PayloadLen;
      int        HeaderSize;
      COLuint8   MaskingKey[4] = {};
      const bool Masked        = (m_ReceiveBuffer[1] & 0x80) != 0;

      if(!parseFrameHeader(Opcode, PayloadLen, HeaderSize, MaskingKey)) { break; }

      COLstring Payload = extractFramePayload(PayloadLen, HeaderSize, MaskingKey, Masked);

      if(Opcode == GRIDWS_OPCODE_BINARY) {
         COL_TRC("Processing binary frame");
         processFrame(Payload);
      } else if(Opcode == GRIDWS_OPCODE_CLOSE) {
         COL_TRC("Received close frame");
         setLastError(-1, "Connection closed by peer");
         return close();
      } else if(Opcode == GRIDWS_OPCODE_PING) {
         handlePingFrame(Payload, PayloadLen);
      } else if(Opcode == GRIDWS_OPCODE_PONG) {
         handlePongFrame();
      } else {
         COL_TRC("Received unknown opcode=0x" << (int)Opcode);
      }
   }
}

bool GRIDwebSocket::parseFrameHeader(COLuint8& Opcode, COLuint64& PayloadLen, int& HeaderSize, COLuint8 MaskingKey[4]) {
   COL_METHOD(GRIDwebSocket::parseFrameHeader);
   if(m_ReceiveBuffer.size() < 2) {
      COL_TRC("Insufficient data for frame header, need at least 2 bytes");
      return false;
   }

   const COLuint8 Byte0 = m_ReceiveBuffer[0];
   const COLuint8 Byte1 = m_ReceiveBuffer[1];
   Opcode               = Byte0 & 0x0F;
   const bool Masked    = (Byte1 & 0x80) != 0;
   PayloadLen           = Byte1 & 0x7F;
   HeaderSize           = 2;

   if(PayloadLen == 126) {
      if(m_ReceiveBuffer.size() < 4) {
         COL_TRC("Insufficient data for extended 16-bit length");
         return false;
      }
      PayloadLen = COLbigEndianToSystem(*(COLuint16*)(m_ReceiveBuffer.c_str() + 2));
      HeaderSize = 4;
      COL_TRC("Extended 16-bit payload length=" << PayloadLen);
   } else if(PayloadLen == 127) {
      if(m_ReceiveBuffer.size() < 10) {
         COL_TRC("Insufficient data for extended 64-bit length");
         return false;
      }
      COLuint64 NetLen = *(COLuint64*)(m_ReceiveBuffer.c_str() + 2);
      PayloadLen       = COLbigEndianToSystem(NetLen);
      HeaderSize       = 10;
      COL_TRC("Extended 64-bit payload length=" << PayloadLen);
   }

   if(Masked) {
      if(m_ReceiveBuffer.size() < (size_t)(HeaderSize + 4)) {
         COL_TRC("Insufficient data for masking key");
         return false;
      }
      memcpy(MaskingKey, m_ReceiveBuffer.c_str() + HeaderSize, 4);
      HeaderSize += 4;
      COL_TRC("Frame is masked, masking key extracted");
   }

   if(m_ReceiveBuffer.size() < (size_t)(HeaderSize + PayloadLen)) {
      COL_TRC("Incomplete frame, need " << (HeaderSize + PayloadLen) << " bytes, have " << m_ReceiveBuffer.size());
      return false;
   }
   return true;
}

COLstring GRIDwebSocket::extractFramePayload(const COLuint64 PayloadLen, const int HeaderSize,
                                             const COLuint8 MaskingKey[4], const bool Masked) {
   COL_METHOD(GRIDwebSocket::extractFramePayload);
   COLstring Payload;
   if(PayloadLen > 0) {
      Payload.append(m_ReceiveBuffer.c_str() + HeaderSize, (int)PayloadLen);
      if(Masked) {
         COL_TRC("Unmasking payload of " << PayloadLen << " bytes");
         for(COLuint64 i = 0; i < PayloadLen; i++) { Payload[i] ^= MaskingKey[i % 4]; }
      }
   }
   m_ReceiveBuffer = m_ReceiveBuffer.substr(HeaderSize + (int)PayloadLen);
   COL_TRC("Extracted payload, remaining buffer size=" << m_ReceiveBuffer.size());
   return Payload;
}

void GRIDwebSocket::handlePingFrame(const COLstring& Payload, const COLuint64 PayloadLen) {
   COL_METHOD(GRIDwebSocket::handlePingFrame);
   COL_TRC("Received WebSocket ping, payload size=" << PayloadLen);
   
   const bool NeedMask = m_Mode == GRIDWS_MODE_CLIENT;
   COLstring  PongFrame;
   PongFrame.append(1, (char)GRIDWS_FRAME_PONG);

   if(NeedMask) {
      PongFrame.append(1, (char)(0x80 | (PayloadLen & 0x7F)));
      COLuint8 MaskingKey[4];
      RAND_bytes(MaskingKey, 4);
      PongFrame.append((const char*)MaskingKey, 4);
      for(COLuint64 i = 0; i < PayloadLen; i++) { PongFrame.append(1, Payload[i] ^ MaskingKey[i % 4]); }
      COL_TRC("Pong frame masked with generated key");
   } else {
      PongFrame.append(1, (char)(PayloadLen & 0x7F));
      PongFrame.append(Payload);
      COL_TRC("Pong frame unmasked");
   }
   auto* pConn = getSocket();
   if(pConn) {
      pConn->writeString(PongFrame);
      COL_TRC("Sent pong response (masked=" << NeedMask << ")");
   } else {
      COL_TRC("Connection is null.");
   }
}

void GRIDwebSocket::handlePongFrame() {
   COL_METHOD(GRIDwebSocket::handlePongFrame);
   m_Monitor.recordPingReceived();
}

void GRIDwebSocket::processFrame(const COLstring& Frame) {
   COL_METHOD(GRIDwebSocket::processFrame);
   if(Frame.size() < 3) return;
   const COLuint8* pData = (const COLuint8*)Frame.c_str();
   COLuint16       Version;
   memcpy(&Version, pData, sizeof(Version));
   Version = COLbigEndianToSystem(Version);
   pData += sizeof(Version);
   const COLuint8 MsgType = *pData++;
   if(Version != GRIDWS_VERSION) {
      COL_TRC("Version mismatch: expected " << GRIDWS_VERSION << ", got " << Version);
      return;
   }
   if(m_pAuth) {
      COLauto<COLclosure2<const void*, int>> pSendFrameFn = COLnewClosure2(this, &GRIDwebSocket::sendFrame);
      switch(MsgType) {
         case GRIDWS_MSG_AUTH_CHALLENGE: return m_pAuth->processChallenge(Frame, pSendFrameFn);
         case GRIDWS_MSG_AUTH_RESPONSE:  return m_pAuth->processResponse(Frame, pSendFrameFn);
         case GRIDWS_MSG_AUTH_SUCCESS:   return m_pAuth->processSuccess(Frame);
         case GRIDWS_MSG_HEARTBEAT:      return processHeartbeat(Frame, pData);
         case GRIDWS_MSG_JSON:           return processJson(Frame, pData);
         default:                        COL_TRC("Unknown message type: " << (int)MsgType); break;
      }
   }
}

void GRIDwebSocket::processHeartbeat(const COLstring& Frame, const COLuint8* pData) {
   COL_METHOD(GRIDwebSocket::processHeartbeat);
   if(!m_pAuth || !m_pAuth->isAuthenticated()) {
      COL_TRC("Ignoring HEARTBEAT - not authenticated yet");
      return;
   }
   GRIDwebSocketHeartbeatFrame Header;
   COLuint64 TimestampMs;
   memcpy(&TimestampMs, pData, sizeof(TimestampMs));
   Header.m_TimeStampMs = COLbigEndianToSystem(TimestampMs);
   pData += sizeof(TimestampMs);
   COLuint16 InstanceName;
   memcpy(&InstanceName, pData, sizeof(InstanceName));
   InstanceName = COLbigEndianToSystem(InstanceName);
   pData += sizeof(InstanceName);
   if(InstanceName > 0) {
      Header.m_InstanceName.append((const char*)pData, InstanceName);
      pData += InstanceName;
   }
   COLuint32 ReceivedCrc;
   memcpy(&ReceivedCrc, pData, sizeof(ReceivedCrc));
   ReceivedCrc             = COLbigEndianToSystem(ReceivedCrc);
   const COLuint32 CalcCrc = GRIDcalculateCRC32(Frame.c_str(), Frame.size() - 4);
   if(ReceivedCrc == CalcCrc && m_pOnHeartbeat.get()) m_pOnHeartbeat->run(&Header);
}

void GRIDwebSocket::processJson(const COLstring& Frame, const COLuint8* pData) {
   COL_METHOD(GRIDwebSocket::processJson);
   if(!m_pAuth || !m_pAuth->isAuthenticated()) {
      COL_TRC("Ignoring JSON - not authenticated yet");
      return;
   }
   COLuint32 PayloadSize;
   memcpy(&PayloadSize, pData, sizeof(PayloadSize));
   PayloadSize = COLbigEndianToSystem(PayloadSize);
   pData += sizeof(PayloadSize);

   if(!validateJsonFrameBounds(Frame, pData, PayloadSize)) { return; }

   COLstring JsonPayload;
   JsonPayload.append((const char*)pData, PayloadSize);
   pData += PayloadSize;
   COLuint32 ReceivedCrc;
   memcpy(&ReceivedCrc, pData, sizeof(ReceivedCrc));
   ReceivedCrc             = COLbigEndianToSystem(ReceivedCrc);
   const COLuint32 CalcCrc = GRIDcalculateCRC32(Frame.c_str(), Frame.size() - 4);
   if(ReceivedCrc == CalcCrc && m_pOnJsonFrame.get()) {
      m_pOnJsonFrame->run(&JsonPayload);
   } else {
      COL_TRC("JSON frame CRC mismatch: received=0x" << ReceivedCrc << " calculated=0x" << CalcCrc);
   }
}

bool GRIDwebSocket::validateJsonFrameBounds(const COLstring& Frame, const COLuint8* pData,
                                            const COLuint32 PayloadSize) const {
   COL_METHOD(GRIDwebSocket::validateJsonFrameBounds);
   if(PayloadSize == 0) {
      COL_TRC("JSON payload size is zero");
      return false;
   }
   const size_t DataOffset   = pData - (const COLuint8*)Frame.c_str();
   const size_t RequiredSize = DataOffset + PayloadSize + sizeof(COLuint32);
   if(RequiredSize > Frame.size()) {
      COL_TRC("JSON frame bounds invalid: offset=" << DataOffset << " payloadSize=" << PayloadSize
                                                   << " frameSize=" << Frame.size());
      return false;
   }
   return true;
}

void GRIDwebSocket::onAuthenticationComplete() {
   COL_METHOD(GRIDwebSocket::onAuthenticationComplete);
   COL_TRC("Authentication completed, starting heartbeat monitor");
   m_WsAuthComplete = true;
   clearLastError();
   m_Monitor.startHeartbeat(COLnewClosure0(this, &GRIDwebSocket::sendAutoHeartbeat));
   if(m_pOnAuth.get()) { m_pOnAuth->run(); }
}

void GRIDwebSocket::startProtocol() {
   COL_METHOD(GRIDwebSocket::startProtocol);
   if(m_NodeInfo.m_IguanaId.is_null()) {
      COL_TRC("Identity not set, skipping protocol start");
      return;
   }

   if(!m_NodeInfo.m_pPrivateKey) {
      COL_TRC("Private key not set, cannot start authentication");
      close();
      return;
   }

   COL_TRC("Authentication enabled, starting auth protocol");
   if(m_Mode == GRIDWS_MODE_SERVER && m_pAuth) {
      COLauto<COLclosure2<const void*, int>> pSendFrameFn = COLnewClosure2(this, &GRIDwebSocket::sendFrame);
      m_pAuth->initiateServerChallenge(pSendFrameFn);
   } else {
      COL_TRC("Client mode: waiting for AUTH_CHALLENGE from server");
   }
}

void GRIDwebSocket::sendAutoHeartbeat() {
   COL_METHOD(GRIDwebSocket::sendAutoHeartbeat);
   const bool Authenticated = m_pAuth && m_pAuth->isAuthenticated();
   COL_TRC("Auto-heartbeat tick, connected=" << m_Connected << " handshakeComplete=" << m_WsAuthComplete << " authenticated=" << Authenticated);
   // m_WsAuthComplete implies m_Connected, but check both for safety
   if(m_Connected && m_WsAuthComplete) {
      sendHeartbeat((COLuint64)time(NULL) * 1000);
      if(Authenticated) { sendPing(); }
      else              { COL_TRC("Skipping ping - not authenticated yet"); }
   }
}

void GRIDwebSocket::sendPing() {
   COL_METHOD(GRIDwebSocket::sendPing);
   const bool NeedMask = m_Mode == GRIDWS_MODE_CLIENT;
   COLstring Frame;
   Frame.append(1, (char)GRIDWS_FRAME_PING);

   if(NeedMask) {
      Frame.append(1, (char)0x80);
      COLuint8 MaskingKey[4];
      RAND_bytes(MaskingKey, 4);
      Frame.append((const char*)MaskingKey, 4);
   } else {
      Frame.append(1, (char)0x00);
   }
   auto* pConn = getSocket();
   if(pConn) {
      m_Monitor.recordPingSent();
      pConn->writeString(Frame);
      COL_TRC("Sent WebSocket ping (masked=" << NeedMask << ")");
   } else {
      COL_TRC("Connection is null.");
   }
}

void GRIDwebSocket::sendHeartbeat(const COLuint64 TimestampMs) {
   COL_METHOD(GRIDwebSocket::sendHeartbeat);
   COL_TRC("Sending heartbeat: timestamp=" << TimestampMs << " instance=" << m_NodeInfo.m_pWebConfig->serverDescription());
   const COLstring Frame = GRIDbuildHeartbeatFrame(TimestampMs, m_NodeInfo.m_pWebConfig->serverDescription());
   sendFrame(Frame.c_str(), Frame.size());
}

void GRIDwebSocket::sendFrame(const void* pPayload, const int PayloadSize) {
   COL_METHOD(GRIDwebSocket::sendFrame);
   COLstring Frame;
   GRIDencodeFrame(pPayload, PayloadSize, true, &Frame);
   sendData(Frame);
}

void GRIDwebSocket::sendData(const COLstring& Data) {
   COL_METHOD(GRIDwebSocket::sendData);
   auto* pConn = getSocket();
   if(pConn) { pConn->writeString(Data); }
   else      { COL_TRC("Connection is null."); }
}

void GRIDwebSocket::captureLastError() {
   COL_METHOD(GRIDwebSocket::captureLastError);
   if(m_LastErrorCode != 0) {
      COL_TRC("Already have error set: code=" << m_LastErrorCode << " message=" << m_LastErrorString);
      return;
   }
   const SCKconnection* pConn = getSocket();
   if(!pConn) {
      COL_TRC("No connection to capture error from");
      if(m_LastErrorString.is_null()) { setLastError(503, "Server unreachable."); }
      return;
   }
   m_LastErrorCode = pConn->lastError();
   if(m_LastErrorCode == 0) {
      COL_TRC("No error to capture");
      return;
   }
   // For SSL connections, check for SSL-specific errors
   if(m_IsSecure) {
      auto* pNonConstConn = const_cast<SCKconnection*>(pConn);
      auto pSslConn = dynamic_cast<SCKSsslConnection*>(pNonConstConn);
      if(pSslConn && !pSslConn->sslError().is_null()) { 
         m_LastErrorString = pSslConn->sslError(); 
      } else { 
         m_LastErrorString = SCKsocketError(m_LastErrorCode); 
      }
   } else {
      m_LastErrorString = SCKsocketError(m_LastErrorCode);
   }
   COL_TRC("Captured error: code=" << m_LastErrorCode << " message=" << m_LastErrorString);
}

void GRIDwebSocket::clearLastError() {
   COL_METHOD(GRIDwebSocket::clearLastError);
   m_LastErrorCode = 0;
   m_LastErrorString.clear();
   COL_TRC("Cleared last error");
}

void GRIDwebSocket::setLastError(const int Code, const COLstring& Message) {
   COL_METHOD(GRIDwebSocket::setLastError);
   m_LastErrorCode = Code;
   m_LastErrorString = Message;
   COL_TRC("Set error: code=" << Code << " message=" << Message);
}

void GRIDwebSocket::resetMembers() {
   COL_METHOD(GRIDwebSocket::resetMembers);
   m_Connected           = false;
   m_WsHandshakeComplete = false;
   m_WsAuthComplete      = false;
   m_Monitor.stopHeartbeat();
}
