//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LX12api.cpp
//
//  Description:
//
//  Implementation
//
//  Author: Matthew Sobkowski
//  Date:   3/8/23 1:55 PM
//  ---------------------------------------------------------------------------
#include <LX12/LX12api.h>
#include <LX12/LX12parse.h>
#include <LX12/LX12message.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void LX12api(lua_State* L){
   COL_FUNCTION(LHL7api);
   lua_newtable (L);
   LUAaddMethod(L, "parse",        &LX12parseMessage);
   LUAaddMethod(L, "message",      &LX12newMessage);
   lua_setglobal(L, "x12");
}