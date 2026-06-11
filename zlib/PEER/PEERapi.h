#ifndef PEERAPI_H
#define PEERAPI_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERapi.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>

class IWEBconfig;
class PEERmap;
class SCKloop;

void PEERapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, PEERmap* pPeerMap,
             IWEBconfig* pWebConfig, SCKloop* pLoop);

#endif // end of defensive include
