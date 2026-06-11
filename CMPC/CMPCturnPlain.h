#ifndef __CMPC_TURN_PLAIN_H__
#define __CMPC_TURN_PLAIN_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCturnPlain
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 25 October 2024 - 12:38PM
// ---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;
struct CFGmap;
class SCKloop;

void CMPCremoveTracking(const COLstring& Component);
void CMPCturnPlain(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop);
   
#endif // end of defensive include
