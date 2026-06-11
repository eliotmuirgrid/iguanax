//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERmap.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <PEER/PEERmap.h>
COL_LOG_MODULE;

bool PEERmapAddressExists(const PEERmap& PeerMap, const COLstring& Address) {
   COL_FUNCTION(PEERmapAddressExists);
   for(const auto& it : PeerMap.m_PeerMap) {
      if(it.second == Address) { return true; }
   }
   return false;
}

COLvar PEERmapToVar(const PEERmap& PeerMap) {
   COL_FUNCTION(PEERmapToVar);
   COLvar Result;
   Result.setArrayType();
   for(const auto& it : PeerMap.m_PeerMap) { Result.push_back(it.first); }
   COL_VAR(Result.json(1));
   return Result;
}