// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHL7api
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Friday 07 October 2022 - 08:30AM
// ---------------------------------------------------------------------------

#include <LHL7/LHL7api.h>
#include <LHL7/LHL7mapping.h>
#include <LHL7/LHL7message.h>
#include <LHL7/LHL7parse.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LHL7api(lua_State* L){
   COL_FUNCTION(LHL7api);
   lua_newtable (L);
   LUAaddMethod(L, "message",      &LHL7newMessage);
   // LUAaddMethod(L, "mapping",      &LHL7mapping);
   LUAaddMethod(L, "parse",        &LHL7parseMessage);
   lua_setglobal(L, "hl7");
}