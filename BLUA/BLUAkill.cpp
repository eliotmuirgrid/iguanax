//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAkill
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Wednesday Oct 5 2022
//---------------------------------------------------------------------------
#include <BLUA/BLUAkill.h>
#include <BLUA/BLUAtask.h>
#include <COL/COLlog.h>
#include <LUA/LUAheader.h>
#ifdef _WIN32
#include <LUACOM/LUACOMcreateDestroy.h>
#endif
COL_LOG_MODULE;

BLUAtask BLUAkillTask() {
   BLUAtask Task;
   Task.Id = BLUA_KILL;
   return Task;
}

// LUA THREAD
void BLUAhandleKill(lua_State*& L, const COLvar& Payload, int* pResult) {
   COL_FUNCTION(BLUAhandleKill);
   COLASSERT(L);
   #ifdef _WIN32
   LUACOMdestroyLuacomWrapper(L, true);
   #endif
   lua_close(L);
   L = NULL;
   *pResult = BLUA_TERMINATE;
}
