#ifndef __BLUA_AUTO_START_H__
#define __BLUA_AUTO_START_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAautoStart
//
// Author: John Q
//
// Description:
//
// Auto start components on Iguana startup
//---------------------------------------------------------------------------
#include <COL/COLarray.h>
class COLclosure0;
class COLstring;
class BLUAinstanceManager;
class SCKloop;
struct CFGmap;

void BLUAstartComponents(COLclosure0* pDone, SCKloop* pLoop, CFGmap* pMap, BLUAinstanceManager* pManager);

// for unit testing
bool BLUAautoStartEnabled(const COLarray<COLstring>& Tags);

#endif // end of defensive include
