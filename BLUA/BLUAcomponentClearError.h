#ifndef __BLUA_COMPONENT_CLEAR_ERROR_H__
#define __BLUA_COMPONENT_CLEAR_ERROR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAcomponentClearError
//
// Description:
//
// Clears errors from a specific component
//
// Author: Eliot Muir 
// Date:   Thursday 13 April 2023 - 01:05PM
// ---------------------------------------------------------------------------

class COLwebRequest;
struct CFGmap;
class SCKloop;
class BLUAinstanceManager;

void BLUAcomponentClearError(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap);

#endif // end of defensive include
