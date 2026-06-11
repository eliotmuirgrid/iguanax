#ifndef __BLUA_COMPONENT_FUNCTION_CALL_H__
#define __BLUA_COMPONENT_FUNCTION_CALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcomponentFunctionCall
//
// Description:
// Generic component function call web API handler.
//
// Author: iNTERFACEWARE Inc.
// ---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;
class SCKloop;
struct CFGmap;

void BLUAcomponentFunctionCall(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
