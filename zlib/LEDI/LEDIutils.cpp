// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LEDIcache
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 13 June 2023 - 02:04PM
// ---------------------------------------------------------------------------
#include <LEDI/LEDIutils.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <LUA/LUAopen.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring LEDIresolveVmdFile(lua_State* L){
   COL_FUNCTION(LEDIresolveVmdFile);
   COLstring Vmd;
   COLstring IncomingFilePath = FILexpand(LUAexpectedStringParam(L, 1, "vmd"));
   if(FILpathIsAbsolute(IncomingFilePath)) { 
      Vmd = IncomingFilePath;
   } else { 
      Vmd = LUAprojectRoot(L) + IncomingFilePath;
   }
   return Vmd;
}
