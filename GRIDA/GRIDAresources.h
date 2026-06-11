#ifndef _GRIDAPIPEERSTATS_H
#define _GRIDAPIPEERSTATS_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiPeerStats.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAresources(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif  // end of defensive include
