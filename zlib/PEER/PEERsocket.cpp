//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERsocket.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/01/25 9:57 AM
//  ---------------------------------------------------------------------------
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <PEER/PEERsocket.h>
#include <SCK/SCKconnection.h>
#include <SCK/SCKloop.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBurl.h>
#include <COL/COLvar.h>
COL_LOG_MODULE;

static void PEERsocketOnAsyncClose(COLuint64 ConnId, PEERmap* pPeerMap, PEERnode* pNode) {
   COL_FUNCTION(PEERsocketOnAsyncClose);
   COL_TRC("Connection to " << pNode->Address << " has been closed");
   if(pPeerMap->m_ConnectionMap.count(pNode->Guid) == 0) { return; }
   COL_TRC("Freeing PEERnode* " << pNode->Guid);
   delete pPeerMap->m_ConnectionMap[pNode->Guid];
   COL_TRC("Removing " << pNode->Guid << " from connection map");
   pPeerMap->m_ConnectionMap.removeKey(pNode->Guid);
}

static void PEERsocketOnAsyncWritable(COLuint64 ConnectionId) {
   COL_FUNCTION(PEERsocketOnAsyncWritable);
   COL_TRC("Writable event");
}

static void PEERsocketReadData(COLstring* pData, WEBresponse* pResponse) {
   COL_FUNCTION(PEERsocketReadData);
   COLmap<COLstring, COLstring> Headers;
   int                          Position, ContentLength = 0;
   if(pData->find(&Position, "\r\n\r\n")) {
      COL_TRC("Found end of headers.");
      const char* pEnd    = pData->c_str() + Position + 4;
      const char* pCursor = pData->c_str() + WEBparseHttpLine(pData->c_str(), Position, &pResponse->ReturnCode);
      WEBrequestParseHeaders(pCursor, pEnd, &Headers);
      COL_TRC("Truncate the rest of the data");
      *pData = pData->substr(Position + 4);
      COL_VAR(Headers);
      COLstring ContentLengthStr;
      if(WEBheaderGetValue(Headers, "Content-Length", &ContentLengthStr)) {
         ContentLength = atoi(ContentLengthStr.c_str());
         COL_VAR(ContentLength);
      }
      pData->setSize(ContentLength);
      pResponse->Body = *pData;
   }
}

static void PEERsocketOnAsyncData(COLuint64 ConnId, COLstring* pData, PEERmap* pPeerMap, PEERnode* pNode) {
   COL_FUNCTION(PEERsocketOnAsyncData);
   COL_TRC("Received data from " << pNode->Address << "\n" << *pData);
   WEBresponse Response;
   PEERsocketReadData(pData, &Response);
   COL_VAR2(Response.ReturnCode, Response.Body);
   if(Response.ReturnCode != 200) {
      COL_TRC("Not 200 code -- " << Response.ReturnCode);
      // TODO what to do?
      return;
   }
   COLvar Json;
   Json.parse(Response.Body);
   COL_VAR(Json.json(1));
   if(Json["data"]["endpoint"].asString() == "/peers/server_guid") {
      pNode->Guid = Json["data"]["guid"];
      pPeerMap->m_ConnectionMap.add(pNode->Guid, pNode);
      pPeerMap->m_PeerMap.add(pNode->Guid, pNode->Address);
      return;
   }
}

static void PEERsocketOnAsyncConnect(COLuint64 ConnId, PEERmap* pPeerMap, PEERnode* pNode) {
   COL_FUNCTION(PEERsocketOnAsyncConnect);
   COL_TRC("Successfully established connection to " << pNode->Address);
   pNode->Connected = true;
   WEBrequest Request;
   Request.Timeout = 3000;
   WEBurl Url;
   Url.Host = pNode->Host;
   Url.Port = pNode->Port;
   Url.Path = "/peers/server_guid";
   COLstring Data;
   WEBformatGetRequest(Request, Url, &Data);
   pNode->pConnection->write(Data.c_str(), Data.size());
}

static bool PEERsocketCheckAddress(const COLstring& Address, PEERnode* pNode) {
   COL_FUNCTION(PEERsocketCheckAddress);
   COLstring Port;
   if(!Address.rsplit(pNode->Host, Port, ":")) {
      COL_TRC("Received bad address: " << Address);
      return false;
   }
   pNode->Port = atoi(Port.c_str());
   return true;
}

bool PEERsocketInit(PEERmap* pPeerMap, PEERnode* pNode, const COLstring& Address, SCKloop* pLoop) {
   COL_FUNCTION(PEERsocketInit);
   pNode->pConnection                = new SCKconnection(*pLoop);
   pNode->Address                    = Address;
   pNode->pConnection->m_pOnConnect  = COLnewClosure1(&PEERsocketOnAsyncConnect, pPeerMap, pNode);
   pNode->pConnection->m_pOnData     = COLnewClosure2(&PEERsocketOnAsyncData, pPeerMap, pNode);
   pNode->pConnection->m_pOnWritable = COLnewClosure1(&PEERsocketOnAsyncWritable);
   pNode->pConnection->m_pOnClose    = COLnewClosure1(&PEERsocketOnAsyncClose, pPeerMap, pNode);
   if(PEERsocketCheckAddress(Address, pNode)) {
      COL_VAR2(pNode->Host, pNode->Port);
      pNode->pConnection->connect(pNode->Host, pNode->Port);
      return true;
   }
   COL_TRC("Bad address -- could not resolve host:port");
   return false;
}