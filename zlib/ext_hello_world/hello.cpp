// This is an example "hello world" DLL extension that will work with Iguana.

#if defined(_WIN32) || defined(_WIN32)
#  define LUA_BUILD_AS_DLL   /* for #define LUA_API __declspec(dllimport) */
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "lua.h"
#include "lauxlib.h"

#ifdef __cplusplus
}
#endif

#if defined(_WIN32) || defined(_WIN32)
#  ifdef __cplusplus
#    define MY_LUA_API   extern "C" __declspec(dllexport)
#  else
#    define MY_LUA_API   __declspec(dllexport)
#  endif
#else
#  define MY_LUA_API     extern "C"
#endif

// You should read the Lua manual about Lua's C API to understand this
// code but this should get you started.

// All Lua functions take one argument - lua_State* L
static int world(lua_State* L){
   // Check the arguments.
   int CountOfArg = lua_gettop(L);
   if (CountOfArg != 1 || !lua_isstring(L, 1)){
      lua_pushstring(L, "Give me one string argument please!");
	  lua_error(L);
   }
   lua_pushstring(L, "Hello world!");
   return 2;   // We'll return 2 results, the argument passed in and our "Hello World!"
}

static int compute_average_and_sum(lua_State* L){
   lua_Number sum = 0;
   int n = lua_gettop(L);            /* number of arguments */
   for (int i = 1; i <= n; i++) {
      if (!lua_isnumber(L, i)) {
         lua_pushstring(L, "incorrect argument");
         lua_error(L);
      }
      sum += lua_tonumber(L, i);
   }
   lua_pushnumber(L, (n?(sum/n):0)); /* first result */
   lua_pushnumber(L, sum);           /* second result */
   return 2;                         /* number of results */
}


MY_LUA_API int luaopen_hello(lua_State* L){
   // We are defining two functions here.
   struct luaL_reg driver[] = {
	  {"world" , world},
	  {"compute_average_and_sum", compute_average_and_sum},
      {NULL, NULL}
   };
   luaL_register(L, "hello", driver);
   return 1;
}

