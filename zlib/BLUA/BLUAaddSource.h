#ifndef __BLUA_ADD_SOURCE_H__
#define __BLUA_ADD_SOURCE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc. All Rights Reserved
//
// Module: BLUAaddSource
//
// Author: Matthew Sobkowski
//
// Adds a component to the source list
//---------------------------------------------------------------------------

class BLUAinstanceManager;
class SCKloop;
class COLwebRequest;
struct CFGmap;

void BLUAaddComponentSource(const COLwebRequest& Request, BLUAinstanceManager* pBluaManager, CFGmap* pMap, SCKloop* pLoop);

#endif