//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: LQAPIcount.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   01/12/23 3:53 PM
//  ---------------------------------------------------------------------------
#include <LQAPI/LQAPIcount.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAopen.h>

#include <SDB/SDBcapi.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int LQAPIcount(lua_State* L){
   COL_FUNCTION(LCOMdescription);
   COLstring Guid = LUAcomponentId(L);
   int Count = SDBIgetQueueCount(Guid);
   lua_pushinteger(L, Count);
   return 1;
}