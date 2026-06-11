#ifndef __LUACOM_DLL_H__
#define __LUACOM_DLL_H__

#include "lua.h"
#include "luacom.h"

FILE* tLuaGetLogFile();
void tLuaSetLogFile(FILE*);

LUACOM_API int luacom_openlib(lua_State* L);

#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
LUACOM_API int luaopen_luacom(lua_State* L);
#endif

#endif // __LUACOM_DLL_H__
