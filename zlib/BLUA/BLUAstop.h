#ifndef __BLUA_STOP_H__
#define __BLUA_STOP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAstop
//
// Description:
//
// Code to stop a component
//
// Author: Vismay Shah - refactored by Eliot
// Date:   Tuesday Aug 16 2022
//---------------------------------------------------------------------------

class BLUAinstance2;
class BLUAinstanceManager;
class COLstring;
class COLwebRequest;
class SCKloop;
struct CFGmap;

void BLUAinstanceStopped(BLUAinstance2* pInstance, BLUAinstanceManager* pManager);
void BLUAinstanceDelete(BLUAinstance2* pInstance, bool Force = false);
void BLUAinstanceStop(BLUAinstanceManager* pManager, const COLstring& ComponentId, bool Force = false);
void BLUAcomponentStop(const COLwebRequest& Request, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop);
void BLUAcomponentStopCheck(const COLwebRequest& Request);
void BLUAinstanceScheduleAllStop(BLUAinstance2* pInstance);

#endif // end of defensive include