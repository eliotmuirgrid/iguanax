#ifndef __SDBA_LOG_QUEUE_POSITION_H__
#define __SDBA_LOG_QUEUE_POSITION_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAlogQueuePosition
//
// Description:
//
// Blocking API to reposition a queue consumer.
//
// Author: Eliot Muir
// Date:   Friday 05 May 2023 - 03:49PM
// ---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;
struct CFGmap;
class SDBsystem;

void SDBAlogQueuePosition(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pBluaManager, SDBsystem* pSystem);

#endif // end of defensive include
