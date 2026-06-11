/*
 * Example of a native Lua module.
 */

#include <stdio.h>

// Make sure the compiler doesn't mangle the Lua library names.
// Technically, this is unnecessary building against our Lua
// headers, but it is a good habit to get into.
extern "C" {
   #include <LUA/LUAheader.h>
}

#define PROJECT_TABLENAME "myhello"

#undef LUA_API    // Quiet CPP warnings about redefinitions.
#if defined _WIN32 || defined _WIN32
#define LUA_API __declspec(dllexport)
#else
#define LUA_API
#endif

extern "C" {
   int LUA_API luaopen_myhello (lua_State *L);
}

/**
 * helloworld
 *
 * Returns a message on the stack for Lua to use.
 */
static int helloworld (lua_State *L) {
   const char message[] = "Hello, World!";
   lua_pushstring(L, message);
   return 1;
}

/*
 * 5.1 style driver function that loads the public functions into the module table.
 * 
 * This has changed for 5.2! See:
 *
 * http://www.lua.org/manual/5.2/manual.html#8.3
 * http://www.lua.org/manual/5.2/manual.html#luaL_setfuncs
 */
int LUA_API luaopen_myhello (lua_State *L) {
   struct luaL_reg driver[] = {
      {"helloworld", helloworld},		
      {NULL, NULL},
   };
   luaL_register(L, PROJECT_TABLENAME, driver);
   return 1;
}

/* Lua test
require("myhello")
myhello.helloworld() 
*/
