//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAidleConnection
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------
#include <BLUA/BLUAidleConnection.h>

#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAtask.h>


#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static BLUAtask BLUAremoveIdleConnectionTask() {
   BLUAtask Task;
   Task.Id = BLUA_RM_IDLE;
   return Task;
}

// LUA THREAD
void BLUAhandleRemoveIdleConnection(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleRemoveIdleConnection);
   if (L) {
      LUArunLuaIdleCallBack(L);
   }
   *pResult = 0;
}

static void BLUApurgeIdleConnections(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUApurgeIdleConnections);
   pInstance->m_JobQueue.put(BLUAremoveIdleConnectionTask());
}

// called from main thread
void BLUAstartIdleConnectionPurgeTimer(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAstartIdleConnectionPurgeTimer);
   pInstance->m_IdleTimer = pInstance->m_pLoop->addTimer(COLnewClosure0(&BLUApurgeIdleConnections, pInstance), 2000, true);
}

void BLUAcancelIdleConnectionPurgeTimer(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAcancelIdleConnectionPurgeTimer);
   pInstance->m_pLoop->cancelTimer(pInstance->m_IdleTimer);
   pInstance->m_IdleTimer = 0;
}