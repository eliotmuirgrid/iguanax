// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNconnectionCheck
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 08 June 2023 - 01:45PM
// ---------------------------------------------------------------------------

#include <TRN/TRNconnectionCheck.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNrequestCheckValid.h>

#include <LUA/LUAutils.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void TRNsetConnectionIdInLua(lua_State* L, COLuint64 ConnectionId){
   // COL_FUNCTION(TRNsetConnectionIdInLua); //noisy
   COL_VAR(ConnectionId);
   TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
   pAnnotationState->m_WebConnectionId = ConnectionId;
}

COLuint64 TRNconnectionIdFromLua(lua_State* L){
   // COL_FUNCTION(TRNconnectionIdFromLua); //noisy
   TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
   return pAnnotationState->m_WebConnectionId;
}

// Sets the current time in the TRNannotationState
void TRNconnectionCheckTimeRegister(lua_State* L){
   COL_FUNCTION(TRNconnectionCheckTimeRegister);
   TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
   COLtimeStamp* pNow = &pAnnotationState->m_TimeStamp;
   COLcurrentTimeStamp(pNow);
}

bool TRNconnectionCheck(lua_State* L){
   // COL_FUNCTION(TRNconnectionCheck); // super noisy.
   TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
   COLtimeStamp LastCheckTime = pAnnotationState->m_TimeStamp;
   COLtimeStamp Now;
   COLcurrentTimeStamp(&Now);
   // If more than a quarter of a second has passed since the last check, lets check the connection again.
   if (COLtimeStampDiffInMillisecond(Now, LastCheckTime) > 250){
      COL_TRC("More than 0.25 seconds has passed, is the connection still alive?");
      COLuint64 ConnectionId = TRNconnectionIdFromLua(L);
      COL_VAR(ConnectionId);
      if (!TRNcheckRequestStillValid(ConnectionId)) {
         COL_TRC("Connection is gone, exit.");
         // throw COLerror("Connection to browser has gone away, abandon annotation job");
         return false;
      } else {
         COL_TRC("Connection still alive, update the last check time");
         TRNconnectionCheckTimeRegister(L);
      }
   }
   return true;
}

