#ifndef BLUAIGUANAEVENT_H
#define BLUAIGUANAEVENT_H
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: BLUAiguanaEvent.h
//
//  Description: Attempts to call the designated function if exists in each component
//
//  Author: Matthew Sobkowski
//  Date:   24/03/25 10:15 AM
//  ---------------------------------------------------------------------------
class COLstring;
class COLclosure0;
class BLUAinstanceManager;
class SCKloop;

struct CFGmap;

enum BLUAiguanaEventType {
   STARTUP,
   SHUTDOWN,
};

void BLUAiguanaEvent(COLclosure0* pDone, SCKloop* pLoop, CFGmap* pMap, BLUAiguanaEventType EventType);

#endif // end of defensive include
