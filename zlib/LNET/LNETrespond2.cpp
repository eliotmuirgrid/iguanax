// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNETrespond
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday 27 October 2023 - 03:25PM
// ---------------------------------------------------------------------------

#include <LNET/LNETrespond.h>

#include <BLUA/BLUAinstance.h>

#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure6.h>
#include <COL/COLevent.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static nghttp2_nv LNETcreateNv(const COLstring& name, const COLstring& value) {
   nghttp2_nv nv;
   nv.name = (uint8_t*)strdup(name.c_str());
   nv.value = (uint8_t*)strdup(value.c_str());
   nv.namelen = name.size();
   nv.valuelen = value.size();
   nv.flags = NGHTTP2_NV_FLAG_NONE; // Adjust flags as needed
   return nv;
}

static COLarray<nghttp2_nv> LNETprepHttp2Headers(const COLstring& StatusCode, const COLvar& Headers) {
   COLarray<nghttp2_nv> Out;
   Out.push_back(LNETcreateNv(":status", StatusCode.data()));
   for (const auto& Header : Headers.array()) {
      COLstring Name, Value;
      Header.str().split(Name, Value, ": ");
      COL_VAR2(Name, Value);
      Out.push_back(LNETcreateNv(Name.data(), Value.data()));
   }
   return Out;
}

static nghttp2_ssize LNEThttp2ResponseDataCallback(nghttp2_session *session, int32_t stream_id,
                                                   uint8_t *buf, size_t length, uint32_t *data_flags,
                                                   nghttp2_data_source *source, void *user_data) {
   COL_TRC("send body");
   COLstring* pData = (COLstring*) source->ptr;
   int data_len = pData->size();
   if (length < data_len) {
      memcpy(buf, pData->data(), length);
      COL_TRC("Processed " << length << " bytes. Advancing the data pointer");
      pData->shift(length);
      return length;
   }
   memcpy(buf, pData->data(), data_len);
   *data_flags = NGHTTP2_DATA_FLAG_EOF;
   delete pData; // pData was created on heap in LNEThttp2Respond
   return data_len;
}

static void LNEThttp2Respond(COLvar& Parameters, nghttp2_session *pSession, int StreamId, COLstring* pError) {
   COL_FUNCTION(LNEThttp2Respond);
   COL_VAR(Parameters);
   COL_TRC("send headers");
   COLarray<nghttp2_nv> Headers = LNETprepHttp2Headers(Parameters["code"], Parameters["headers"]);
   const COLstring& Body = Parameters["body"];
   int Flags = NGHTTP2_FLAG_END_HEADERS;
   if (Body.is_null()) { 
      Flags |= NGHTTP2_FLAG_END_STREAM;
   }
   int rv = nghttp2_submit_headers(pSession, Flags, StreamId, NULL, &(Headers[0]), Headers.size(), NULL);
   if (0 != rv) {
      *pError = nghttp2_strerror(rv);
      COL_ERR("Fatal error: " << *pError);
      return;
   }
   if (Body.is_null()) {
      COL_TRC("No body. End stream on headers.");
      rv = nghttp2_session_send(pSession);
      if (rv != 0) {
         *pError = nghttp2_strerror(rv);
         COL_ERR("Fatal error (send): " << *pError);
      }
      return;
   }
   // New from IX-3729 - create the string on heap. We'll free it in the callback once all the data is sent. This is needed because if we're sending lots of data
   // the string could go out of scope and we could segfault later. Also nice to have a COLstring since we don't have to worry about null bytes in the middle
   COLstring* pData = new COLstring(Body);
   nghttp2_data_provider2 data_provider;
   data_provider.source.ptr = (void*) pData;
   data_provider.read_callback = LNEThttp2ResponseDataCallback;
   COL_TRC("send data");
   rv = nghttp2_submit_data2(pSession, NGHTTP2_FLAG_END_STREAM, StreamId, &data_provider);
   if (0 != rv) {
      COL_ERR("Fatal error (submit): " << nghttp2_strerror(rv));
   }
   rv = nghttp2_session_send(pSession);
   if (!(rv)) {
      if (rv == 0) { COL_TRC("rv was 0, no bytes left to send"); } 
      else         { COL_ERR("Fatal error (send): " << nghttp2_strerror(rv)); }
   }
}

static void LNEThttp2RespondOnMainThread(BLUAinstance2* pInstance, COLvar Parameters, int* pConnId, int* pStreamId, COLstring* pError, COLevent* pEvent) {
   COL_FUNCTION(LNEThttp2RespondOnMainThread);
   int& ConnId   = *pConnId;
   int& StreamId = *pStreamId;
   if (!pInstance->m_Connections.count(ConnId) || !pInstance->m_Http2Connections.count(ConnId)) {
      COL_TRC("No longer have connection " << ConnId);
      *pError = "Connection is gone";
   } else {
      COL_TRC("Connection exists, writing response");
      LNEThttp2Respond(Parameters, pInstance->m_Http2Connections[ConnId].pSession, StreamId, pError);
      ConnId = -1;
      StreamId = 0;
   }
   COL_TRC("Response done, signal Lua thread to continue");
   pEvent->signal();
}


// must be called from Lua worker thread
bool LNEThttp2RespondAsync(BLUAinstance2* pInstance, const COLvar& Parameters, int* pConnId, int* pStreamId, COLstring* pError) {
   COL_FUNCTION(LNEThttp2RespondAsync);
   pError->clear();
   COLevent Event;
   pInstance->m_pLoop->post(COLnewClosure0(&LNEThttp2RespondOnMainThread, pInstance, Parameters, pConnId, pStreamId, pError, &Event));
   COL_TRC("Posting to main and running...");
   COL_TRC("Blocking until response is sent...");
   Event.wait();
   return pError->is_null();
}
