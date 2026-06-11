#ifndef _GRIDAPICONNECT_H
#define _GRIDAPICONNECT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDapiConnect.h
//
//  Description: API for establishing GRID connections.
//               GRIDapiConnectToPeer - initiates outgoing connection to peer
//               GRIDconnectHandler - accepts incoming WebSocket connection via WEB server
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class COLwebRequest;
class GRIDnode;
class SCKloop;
class WEBrequestData;

void GRIDAaddPeer(const COLwebRequest& Request, GRIDnode* pNode, SCKloop* pLoop);
void GRIDIprocessWsUpgradeRequest(const WEBrequestData& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif  // end of defensive include
