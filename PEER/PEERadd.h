#ifndef PEERADD_H
#define PEERADD_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERadd.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   21/01/25 3:29 PM
//  ---------------------------------------------------------------------------
#include <PEER/PEERmap.h>

class COLwebRequest;
class SCKloop;

void PEERadd(const COLwebRequest& Request, PEERmap*  pPeerMap, SCKloop* pLoop);


#endif // end of defensive include
