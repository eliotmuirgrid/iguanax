#ifndef _GRIDREQUEST_H
#define _GRIDREQUEST_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDrequest.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>

class COLaddress;
class COLstring;
class GRIDnode;
class GRIDwebSocket;
class SCKloop;

// make a request to all connected iguanas
void GRIDrequestAll(const COLstring& Api, const COLvar& Data, GRIDnode* pNode, SCKloop* pLoop, COLauto<COLclosure1<const COLvar&>> pCallback);

COLuint64 GRIDgenerateConnectionId(GRIDnode* pNode);

// Returns nullptr if not found or not connected.
GRIDwebSocket* GRIDfindConnectedPeer(GRIDnode* pNode, const COLstring& IguanaId);

void GRIDbuildRemoteRequestJson(COLvar& OutRequest, const COLstring& Api, const COLvar& Data, COLuint64 ConnectionId);

#endif  // end of defensive include
