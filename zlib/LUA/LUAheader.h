#ifndef __LUA_HEADER_H__
#define __LUA_HEADER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAheader
//
// Description:
//
// LUAheader - wrapper around main Lua headers.  These are the only three public
// header files for Lua.
//
// Author: Eliot Muir
// Date:   Tuesday, June 22nd, 2010 @ 04:12:27 PM
//---------------------------------------------------------------------------

extern "C" {
#if defined(CHM_JIT1)
   // win32 - use luajit_1_1_6
   #include <luajit_1_1_6/lua.h>
   #include <luajit_1_1_6/lauxlib.h>
   #include <luajit_1_1_6/lualib.h>
   #include <luajit_1_1_6/luajit.h>
#elif defined(CHM_JIT)
   // LuaJIT202
   #include <LuaJIT202/LuaJIT-2.0.2/src/lua.h>
   #include <LuaJIT202/LuaJIT-2.0.2/src/lauxlib.h>
   #include <LuaJIT202/LuaJIT-2.0.2/src/lualib.h>
   #include <LuaJIT202/LuaJIT-2.0.2/src/luajit.h>
#else
   // stock interpreted Lua
   #include <LUAC/lua.h>
   #include <LUAC/lauxlib.h>
   #include <LUAC/lualib.h>
#endif
}

#endif // end of defensive include
