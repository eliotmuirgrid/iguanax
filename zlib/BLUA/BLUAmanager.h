#ifndef __BLUA_TRANSLATOR_H__
#define __BLUA_TRANSLATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAtranslatorInstance
//
// Description:
//    Functions that start/stop BLUAinstances.
//
// Author: Shuo Hou
// Date:   Friday Sept 3 2021
//---------------------------------------------------------------------------
#include <BLUA/BLUAinstance.h>
#include <COL/COLweb.h>
#include <COL/COLmap.h>

struct CFGconfig;
struct CFGmap;
class SCKloop;

class BLUAinstanceManager {
public:
   bool m_ShutdownRequested { false };
   COLauto<COLclosure0> m_pOnShutdown;
   COLmap<COLstring, COLauto<BLUAinstance2>> m_Instances;
};

bool BLUAallComponentsStopped(const BLUAinstanceManager& Manager);
bool BLUAinstanceRunning(const BLUAinstanceManager& Manager, const COLstring& ComponentId);
void BLUAinstanceStopAll(COLclosure0* pCallback, BLUAinstanceManager* pManager);
void BLUAinstanceStopAllWithoutCleanup(COLclosure0* pCallback, BLUAinstanceManager* pManager);

#endif // end of defensive include