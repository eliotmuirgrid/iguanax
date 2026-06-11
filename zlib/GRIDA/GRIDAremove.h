#ifndef _GRIDAPIREMOVE_H
#define _GRIDAPIREMOVE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiRemove.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;

void GRIDAremovePeer(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif  // end of defensive include
