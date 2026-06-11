#ifndef _GRIDWEBSOCKET_H
#define _GRIDWEBSOCKET_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDwebSocket.h
//
// Represents a single WebSocket connection either client-initiated or server-accepted
// Handles WebSocket protocol details framing, masking, handshake upgrade
// Can be used standalone without GRIDnode for single connections
//
// WebSocket Protocol Frames
//
// Heartbeat Frame Layout (binary payload):
// ┌──────────────────────────────────────────────┐
// │ COLuint16  version                            │
// │ COLuint8   msg_type  (GRIDWS_MSG_HEARTBEAT)   │
// │ COLuint64  timestamp_ms                       │
// │ COLuint8   role                               │
// │ COLuint16  instance_id_len                    │
// │ COLuint8[] instance_id (instance_id_len bytes)│
// └──────────────────────────────────────────────┘
// Followed by: COLuint32 CRC32
//
// JSON Frame Layout (binary payload):
// ┌──────────────────────────────────────────────┐
// │ COLuint16  version                            │
// │ COLuint8   msg_type  (GRIDWS_MSG_JSON)        │
// │ COLuint32  payload_size                       │
// │ COLuint8[] json_payload (payload_size bytes)  │
// └──────────────────────────────────────────────┘
// Followed by: COLuint32 CRC32
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <GRID/GRIDconnectionMonitor.h>
#include <GRID/GRIDframe.h>
#include <SCK/SCKloop.h>

class SCKSsslContext;
class SCKSsslConnection;
class SCKconnection;
class GRIDSwebSocketAuth;

struct GRIDnodeInfo;

const COLuint16 GRIDWS_VERSION = 0x0001;

enum GRIDwebSocketMode { GRIDWS_MODE_CLIENT = 0, GRIDWS_MODE_SERVER = 1 };

enum GRIDwebSocketOpcode {
   GRIDWS_OPCODE_BINARY = 0x2,
   GRIDWS_OPCODE_CLOSE  = 0x8,
   GRIDWS_OPCODE_PING   = 0x9,
   GRIDWS_OPCODE_PONG   = 0xA
};

static const COLuint8 GRIDWS_FRAME_CLOSE = 0x88;
static const COLuint8 GRIDWS_FRAME_PING  = 0x89;
static const COLuint8 GRIDWS_FRAME_PONG  = 0x8A;

class GRIDwebSocket {
  public:
   GRIDwebSocket(GRIDnodeInfo& NodeInfo, const COLstring& Fingerprint, SCKloop& Loop, bool IsServer = false, SCKconnection* pConnection = nullptr);
   ~GRIDwebSocket();

   SCKconnection* getSocket() const;
   void connect(const COLstring& Address, int Port) const;
   void sendJson(const COLstring& JsonString);
   void sendChallenge();
   void close();

   void setOnHeartbeat(COLauto<COLclosure1<const GRIDwebSocketHeartbeatFrame*>> pCallback) { m_pOnHeartbeat = pCallback; }
   void setOnJsonFrame(COLauto<COLclosure1<const COLstring*>> pCallback)                   { m_pOnJsonFrame = pCallback; }
   void setOnConnect(COLauto<COLclosure0> pCallback)                                       { m_pOnConnect   = pCallback; }
   void setOnAuth(COLauto<COLclosure0> pCallback)                                          { m_pOnAuth      = pCallback; }
   void setOnClose(COLauto<COLclosure0> pCallback)                                         { m_pOnClose     = pCallback; }

   bool      connected()          const { return m_Connected && m_WsAuthComplete; };
   bool      isSecure()           const { return m_IsSecure; }
   COLuint64 getLastRtt()         const { return m_Monitor.getLastRtt(); }
   int       getLastErrorCode()   const { return m_LastErrorCode; }
   COLstring getLastErrorString() const { return m_LastErrorString; };

  private:
   GRIDnodeInfo&         m_NodeInfo;
   SCKloop&              m_Loop;
   GRIDconnectionMonitor m_Monitor;
   GRIDSwebSocketAuth*   m_pAuth = nullptr;

   COLuint64 m_ConnectionId = 0;
   bool      m_IsSecure     = false;

   bool              m_WsHandshakeComplete = false;
   bool              m_WsAuthComplete      = false;
   bool              m_Connected           = false;
   COLstring         m_ReceiveBuffer       = "";
   COLstring         m_WebSocketKey        = "";
   GRIDwebSocketMode m_Mode                = GRIDWS_MODE_CLIENT;
   
   int               m_LastErrorCode       = 0;
   COLstring         m_LastErrorString     = "";

   COLauto<COLclosure1<const GRIDwebSocketHeartbeatFrame*>> m_pOnHeartbeat;
   COLauto<COLclosure1<const COLstring*>>                   m_pOnJsonFrame;
   COLauto<COLclosure0>                                     m_pOnConnect;
   COLauto<COLclosure0>                                     m_pOnAuth;
   COLauto<COLclosure0>                                     m_pOnClose;
   COLauto<COLclosure1<COLuint64>>                          m_pChainedOnClose;

   // Connection lifecycle callbacks
   void onData(COLuint64 ConnectionId, COLstring* pData);
   void onClose(COLuint64 ConnectionId);
   void performHandshake(COLuint64 ConnectionId);

   // HTTP handshake processing
   void      processHandshake();
   COLstring buildHttpUpgradeRequest();
   bool      parseHttpResponse(const COLstring& Response);
   bool      parseHttpRequest(const COLstring& Request);

   // WebSocket frame processing
   void      processWebSocketFrames();
   bool      parseFrameHeader(COLuint8& Opcode, COLuint64& PayloadLen, int& HeaderSize, COLuint8 MaskingKey[4]);
   COLstring extractFramePayload(COLuint64 PayloadLen, int HeaderSize, const COLuint8 MaskingKey[4], bool Masked);
   void      handlePingFrame(const COLstring& Payload, COLuint64 PayloadLen);
   void      handlePongFrame();

   // Protocol frame handlers
   void processFrame(const COLstring& Frame);
   void processHeartbeat(const COLstring& Frame, const COLuint8* pData);
   void processJson(const COLstring& Frame, const COLuint8* pData);
   bool validateJsonFrameBounds(const COLstring& Frame, const COLuint8* pData, COLuint32 PayloadSize) const;

   // Authentication
   void onAuthenticationComplete();
   void startProtocol();

   // Heartbeat and ping
   void sendAutoHeartbeat();
   void sendPing();
   void sendHeartbeat(COLuint64 TimestampMs);

   // Low-level send
   void sendFrame(const void* pPayload, int PayloadSize);
   void sendData(const COLstring& Data);
   
   void captureLastError();
   void clearLastError();
   void setLastError(int Code, const COLstring& Message);

   void resetMembers();

   GRIDwebSocket(const GRIDwebSocket&)            = delete;
   GRIDwebSocket& operator=(const GRIDwebSocket&) = delete;
};

#endif  // end of defensive include
