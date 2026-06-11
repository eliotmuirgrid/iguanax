#ifndef PEERPEERS_H
#define PEERPEERS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERpeers.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 9:58 PM
//  ---------------------------------------------------------------------------
#include <PEER/PEERmap.h>
#include <COL/COLweb.h>

class SCKloop;

void PEERpeers(const COLwebRequest& Request, PEERmap*  pPeerMap);

#endif // end of defensive include
