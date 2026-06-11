#ifndef PEERDELETE_H
#define PEERDELETE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERdelete.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/01/25 12:14 PM
//  ---------------------------------------------------------------------------
#include <PEER/PEERmap.h>

class COLwebRequest;
class SCKloop;

void PEERdelete(const COLwebRequest& Request, PEERmap*  pPeerMap, SCKloop* pLoop);

#endif // end of defensive include
