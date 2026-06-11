// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LCOMstopping
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 24 July 2023 - 11:59AM
// ---------------------------------------------------------------------------

#include <LCOM/LCOMstopping.h>

#include <LUA/LUAstartup.h>
#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <TRN/TRNannotationState.h>
#include <BLUA/BLUAinstance.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LCOMisStopping(lua_State* L){
   COL_FUNCTION(LCOMisStopping);
   if (LUALisEnvironmentStartup(L)) { 
      // In startup mode, there's no such thing as stopping.
      lua_pushboolean(L, false);
      return 1;
   }
   if (LUALisEnvironmentTest(L)){
      // pAnnotationState->m_Cancelled is true when a new translator request comes in and we cancel the existing one. 
      // In an interruptible Lua call, if we detect that m_Cancelled is true, we call LUAraiseError.
      // This means this api in translator should always return false.
      TRNannotationState* pAnnotationState = TRNgetAnnotationState(L);
      COL_VAR(pAnnotationState->m_Cancelled);
      lua_pushboolean(L, pAnnotationState->m_Cancelled);
   } else { 
      // pInstance->m_IsStopping is set to true when the component switch is changed to off. If not a force stop, we wait
      // for the script to finish executing, so there is a chance that this returns true.
      BLUAinstance2* pInstance = BLUAinstanceGet(L);
      COL_VAR(pInstance->m_IsStopping);
      lua_pushboolean(L, pInstance->m_IsStopping);
   }
   return 1;
}
