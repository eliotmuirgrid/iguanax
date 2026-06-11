//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBclient
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include "WEBclient.h"
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBurl.h>

#include <SFI/SFIuriEscapeFilter.h>

#include <SCK/SCKaddress.h>
#include <SCK/SCKerror.h>
#include <SCK/SCKloop.h>

#include <COL/COLsplit.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

WEBclientConnection::WEBclientConnection(WEBcallbackFunc pCallback, int Timeout, SCKloop* pLoop) 
: _pCallback(pCallback), _pSocket(NULL), _ReturnCode(0), 
   _ContentLength(-1), _TimeoutHandle(-1), _pLoop(pLoop),
   _ChunkedContentEncoding(false), _FoundLastChunk(false),
   m_TimeoutLength(Timeout) 
{
   setTimeoutTimer();
   _SslContext.setServerMode(false);
}

void WEBclientConnection::cancelTimeout(){
   COL_METHOD(cancelTimeout);
   if (_TimeoutHandle != -1){
      COL_TRC("Cancelling timeout timer.");
      _pLoop->cancelTimer(_TimeoutHandle);
      _TimeoutHandle = -1;     
   }
}

WEBclientConnection::~WEBclientConnection(){
   COL_METHOD(~WEBclientConnection);
   cancelTimeout();
}

void WEBclientConnection::setTimeoutTimer(){
   //COL_METHOD(setTimeoutTimer);
   if (_TimeoutHandle != -1){
      COL_DBG("Resetting timeout timer.");
      _pLoop->cancelTimer(_TimeoutHandle);
   }
   _TimeoutHandle = _pLoop->addTimer(COLnewClosure0(this, &WEBclientConnection::timedOut), m_TimeoutLength);
   //COL_VAR(_TimeoutHandle);
}
   
void WEBclientConnection::timedOut(){
   COL_METHOD(WEBclientConnection::timedOut);
   _TimeoutHandle = -1;
   _Error = "Connection timed out.";
   if (_pSocket){
      COL_TRC("Closing socket");
      COL_VAR(_pSocket);
      _pSocket->hardClose();  // Hard close is important otherwise it will take a while to close
   }
}

void WEBclientConnection::sendResult(const WEBresponse& Result){
   COL_METHOD(sendResult);
   if (_pCallback.get()){
      COL_TRC("Sending result");
      // TODO: Since WEBconnection is not accessible outside of WEBclient::get(), we resort to try
      // catch throw (in callback) to close connections when log poller shuts down. Furthermore,
      // because a single callback function is used by onData, onClose and onError events, we have
      // to remove it below because otherwise it will mess up the operation counter in log poller
      // when called again later in hardClose().
      // The logic in this class should NOT be influenced by client code (i.e. log poller). We need
      // to redesign this class to at least expose the WEBconnections and allow dedicated callback
      // functions for various socket events.
      try {
         _pCallback->run(Result);
         _pCallback = NULL;
      } catch (const COLerror& Error) {
         COL_TRC("WEBcallbackFunc encountered an unexpected exception. Closing socket connection.");
         _pCallback = NULL;
         _pSocket->hardClose();
      }
   }
}


static void WEBhandleContentLength(WEBclientConnection* pConnection, const COLstring& Data){
   COL_FUNCTION(WEBhandleContentLength);
   // we are looking for the rest of the body.
   COL_VAR2(Data.size(), pConnection->_ContentLength);   
   if (Data.size() >= pConnection->_ContentLength){
      COL_TRC("Got all the data we need from this web request.");
      WEBresponse Response;
      Response.ReturnCode = pConnection->_ReturnCode;
      Response.Body       = pConnection->_Data;
      Response.Headers    = pConnection->_Header;
      pConnection->sendResult(Response);
      pConnection->cancelTimeout();
      pConnection->_pSocket->hardClose();
   } else {
      COL_TRC("Still waiting on more data.");
   }
}

static void WEBonChunk(COLstring* pChunk, bool LastChunk, WEBclientConnection* pConnection){
   COL_FUNCTION(WEBonChunk);
   pConnection->_CombinedChunks.append(*pChunk);
   pChunk->clear();        
   COL_VAR(LastChunk);
   if (LastChunk) {
      COL_TRC("Have the last chunk, now responding.");
      WEBresponse Response;
      Response.ReturnCode = pConnection->_ReturnCode;
      Response.Body       = pConnection->_Data;
      Response.Headers    = pConnection->_Header;
      pConnection->sendResult(Response);
   }
}

static void WEBonData(COLuint64 ConnectionId, COLstring* pIn, WEBclientConnection* pConnection){
   COL_FUNCTION(WEBonData);
   //COL_VAR(pConnection->_pSocket->bufferIn());
   COL_VAR(pConnection->_ContentLength);
   COLstring& Data = pConnection->_Data;  // TODO - do we really need to double buffer?  Eliot
   Data += *pIn;
   pIn->clear();
   int Position=0;
   pConnection->setTimeoutTimer();   // reset timeout timer since we have signs of life.
   // Are we still looking for the complete headers.
   if (pConnection->_ContentLength == -1) {
      if (Data.find(&Position, "\r\n\r\n")){
         COL_TRC("Found end of headers.");
         const char* pEnd = Data.c_str() + Position+4;
         const char* pCursor = Data.c_str() + WEBparseHttpLine(Data.c_str(),Position, &pConnection->_ReturnCode);
         WEBrequestParseHeaders(pCursor, pEnd, &pConnection->_Header);
         COL_TRC("Truncate the rest of the data");
         Data = Data.substr(Position+4);
         COL_VAR(pConnection->_Header);
         COLstring ContentLength;
         if (WEBheaderGetValue(pConnection->_Header, "Content-Length", &ContentLength)) {
            pConnection->_ContentLength = atoi(ContentLength.c_str());
            COL_VAR(pConnection->_ContentLength);
         }
         if (pConnection->_ReturnCode == 100) {
            // dealing with expect header 100-continue response
            pConnection->_ContentLength = 0;
         }
         COL_VAR(pConnection->_ContentLength);
         COLstring TransferEncoding;
         if (WEBheaderGetValue(pConnection->_Header, "Transfer-Encoding", &TransferEncoding)) {
            if (TransferEncoding == "chunked"){
               COL_TRC("We have chunked transfer encoding.");
               pConnection->_ContentLength = 0;  // Content-Length does not always exist for chunked encoding.
               COL_VAR(pConnection->_ContentLength);
               pConnection->_ChunkedContentEncoding = true;
               pConnection->m_ChunkParser.setChunkCallback(COLnewClosure2(&WEBonChunk, pConnection));
               pConnection->m_ChunkParser.reset();
            }
         }
      } else {
         COL_TRC("Haven't got headers yet.");
         return;
      }
   } 
   if (pConnection->_ChunkedContentEncoding){
      COL_TRC("Adding " << Data.size() << " bytes to chunked encoding parser.");
      if (pConnection->m_ChunkParser.parseData(Data.c_str(), Data.size(), &pConnection->m_Chunk)){
         pConnection->_FoundLastChunk = true;
         pConnection->_pSocket->close();
      }
      Data.clear();
      // TODO - handle chunked parsing error - or do we just rely on timeout?
   } else {
      WEBhandleContentLength(pConnection, Data);   
   }
}

static void WEBclose(COLuint64 ConnectionId, WEBclientConnection* pConnection){
   COL_FUNCTION(WEBclose);
   if (pConnection->_pSocket->lastError() || pConnection->_Error.is_null()){
      pConnection->_Error = SCKsocketError(pConnection->_pSocket->lastError());
   }
   int ContentLength = pConnection->_ContentLength;
   pConnection->cancelTimeout();
   if (ContentLength == -1){
      COL_TRC("Connection closed due to error: " << pConnection->_Error);
      WEBresponse Result;
      Result.ReturnCode = -1;
      Result.Body = "Connection closed due to error: " + pConnection->_Error;
      WEBsetConnectionErrorHeaders(&Result);
      pConnection->sendResult(Result);
   }
   else if (pConnection->_ChunkedContentEncoding && !pConnection->_FoundLastChunk) {
      // Note: in this case, ContentLength = 0
      COL_TRC("Chunked transfer connection closed.");
      WEBresponse Result;
      Result.ReturnCode = -1;
      Result.Body = "Connection closed due to error: " + pConnection->_Error;
      WEBsetConnectionErrorHeaders(&Result);
      pConnection->sendResult(Result);
   }
   COL_TRC("Connection closed.");
   delete pConnection;
}


// TODO 
// Run under valgrind.
// Add support for persistent connections 
// Add support for chunked content.
// Document SFI library
// Document COLvar map traversal.
// Document COLauto
// Document DOM library.

void WEBsetCallbacks(SCKconnection* pConnection, WEBclientConnection* pClient){
   COL_FUNCTION(WEBsetCallbacks);
   pConnection->m_pOnClose   =COLnewClosure1(&WEBclose, pClient);
   pConnection->m_pOnData    =COLnewClosure2(&WEBonData, pClient);
}

void WEBsetConnectionErrorHeaders(WEBresponse* pResponse){
   COL_FUNCTION(WEBsetConnectionErrorHeaders);
   pResponse->Headers["Content-Type"]   = "text/html";
   pResponse->Headers["Content-Length"] = COLintToString(pResponse->Body.size());
}
