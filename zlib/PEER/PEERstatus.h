#ifndef PEERSTATUS_H
#define PEERSTATUS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERstatus.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/01/25 11:57 AM
//  ---------------------------------------------------------------------------
#include <PEER/PEERmap.h>

class COLwebRequest;
class SCKloop;

void PEERstatus(const COLwebRequest& Request, PEERmap*  pPeerMap, SCKloop* pLoop);


#endif // end of defensive include
