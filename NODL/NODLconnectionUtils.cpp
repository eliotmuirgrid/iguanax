// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLconnectionUtils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 1 December 2022
// ---------------------------------------------------------------------------
#include "NODLconnectionUtils.h"
#include "NODLconstants.h"
#include "NODLutils.h"

#include <COL/COLlog.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
#include <LUAC/lua.h>
COL_LOG_MODULE;

//Check whether any parameters in current method call aren't in either Params or standard connection parameters.
//NewConnection should be false when using an existing connection, and false otherwise.
void NODLcheckCombinedParams(lua_State* L, int i, bool NewConnection, const char* const Params[]){
   COL_FUNCTION(NODLcheckCombinedParams);
   const char* NODL_PARAMS_TEMP[64];
   int ParamIndex = 0;
   const char* const* opt = NewConnection ? NODL_NEW_CONNECT_PARAMS : NODL_OLD_CONNECT_PARAMS;
   for( ; *opt; opt++, ParamIndex++) {
      NODL_PARAMS_TEMP[ParamIndex] = *opt;
   }
   for(opt=Params; *opt; opt++, ParamIndex++) {
      NODL_PARAMS_TEMP[ParamIndex] = *opt;
   }
   NODL_PARAMS_TEMP[ParamIndex] = NULL;
   NODLcheckParams(L,i,NODL_PARAMS_TEMP);
}

