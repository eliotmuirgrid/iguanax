//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAtask
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------
#include "BLUAtask.h"

#include <BLUA/BLUAcall.h>
#include <BLUA/BLUAinstance.h>
#include <BLUA/BLUAkill.h>
#include <BLUA/BLUAqueue.h>
#include <BLUA/BLUAidleConnection.h>
#include <BLUA/BLUArun.h>
#include <BLUA/BLUAsocket.h>
#include <BLUA/BLUAtimer.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void BLUAregisterTasks(BLUAinstance2* pInstance) {
   COL_FUNCTION(BLUAregisterTasks);
   COLmap<BLUAtaskId, COLauto<COLclosure3<lua_State*&, const COLvar&, int*>>>& TaskMap     = pInstance->m_TaskMap;
   TaskMap[BLUA_RUN]        = COLnewClosure3(&BLUAhandleRun, pInstance);
   TaskMap[BLUA_KILL]       = COLnewClosure3(&BLUAhandleKill);
   TaskMap[BLUA_RM_IDLE]    = COLnewClosure3(&BLUAhandleRemoveIdleConnection);
   TaskMap[BLUA_ON_CALL]    = COLnewClosure3(&BLUAhandleCall);
   TaskMap[BLUA_ON_DEQUEUE] = COLnewClosure3(&BLUAhandleDequeue);
   TaskMap[BLUA_ON_TIMER]   = COLnewClosure3(&BLUAhandleTimer);
   TaskMap[BLUA_ON_ACCEPT]  = COLnewClosure3(&BLUAhandleSocketOnAccept);
   TaskMap[BLUA_ON_CONNECT] = COLnewClosure3(&BLUAhandleSocketOnConnect);
   TaskMap[BLUA_ON_DATA]    = COLnewClosure3(&BLUAhandleSocketOnData);
   TaskMap[BLUA_ON_WRITE]   = COLnewClosure3(&BLUAhandleSocketOnWrite);
   TaskMap[BLUA_ON_CLOSE]   = COLnewClosure3(&BLUAhandleSocketOnClose);
   TaskMap[BLUA_HTTP_DATA]  = COLnewClosure3(&BLUAhandleHttpOnData);
}

COLostream& operator<<(COLostream& Stream, BLUAtaskId TaskItem) {
   switch (TaskItem) {
      case BLUA_RUN        : Stream << "RUN";             break;
      case BLUA_KILL       : Stream << "KILL";            break;
      case BLUA_RM_IDLE    : Stream << "RM_IDLE";         break;
      case BLUA_ON_CALL    : Stream << "ON_CALL";         break;
      case BLUA_ON_DEQUEUE : Stream << "ON_DEQUEUE";      break;
      case BLUA_ON_TIMER   : Stream << "ON_TIMER";        break;
      case BLUA_ON_ACCEPT  : Stream << "ON_ACCEPT";       break;
      case BLUA_ON_CONNECT : Stream << "ON_CONNECT";      break;
      case BLUA_ON_DATA    : Stream << "ON_DATA";         break;
      case BLUA_ON_WRITE   : Stream << "ON_WRITE";        break;
      case BLUA_ON_CLOSE   : Stream << "ON_CLOSE";        break;
      case BLUA_HTTP_DATA  : Stream << "ON_ HTTP_DATA";   break;
   }
   return Stream;
}

COLstring BLUAtaskResultAsString(int Result){
   COL_FUNCTION(BLUAtaskResultAsString);
   switch(Result){
   case BLUA_ERROR_INTERNAL: return "BLUA_ERROR_INTERNAL";
   case BLUA_ERROR_COMPILE : return "BLUA_ERROR_COMPILE";
   case BLUA_TERMINATE     : return "BLUA_TERMINATE";
   case BLUA_FORCE_KILLED  : return "BLUA_FORCE_KILLED";
   case BLUA_UNKNOWN       : return "BLUA_UNKNOWN";
   default:  return "OTHER - " + COLintToString(Result);
   }
}
