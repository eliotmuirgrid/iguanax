// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LTUNpeerList
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 07 July 2023 - 01:40PM
// ---------------------------------------------------------------------------

#include <LTUN/LTUNpeerList.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <SCK/SCKcall.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static bool LTUNcheckResponseStructure(const COLvar& Response){
   COL_FUNCTION(LTUNcheckResponseStructure);
   if (!Response.exists("data"))           return false; 
   if (!Response["data"].exists("peers"))  return false;
   if (!Response["data"]["peers"].isMap()) return false;
   return true;
}

int LTUNpeerList(lua_State* L){
   COL_FUNCTION(LTUNpeerList);
   COLvar Request;
   COLvar Result;
   bool ApiCallSuccess = SCKcall("/tunnel/status", Request, LUAcomponentId(L), &Result);
   COL_VAR(Result);
   if (!ApiCallSuccess || Result.exists("error")) { LUAraiseLuaError(L, "Failed to get peer information."); }
   if (!LTUNcheckResponseStructure(Result)) { 
      COL_TRC("Maybe the api changed?"); // TODO - remove once the api is concrete.
      LUAraiseLuaError(L, "Failed to get peer information in the correct form."); 
   }

   COLvar Peers = Result["data"]["peers"];
   Peers.toLua(L);
   return 1;
}


