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

#include <TNDL/TNDLglobals.h>

#include <PIP/PIPcommandLineUtils.h>
#include <PIP/PIPptrArray.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

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

class COLdateTime;

void (*pDBQtraceLogRollover)(const COLdateTime&, COLint64) = NULL;

void CHAMtraceInit(const COLstring& Env){
   // PROgetenv() uses a static lock which won't be initialized yet, so use getenv
   char* pEnv = getenv(Env.c_str());
   if (!pEnv) {
      return;
   }
   COLstring LogCommandLine = pEnv;
   if (!LogCommandLine.is_null()) {
      COLarray<COLstring> Arguments;
      PIPparseCommandLine("dummy " + LogCommandLine, &Arguments);

      PIPptrArray<char> Argv;
      PIPbuildPosixArgvArray(Arguments, &Argv);

      COL_LOG_INIT(Argv.size(), (const char**)Argv.array());
      COL_FUNCTION(CHAMtraceInit);
      COL_VAR(LogCommandLine);
   }
}

MY_LUA_API int luaopen_chameleon(lua_State* L){
   CHAMtraceInit("CHM_TRACE");
   TNDLregisterCoreGlobals(L);
/*   lua_newtable(L);
   lua_pushstring(L, "transform");
   lua_pushcfunction(L, &TNDLchmTransform);
   lua_settable(L, -3);
   lua_pushstring(L, "parse");
   lua_pushcfunction(L, &TNDLparseHl7);
   lua_settable(L, -3);
   lua_pushstring(L, "message");
   lua_pushcfunction(L, &TNDLinitHl7Message);
   lua_settable(L, -3);
   lua_pushstring(L, "legacy_parse");
   lua_pushcfunction(L, &TNDLfullParse);
   lua_settable(L, -3);*/

   // We are defining two functions here.
   /*struct luaL_reg driver[] = {
	  {"world" , world},
      {NULL, NULL}
   };
   luaL_register(L, "hl7", driver);*/

   return 1;
}

