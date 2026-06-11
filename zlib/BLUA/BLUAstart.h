#ifndef __BLUA_START_H__
#define __BLUA_START_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAstart
//
// Description:
//
// Code to start a component
//
// Author: Vismay Shah - refactored by Eliot
// Date:   Tuesday Aug 16 2022
//---------------------------------------------------------------------------
#include <COL/COLarray.h>

class BLUAinstanceManager;
class BLUAinstance2;
class COLwebRequest;
class COLclosure0;
class SCKloop;

struct CFGconfig;
struct CFGmap;

void BLUAinstanceStarted(BLUAinstance2* pInstance, int WorkerId);
void BLUAinstanceStart(BLUAinstanceManager* pManager, CFGconfig* pConfig, SCKloop* pLoop, bool Resubmission = false);
void BLUAcomponentStart(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop);
COLstring BLUAstartComponentList(const COLarray<COLstring>& ComponentGuids, bool Tracked, BLUAinstanceManager* pManager, CFGmap* pMap,
                            SCKloop* pLoop, COLclosure0* pDone, int BatchSize = 10, int DelayMs = 100);
void BLUAcomponentStartCheck(const COLwebRequest& Request);
#endif  // end of defensive include