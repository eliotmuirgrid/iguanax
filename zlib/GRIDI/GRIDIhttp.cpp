//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDhttp.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GRIDI/GRIDIhttp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
COL_LOG_MODULE;

static auto WEBSOCKET_GUID = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

COLstring GRIDIbuildHttpUpgradeRequest(const COLstring& IguanaId, const COLstring& WebSocketKey,
                                       const COLstring& Path) {
   COL_FUNCTION(GRIDIbuildHttpUpgradeRequest);
   COL_VAR3(IguanaId, WebSocketKey, Path);
   COLstring Request;
   Request.append("GET ");
   Request.append(Path);
   Request.append(" HTTP/1.1\r\n");
   Request.append("IguanaId: " + IguanaId + "\r\n");
   Request.append("Upgrade: websocket\r\n");
   Request.append("Connection: Upgrade\r\n");
   Request.append("Sec-WebSocket-Key: ");
   Request.append(WebSocketKey);
   Request.append("\r\n");
   Request.append("Sec-WebSocket-Version: 13\r\n");
   Request.append("\r\n");
   COL_VAR(Request);
   return Request;
}

COLstring GRIDIbuildHttpUpgradeResponse(const COLstring& ClientKey, const COLstring& IguanaId) {
   COL_FUNCTION(GRIDIbuildHttpUpgradeResponse);
   const COLstring AcceptKey = ClientKey + WEBSOCKET_GUID;
   COLuint8        Hash[SHA_DIGEST_LENGTH];
   SHA1((const COLuint8*)AcceptKey.c_str(), AcceptKey.size(), Hash);
   BIO* pBio = BIO_new(BIO_s_mem());
   BIO* pB64 = BIO_new(BIO_f_base64());
   BIO_set_flags(pB64, BIO_FLAGS_BASE64_NO_NL);
   pBio = BIO_push(pB64, pBio);
   BIO_write(pBio, Hash, SHA_DIGEST_LENGTH);
   BIO_flush(pBio);
   BUF_MEM* pBufferPtr;
   BIO_get_mem_ptr(pBio, &pBufferPtr);
   const COLstring ExpectedAccept(pBufferPtr->data, pBufferPtr->length);
   BIO_free_all(pBio);
   COLstring HttpResponse;
   HttpResponse.append("HTTP/1.1 101 Switching Protocols\r\n");
   HttpResponse.append("IguanaId: " + IguanaId + "\r\n");
   HttpResponse.append("Upgrade: websocket\r\n");
   HttpResponse.append("Connection: Upgrade\r\n");
   HttpResponse.append("Sec-WebSocket-Accept: ");
   HttpResponse.append(ExpectedAccept);
   HttpResponse.append("\r\n\r\n");
   COL_VAR(HttpResponse);
   return HttpResponse;
}

static bool GRIDIextractHeaderValue(const COLstring& Request, const COLstring& Name, COLstring& Value) {
   COL_FUNCTION(GRIDIextractHeaderValue);
   COLstring Key(Name + ": ");
   int Start = Request.find(Key.c_str());
   if (-1 == Start) { return false; }
   int End = Request.find("\r\n", Start);
   Value = Request.substr(Start + Key.size(), End - Start - Key.size());
   COL_VAR2(Name, Value);
   return true;
}

// Validates server response during WebSocket upgrade
bool GRIDIparseHttpResponse(const COLstring& Response, const COLstring& ExpectedKey) {
   COL_FUNCTION(GRIDIparseHttpResponse);
   COL_VAR(Response);
   if(Response.find("HTTP/1.1 101") == -1) {
      COL_TRC("WebSocket upgrade failed - invalid response");
      return false;
   }
   const COLstring AcceptKey = ExpectedKey + WEBSOCKET_GUID;
   COLuint8        Hash[SHA_DIGEST_LENGTH];
   SHA1((const COLuint8*)AcceptKey.c_str(), AcceptKey.size(), Hash);
   BIO* pBio = BIO_new(BIO_s_mem());
   BIO* pB64 = BIO_new(BIO_f_base64());
   BIO_set_flags(pB64, BIO_FLAGS_BASE64_NO_NL);
   pBio = BIO_push(pB64, pBio);
   BIO_write(pBio, Hash, SHA_DIGEST_LENGTH);
   BIO_flush(pBio);
   BUF_MEM* pBufferPtr;
   BIO_get_mem_ptr(pBio, &pBufferPtr);
   const COLstring ExpectedAccept(pBufferPtr->data, pBufferPtr->length);
   BIO_free_all(pBio);
   COLstring ReceivedAccept;
   if (!GRIDIextractHeaderValue(Response, "Sec-WebSocket-Accept", ReceivedAccept)) {
      COL_TRC("WebSocket upgrade failed - no Sec-WebSocket-Accept key");
      return false;
   }
   if(ReceivedAccept != ExpectedAccept) {
      COL_TRC("WebSocket upgrade failed - invalid accept key");
      return false;
   }
   return true;
}

// Extracts Sec-WebSocket-Key from an HTTP upgrade request
bool GRIDIparseHttpRequest(const COLstring& Request, COLstring& OutClientKey) {
   COL_FUNCTION(GRIDIparseHttpRequest);
   if(Request.find("GET ") != 0) {
      COL_TRC("Invalid HTTP request - not a GET");
      return false;
   }
   if(Request.find(" HTTP/1.1") == -1) {
      COL_TRC("Invalid HTTP request - not HTTP/1.1");
      return false;
   }
   if(Request.find("Upgrade: websocket") == -1) {
      COL_TRC("WebSocket upgrade failed - no upgrade header");
      return false;
   }
   if(Request.find("Connection: Upgrade") == -1) {
      COL_TRC("WebSocket upgrade failed - no connection header");
      return false;
   }
   if (!GRIDIextractHeaderValue(Request, "Sec-WebSocket-Key", OutClientKey)) {
      COL_TRC("WebSocket upgrade failed - no IguanaId key");
      return false;
   }
   return true;
}