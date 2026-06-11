//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERpeers.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 9:58 PM
//  ---------------------------------------------------------------------------
#include <PEER/PEERpeers.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void PEERpeers(const COLwebRequest& Request, PEERmap* pPeerMap) {
   COL_FUNCTION(PEERpeers);
   const COLvar MapVar = PEERmapToVar(*pPeerMap);
   COL_VAR(MapVar.json(1));
   COLrespondSuccess(Request.Address, MapVar);
}