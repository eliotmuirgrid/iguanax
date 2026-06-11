#ifndef PEERSOCKET_H
#define PEERSOCKET_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERsocket.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/01/25 9:57 AM
//  ---------------------------------------------------------------------------
#include <PEER/PEERmap.h>

class SCKloop;

bool PEERsocketInit(PEERmap* pPeerMap, PEERnode* pNode, const COLstring& Address, SCKloop* pLoop);

#endif // end of defensive include