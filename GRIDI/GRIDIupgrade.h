#ifndef MAIN2PROJECT_GRIDIUPGRADE_H
#define MAIN2PROJECT_GRIDIUPGRADE_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GRIDIupgrade.h
//
//  Description: WebSocket upgrade handler for GRID connections.
//  Parses upgrade request headers and begins auth challenge/response routine.
//  Triggered by Upgrade: websocket header on any path.
//
//  Author: Matthew Sobkowski
//  ---------------------------------------------------------------------------
class WEBrequestData;
class GRIDnode;
class SCKloop;

void GRIDIprocessWsUpgradeRequest(const WEBrequestData& Request, GRIDnode* pNode, SCKloop* pLoop);

#endif  // end of defensive include
