// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNEThttpListen
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 10:08AM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAsocket.h>
#include <COL/COLlog.h>
#include <COL/COLostream.h>
#include <SFI/SFIbase64DecodeFilter.h>
#include <SCK/SCKconnection.h>
#include <WEB/WEBrequestData.h>
#include <WEB/WEBserverParse.h>
COL_LOG_MODULE;

static const COLstring& HTTP2_PREFACE = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";

bool LNETfoundHttp2Preface(const COLstring& Buffer) {
   return 0 == memcmp(Buffer.data(), HTTP2_PREFACE.data(), HTTP2_PREFACE.size());
}

bool LNETisHttp2UpgradeRequest(const WEBrequestData& Request) {
   COL_FUNCTION(LNETisHttp2UpgradeRequest);
   COL_VAR(Request);
   if (Request.Headers.count("upgrade") == 0) { return false; }
   const COLstring& Upgrade = Request.Headers["upgrade"];
   if (Request.Headers["upgrade"] != "h2c") { return false; }
   if (Request.Headers.count("http2-settings") != 1) { return false; }
   return true;
}

bool LNETisHttp2Connection(BLUAinstance2* pInstance, int ConnId) {
   return pInstance->m_Http2Connections.count(ConnId) != 0;
}

// whenever nghttp2 library needs to send data, write to the underlying SCKconnection
static nghttp2_ssize LNEThttp2WriteData(nghttp2_session *session,
                              const uint8_t *data, size_t length,
                              int flags, void *user_data) {
   COL_FUNCTION(LNEThttp2WriteData);
   HTTP2sessionData& SessionData = *(HTTP2sessionData*)user_data;
   COL_TRC_DUMP("data", data, length);
   COL_VAR(SessionData.pConnection);
   SessionData.pConnection->write((const char*)data, length);
   return (nghttp2_ssize)length;
}

static COLvar LNEThttp2StreamDataToVar(const HTTP2streamData& StreamData) {
   COLvar Result;
   Result["method"] = StreamData.Method;
   Result["scheme"] = StreamData.Scheme;
   Result["authority"] = StreamData.Authority;
   Result["path"] = StreamData.Path;
   COLvar& Headers = Result["headers"];
   for (const auto& Pair : StreamData.Headers) {
      Headers[Pair.first] = Pair.second;
   }
   Result["body"] = StreamData.Body;
   return Result;
}

static char LNETtoUppercase(const char Char) {
   if ('a' <= Char && Char <= 'z') {
      return Char - 32;
   }
   return Char;
}

static COLstring LNETcamelCaseHeaderName(const COLstring& RawHeader) {
   COL_FUNCTION(LNETcamelCaseHeaderName);
   COL_VAR(RawHeader);
   if (RawHeader.equalsIgnoreCase("ifw_conn_id") || RawHeader.equalsIgnoreCase("ifw_stream_id")) {
      COL_TRC("Skip special internal header " << RawHeader);
      return RawHeader;
   }
   COLstring CamelCaseHeader;
   CamelCaseHeader += LNETtoUppercase(RawHeader[0]);
   for (int i = 1; i < RawHeader.size(); i++) {
      CamelCaseHeader += RawHeader[i];
      if (RawHeader[i] == '-') {
         i++;
         if (i < RawHeader.size()) {   // just in case the header name ends with '-'
            CamelCaseHeader += LNETtoUppercase(RawHeader[i]);
         }
      }
   }
   return CamelCaseHeader;
}

static COLstring LNEThttp2Serialize(const HTTP2streamData& StreamData) {
   COL_FUNCTION(LNEThttp2Serialize);
   COLstring Out;
   COLostream Stream(Out);
   Stream << StreamData.Method << " " << StreamData.Path << " HTTP/2\r\n";
   Stream << "Host: " << StreamData.Authority << "\r\n";
   for (const auto& Header : StreamData.Headers) {
      Stream << LNETcamelCaseHeaderName(Header.first) << ": " << Header.second << "\r\n";
   }
   Stream << "\r\n";
   if (!StreamData.Body.is_null()) {
      Stream << StreamData.Body;
   }
   COL_VAR(Out);
   return Out;
}

static int LNEThttp2OnFrameReceived(nghttp2_session *session, const nghttp2_frame *frame, void *user_data) {
   COL_FUNCTION(LNEThttp2OnFrameReceived);
   HTTP2sessionData& SessionData = *(HTTP2sessionData*)user_data;
   HTTP2streamData*  pStreamData  = nullptr;
   COL_VAR((int)frame->hd.type);
   switch (frame->hd.type) {
      case NGHTTP2_DATA:
         COL_TRC("NGHTTP2_DATA");
      case NGHTTP2_HEADERS:
         COL_TRC("NGHTTP2_HEADERS");
         /* Check that the client request has finished */
         if (frame->hd.flags & NGHTTP2_FLAG_END_STREAM) {
            pStreamData = (HTTP2streamData*) nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
            /* For DATA and HEADERS frame, this callback may be called after
               on_stream_close_callback. Check that stream still alive. */
            if (!pStreamData) { return 0; }
            COLstring Data = LNEThttp2Serialize(*pStreamData);
            COL_TRC("Received a complete request on connection " << SessionData.ConnectionId << " stream " << frame->hd.stream_id);
            COL_TRC("Call main with Data:");
            COL_VAR(Data);
            SessionData.pTaskQueue->put(BLUAhttpOnDataTask(SessionData.ConnectionId, Data, frame->hd.stream_id));
         }
         break;
      case NGHTTP2_SETTINGS:
         COL_TRC("NGHTTP2_SETTINGS");
      default:
         break;
   }
   return 0;
}

static int LNEThttp2OnStreamClosed(nghttp2_session *session, int32_t stream_id, uint32_t error_code, void *user_data) {
   COL_FUNCTION(LNEThttp2OnStreamClosed);
   HTTP2streamData*  pStreamData =  (HTTP2streamData*) nghttp2_session_get_stream_user_data(session, stream_id);
   if (!pStreamData) {
      return 0;
   }
   HTTP2sessionData& SessionData = *(HTTP2sessionData*)user_data;
   // remove HTTP2streamData from HTTP2sessionData
   auto Stream = SessionData.Streams.begin();
   while (Stream != SessionData.Streams.end()) {
      if (pStreamData == *Stream) {
         delete pStreamData;
         Stream = SessionData.Streams.erase(Stream);
      } else {
         Stream++;
      }
   }
   return 0;
}

static void LNEThttp2ParsePseudoHeaderFields(const uint8_t *name,  size_t namelen,
                                             const uint8_t *value, size_t valuelen,
                                             HTTP2streamData* pStreamData) {
   COL_FUNCTION(LNEThttp2ParsePseudoHeaderFields);
   if ( 7 == namelen && memcmp(":method", name, namelen) == 0) {
      pStreamData->Method    = COLstring((char*)value, valuelen);
      COL_VAR(pStreamData->Method);
   }
   if ( 7 == namelen && memcmp(":scheme", name, namelen) == 0) {
      pStreamData->Scheme    = COLstring((char*)value, valuelen);
      COL_VAR(pStreamData->Scheme);
   }
   if (10 == namelen && memcmp(":authority", name, namelen) == 0) {
      pStreamData->Authority = COLstring((char*)value, valuelen);
      COL_VAR(pStreamData->Authority);
   }
   if ( 5 == namelen && memcmp(":path", name, namelen) == 0) {
      pStreamData->Path      = COLstring((char*)value, valuelen);
      COL_VAR(pStreamData->Path);
   }
}

static int LNEThttp2OnHeader(nghttp2_session *session, const nghttp2_frame *frame,
                              const uint8_t *name,  size_t namelen,
                              const uint8_t *value, size_t valuelen,
                              uint8_t flags, void *user_data) {
   COL_FUNCTION(LNEThttp2OnHeader);
   static const COLstring PATH(":path");
   switch (frame->hd.type) {
   case NGHTTP2_HEADERS:
      if (frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
         break;
      }
      HTTP2streamData* pStreamData = (HTTP2streamData*) nghttp2_session_get_stream_user_data(session, frame->hd.stream_id);
      if (!pStreamData) {
         COL_TRC("stream not initialized or :path header already found and set. skip");
         break;
      }
      if (0 == memcmp(name, ":", 1)) {
         COL_TRC("handle pseudo headers");
         LNEThttp2ParsePseudoHeaderFields(name, namelen, value, valuelen, pStreamData);
      } else {
         COL_TRC("handle regular headers");
         COL_TRC("" << (char*) name << " -> " << (char*) value);
         pStreamData->Headers[COLstring((const char*) name, namelen)] = COLstring((const char*) value, valuelen);;
      }
      break;
   }
   return 0;
}

static int LNEThttp2OnHeaderBlockStarted(nghttp2_session *session, const nghttp2_frame *frame, void *user_data) {
   COL_FUNCTION(LNEThttp2OnHeaderBlockStarted);
   if (frame->hd.type != NGHTTP2_HEADERS || frame->headers.cat != NGHTTP2_HCAT_REQUEST) {
      return 0;
   }
   HTTP2sessionData& SessionData = *(HTTP2sessionData*)user_data;
   HTTP2streamData* pStreamData  = new HTTP2streamData();
   pStreamData->ID = frame->hd.stream_id;
   SessionData.Streams.push_back(pStreamData);
   nghttp2_session_set_stream_user_data(session, frame->hd.stream_id, pStreamData);
   return 0;
}

static int LNEThttp2OnDataChunkReceived(nghttp2_session *session, uint8_t flags, int32_t stream_id, const uint8_t *data, size_t len, void *user_data){
   COL_FUNCTION(LNEThttp2OnDataChunkReceived);
   HTTP2sessionData& SessionData = *(HTTP2sessionData*)user_data;
   HTTP2streamData* pStreamData  = (HTTP2streamData*) nghttp2_session_get_stream_user_data(session, stream_id);
   if (!pStreamData) {
      COL_TRC("No stream data found for stream " << stream_id);
      return 0;
   }
   // Append data to a buffer associated with the stream
   COL_DBG("Received " << len << " bytes on stream " << stream_id);
   COL_TRC_DUMP("http2 data", (const char*)data, len);
   pStreamData->Body.append((const char*)data, len);
   return 0;
}

static void LNEThttp2SetupSession(HTTP2sessionData& SessionData) {
   COL_FUNCTION(LNEThttp2SetupSession);
   nghttp2_session_callbacks *callbacks;
   nghttp2_session_callbacks_new(&callbacks);
   nghttp2_session_callbacks_set_send_callback2           (callbacks, LNEThttp2WriteData);
   nghttp2_session_callbacks_set_on_frame_recv_callback   (callbacks, LNEThttp2OnFrameReceived);
   nghttp2_session_callbacks_set_on_stream_close_callback (callbacks, LNEThttp2OnStreamClosed);
   nghttp2_session_callbacks_set_on_header_callback       (callbacks, LNEThttp2OnHeader);
   nghttp2_session_callbacks_set_on_begin_headers_callback(callbacks, LNEThttp2OnHeaderBlockStarted);
   nghttp2_session_callbacks_set_on_data_chunk_recv_callback(callbacks, LNEThttp2OnDataChunkReceived); // IX-4180 need this to grab request body
   nghttp2_session_server_new(&SessionData.pSession, callbacks, &SessionData);
   nghttp2_session_callbacks_del(callbacks);
}

static bool LNEThttp2Send(HTTP2sessionData& SessionData) {
   COL_FUNCTION(LNEThttp2Send);
   int rv = nghttp2_session_send(SessionData.pSession);
   if (rv != 0) {
      COL_ERR("Fatal error: " << nghttp2_strerror(rv));
      return false;
   }
   return true;
}

static bool LNEThttp2SendServerSetting(HTTP2sessionData& SessionData) {
   COL_FUNCTION(LNEThttp2SendServerSetting);
   nghttp2_settings_entry iv[1] = {{NGHTTP2_SETTINGS_MAX_CONCURRENT_STREAMS, 100}};
   int rv = nghttp2_submit_settings(SessionData.pSession, NGHTTP2_FLAG_NONE, iv, 1);
   if (rv != 0) {
      COL_ERR("Fatal error: " << nghttp2_strerror(rv));
      return false;
   }
   if (!LNEThttp2Send(SessionData)) { return false; }
   return true;
}

void LNETsetupHttp2Connection(BLUAinstance2* pInstance, int ConnId, const COLstring& UpgradeRequest, const COLstring& Http2Settings) {
   COL_FUNCTION(LNETsetupHttp2Connection);
   if (LNETisHttp2Connection(pInstance, ConnId)) {
      COL_ERR("trying to set up HTTP2 connection " << ConnId << " again. Something is wrong");
      return;
   }
   if (UpgradeRequest.is_null()) {
      COL_TRC("HTTP2 with prior knowledge");
   } else {
      COL_TRC("HTTP2 without prior knowledge");
      COL_TRC("send 101 response");
      static const COLstring Response = "HTTP/1.1 101 Switching Protocols\r\n"
                                        "Upgrade: h2c\r\n"
                                        "Connection: Upgrade\r\n"
                                        "\r\n";
      pInstance->m_Connections[ConnId]->writeString(Response);
   }
   COL_TRC("set up new HTTP2 connection");
   HTTP2sessionData& SessionData = pInstance->m_Http2Connections[ConnId];
   SessionData.ConnectionId = ConnId;
   SessionData.pConnection = pInstance->m_Connections[ConnId];
   SessionData.pTaskQueue = &pInstance->m_JobQueue;
   LNEThttp2SetupSession(SessionData);
   if (!UpgradeRequest.is_null()) {
      COLstring RawHttp2Settings;
      SFIbase64Decode(Http2Settings, &RawHttp2Settings);
      nghttp2_session_upgrade2(SessionData.pSession, (const uint8_t*)RawHttp2Settings.data(), RawHttp2Settings.size(), 0, NULL);
      pInstance->m_JobQueue.put(BLUAhttpOnDataTask(ConnId, UpgradeRequest, 1));
   }
   if (!LNEThttp2SendServerSetting(SessionData)) { pInstance->m_Connections[ConnId]->hardClose(); }
}

// only frees resources related to http2 connection, not the connection itself.
void LNETteardownHttp2Connection(BLUAinstance2* pInstance, int ConnId) {
   COL_FUNCTION(LNETsetupHttp2Connection);
   nghttp2_session_del(pInstance->m_Http2Connections[ConnId].pSession);
   pInstance->m_Http2Connections.removeKey(ConnId);
}

void LNEThandleHttp2Data(COLstring* pData, BLUAinstance2* pInstance, int ConnId) {
   COL_FUNCTION(LNEThandleHttp2Data);
   COL_DUMP("buf", pData->data(), pData->size());
   if (!LNETisHttp2Connection(pInstance, ConnId)) {
      COL_ERR("trying to process data for the non-existant HTTP2 connection " << ConnId << ". Something is wrong");
      return;
   }
   HTTP2sessionData& SessionData = pInstance->m_Http2Connections[ConnId];
   int Amount = nghttp2_session_mem_recv2(SessionData.pSession, (unsigned char *)pData->data(), pData->size());
   COL_VAR(Amount);
   COLASSERT(0 < Amount);
   pData->shift(Amount);
   // send frames produced by nghttp2_session_mem_recv2()
   if (!LNEThttp2Send(SessionData)) { pInstance->m_Connections[ConnId]->hardClose(); }
}

void LNEThttp2OnWrite(BLUAinstance2* pInstance, int ConnId) {
   COL_FUNCTION(LNEThttp2OnWrite);
   HTTP2sessionData& SessionData = pInstance->m_Http2Connections[ConnId];
   if (nghttp2_session_want_read (SessionData.pSession) == 0 &&
       nghttp2_session_want_write(SessionData.pSession) == 0) {
      nghttp2_session_del(SessionData.pSession);
      return;
   }
   if (!LNEThttp2Send(SessionData)) { pInstance->m_Connections[ConnId]->hardClose(); }
}

int LNETsslAlpnSelector(SSL *ssl, const unsigned char **out, unsigned char *outlen, const unsigned char *in, unsigned int inlen, void *arg) {
   COL_FUNCTION(LNETsslAlpnSelector);
   int rv = nghttp2_select_alpn(out, outlen, in, inlen);
   if (rv != 1) {
      return SSL_TLSEXT_ERR_NOACK;
   }
   return SSL_TLSEXT_ERR_OK;
}
