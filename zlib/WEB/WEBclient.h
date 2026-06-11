#ifndef __WEB_CLIENT_H__
#define __WEB_CLIENT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: WEBclient
//
// Author: Eliot Muir
//
// Description:
//
// WEBclientConnection represents a SCKSsocketConnection used to implement HTTP(S) requests with a COLclosure
// type interface. These methods run asynchronously. Use with WEBasyncRequest's
// WEBrequestAsyncGet or WEBrequestAsyncPost which will automatically handle redirects.
//---------------------------------------------------------------------------
class COLstring;
class COLvar;
class SCKloop;
class WEBclientConnection;
class WEBrequest;
class WEBresponse;
class WEBurl;

#include "WEBchunkEncodingParser.h"

#include <SCKS/SCKSsslContext.h>
#include <SCKS/SCKSsslConnection.h>

#include <COL/COLclosure.h>
#include <COL/COLauto.h>
#include <COL/COLmap.h>

typedef COLauto< COLclosure1<const WEBresponse&> > WEBcallbackFunc;

class WEBclientConnection{
public:
   WEBclientConnection(WEBcallbackFunc pCallback, int Timeout, SCKloop* pLoop);
   ~WEBclientConnection();
   void cancelTimeout();
   void setTimeoutTimer();
   void timedOut();
   void sendResult(const WEBresponse& Result);
   SCKconnection* _pSocket;
   SCKSsslContext _SslContext;
   WEBcallbackFunc _pCallback;
   SCKloop* _pLoop;

   int _TimeoutHandle;
   int m_TimeoutLength;
   // Stores SCk error in WEBerror
   // Sent to WEBcallbackFunc in WEBclose
   COLstring _Error;
   // WEBresponse stuff
   int _ReturnCode;
   COLmap<COLstring, COLstring> _Header;
   COLstring _Data;
   // Chunked stuff
   int _ContentLength;
   bool _ChunkedContentEncoding;
   bool _FoundLastChunk;
   COLstring _CombinedChunks;
   WEBchunkEncodingParser m_ChunkParser;  
   COLstring              m_Chunk;
private:
   WEBclientConnection(const WEBclientConnection& Orig);
   WEBclientConnection& operator=(const WEBclientConnection& Orig);
};

void WEBsetCallbacks(SCKconnection* pConnection, WEBclientConnection* pClient);
void WEBsetConnectionErrorHeaders(WEBresponse* pResponse);

#endif // end of defensive include
