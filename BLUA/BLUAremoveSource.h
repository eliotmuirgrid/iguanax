#ifndef __BLUA_REMOVE_SOURCE_H__
#define __BLUA_REMOVE_SOURCE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: BLUAremoveSource
//
// Author: Matthew Sobkowski
//
// Removes a component from source list
//---------------------------------------------------------------------------

class BLUAinstanceManager;
class COLwebRequest;
class SCKloop;
struct CFGmap;

void BLUAremoveComponentSource(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop);

#endif