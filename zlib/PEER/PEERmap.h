#ifndef PEERMAP_H
#define PEERMAP_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERmap.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>

class COLstring;
class SCKconnection;

struct PEERnode {
   bool           Connected   = false;
   SCKconnection* pConnection = nullptr;
   COLstring      Address;
   COLstring      Host;
   int            Port;
   COLstring      Guid;
};

class PEERmap {
  public:
   COLhashmap<COLstring, COLstring> m_PeerMap; // <Guid, Address>
   COLhashmap<COLstring, PEERnode*> m_ConnectionMap;
};

bool PEERmapAddressExists(const PEERmap& PeerMap, const COLstring& Address);
COLvar PEERmapToVar(const PEERmap& PeerMap);

#endif // end of defensive include
