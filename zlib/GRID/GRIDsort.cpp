//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDsort.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <GRID/GRIDnode.h>
#include <GRID/GRIDsort.h>
COL_LOG_MODULE;

static bool GRIDcompareName(const COLvar& Peer1, const COLvar& Peer2, bool Ascending) {
   COL_FUNCTION(GRIDcompareName);
   const COLstring Name1  = Peer1["name"].asString();
   const COLstring Name2  = Peer2["name"].asString();
   const bool      Result = Name1 > Name2;
   return Result == Ascending;
}

static int GRIDgetIntOrDefault(const COLvar& Peer, const COLstring& Field) {
   COL_FUNCTION(GRIDgetIntOrDefault);
   const COLvar& Value = Peer[Field];
   if(Value.isNull()) { return -1; }
   return Value.asInt();
}

static bool GRIDcompareIntField(const COLvar& Peer1, const COLvar& Peer2, const COLstring& Field, const bool Ascending) {
   COL_FUNCTION(GRIDcompareIntField);
   const int  Val1   = GRIDgetIntOrDefault(Peer1, Field);
   const int  Val2   = GRIDgetIntOrDefault(Peer2, Field);
   const bool Result = Val1 > Val2;
   return Result == Ascending;
}

static int GRIDgetStatusRank(GRIDnode* pNode, const COLstring& PeerId) {
   COL_FUNCTION(GRIDgetStatusRank);
   const COLhashmap<COLstring, GRIDpeerInfo>& Peers = pNode->getPeers();
   auto It = Peers.find(PeerId);
   if(It == Peers.end()) { return 1; }
   const GRIDpeerInfo& Info        = It->second;
   bool                IsConnected = false;
   if(Info.pConnection) { IsConnected = Info.pConnection->connected(); }
   if(!IsConnected) {
      if(!Info.LastError.is_null()) { return 0; }
      return 1;
   }
   return 2;
}

static COLuint64 GRIDgetLatency(GRIDnode* pNode, const COLstring& PeerId) {
   COL_FUNCTION(GRIDgetLatency);
   const COLhashmap<COLstring, GRIDpeerInfo>& Peers = pNode->getPeers();
   auto It = Peers.find(PeerId);
   if(It == Peers.end()) { return 0; }
   const GRIDpeerInfo& Info = It->second;
   if(Info.pConnection) { return Info.pConnection->getLastRtt(); }
   return 0;
}

static bool GRIDcompareStatus(GRIDnode* pNode, const COLstring& Id1, const COLstring& Id2, const bool Ascending) {
   COL_FUNCTION(GRIDcompareStatus);
   const int Rank1 = GRIDgetStatusRank(pNode, Id1);
   const int Rank2 = GRIDgetStatusRank(pNode, Id2);
   COL_VAR2(Rank1, Rank2);
   if(Rank1 != Rank2) {
      const bool Result = Rank1 > Rank2;
      return Result != Ascending;
   }
   const COLuint64 Latency1 = GRIDgetLatency(pNode, Id1);
   const COLuint64 Latency2 = GRIDgetLatency(pNode, Id2);
   const bool      Result   = Latency1 > Latency2;
   return Result == Ascending;
}

void GRIDsortPeers(COLvar* pResponse, GRIDnode* pNode, const COLstring& SortBy, const bool Ascending) {
   COL_FUNCTION(GRIDsortPeers);
   if(pResponse->size() <= 1) { return; }
   const int Size = pResponse->size();
   COL_VAR2(Size, Ascending);
   for(int I = 0; I < Size - 1; I++) {
      for(int J = 0; J < Size - I - 1; J++) {
         bool Swap;
         const COLstring IdJ = (*pResponse)[J]["id"].asString();
         const COLstring IdJ1 = (*pResponse)[J + 1]["id"].asString();
         COL_VAR2(IdJ, IdJ1);
         if(SortBy == "name") {
            Swap = GRIDcompareName((*pResponse)[J], (*pResponse)[J + 1], Ascending);
         } else if(SortBy == "errors" || SortBy == "queue" || SortBy == "throughput") {
            Swap = GRIDcompareIntField((*pResponse)[J], (*pResponse)[J + 1], SortBy, Ascending);
         } else {
            Swap = GRIDcompareStatus(pNode, IdJ, IdJ1, Ascending);
         }
         COL_VAR(Swap);
         if(Swap) {
            COL_TRC("Swapping indices " << J << " and " << (J + 1));
            const COLvar Temp   = (*pResponse)[J];
            (*pResponse)[J]     = (*pResponse)[J + 1];
            (*pResponse)[J + 1] = Temp;
         }
      }
   }
}