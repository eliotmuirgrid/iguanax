
#include <windows.h>
  #include "lua.h"
  #include "lualib.h"
  #include "lauxlib.h"
  #include "LuaCompat.h"
#ifdef IUP
  #include "iup.h"
  #ifndef NO_CPICONTROLS
  #include "iupcontrols.h"
  #include "iupgl.h"
  #include "iupluaim.h"
  #endif
  #ifdef SPEECH_SDK
  #include "iupspeech.h"
  #endif
  #include "iuplua.h"
  #include "cdlua5.h"
  #ifndef NO_CPICONTROLS
  #include "luacontrols.h"
  #include "luagl.h"
  #endif
  #ifdef SPEECH_SDK
  #include "luaspeech.h"
  #endif
  #ifdef USE_GLLUA
  #include "gllua.h"
  #endif
#endif

#include "luacom.h"
#include "luacom_internal.h"
#include "tLuaCOMClassFactory.h"
#include "tLuaCOMException.h"
#include "tUtil.h"
#include "tCOMUtil.h"

LUACOM_API int luacom_openlib(lua_State* L) {
	luacom_open(L);
	return 0;
}
#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
LUACOM_API int luaopen_luacom(lua_State* L) {
	return luacom_openlib(L);
}
#endif


#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
#define lua_dofile luaL_dofile
#endif

static volatile DWORD dwTlsIndex = 0;
static volatile DWORD dwTlsLogFileIndex = 0;

static lua_State* getFactoryCache(){
   void* lpvData = TlsGetValue(dwTlsIndex); //TODO - handle error
   return (lua_State*)lpvData;
}

static void clearFactoryCache(){
   if (dwTlsIndex){
      TlsSetValue(dwTlsIndex,0);
   }
}

static void factoryCache_Init() {
#ifdef IUP
	/* iup initialization */
	IupOpen();
	#ifndef NO_CPICONTROLS
	IupGLCanvasOpen();
	IupControlsOpen();
	#endif
	#ifdef SPEECH_SDK
	IupSpeechOpen();
	#endif
#endif
   void* lpvData = TlsGetValue(dwTlsIndex); //TODO - handle error
   if (!lpvData){
#ifdef LUA5
      lpvData = lua_open();
#else
      lpvData = lua_open(0);
#endif
      TlsSetValue(dwTlsIndex,lpvData);
   }
   lua_State* factoryCache = (lua_State*)lpvData;
   lua_newtable(factoryCache);
	lua_setglobal(factoryCache,"factories");
}

static tLuaCOMClassFactory* factoryCache_GetFactory(const char* luaclsid) {
   lua_State* factoryCache = getFactoryCache();
   lua_getglobal(factoryCache,"factories");
	lua_pushstring(factoryCache,luaclsid);
	lua_gettable(factoryCache,-2);
	tLuaCOMClassFactory* pFactory = (tLuaCOMClassFactory*)luaCompat_getPointer(factoryCache,-1);
	lua_pop(factoryCache,2);
	return pFactory;
}

static void factoryCache_PutFactory(const char* luaclsid, tLuaCOMClassFactory* pFactory) {
	pFactory->AddRef();
   lua_State* factoryCache = getFactoryCache();
	lua_getglobal(factoryCache,"factories");
	lua_pushstring(factoryCache,luaclsid);
	luaCompat_pushPointer(factoryCache,pFactory);
	lua_settable(factoryCache,-3);
	lua_pop(factoryCache,1);
}

static void factoryCache_Release() {
   lua_State* factoryCache = getFactoryCache();
   if (factoryCache){
      lua_getglobal(factoryCache,"factories");
      lua_pushnil(factoryCache); 
      while(lua_next(factoryCache,-2) != 0) {
         tLuaCOMClassFactory* pFactory = (tLuaCOMClassFactory*)luaCompat_getPointer(factoryCache,-1);
         pFactory->Release();
         lua_pop(factoryCache,1);
      }
      lua_pop(factoryCache,2);
#ifdef IUP
      IupClose();
#endif
      clearFactoryCache();
   }
}

static lua_State* luacom_DoRegistryFile(const char* luaclsid) {
	lua_State* L_inproc = NULL;
	char* fileName;
	char* key = new char[strlen(luaclsid)+18];

	strcpy(key, "CLSID\\");
	strcat(key,luaclsid);
	strcat(key,"\\ScriptFile");

	if(tCOMUtil::GetRegKeyValue(key,&fileName)) {
		L_inproc = lua_open();
		#if defined(LUA_VERSION_NUM) && LUA_VERSION_NUM >= 501
 		luaL_openlibs(L_inproc);
 		#else
		luaopen_base(L_inproc);
		luaopen_math(L_inproc);
		luaopen_io(L_inproc);
		luaopen_string(L_inproc);
		luaopen_table(L_inproc);
		luaopen_loadlib(L_inproc); // luaopen_package(L_inproc);
 		#endif
		#ifdef IUP
			/* iuplua initialization */
			iuplua_open(L_inproc);
			iupkey_open(L_inproc);
			cdlua5_open(L_inproc);
			#ifndef NO_CPICONTROLS
			controlslua_open(L_inproc);
			gllua_open(L_inproc);
			#endif
			#ifdef SPEECH_SDK
			speechlua_open(L_inproc);
			#endif
			#ifdef USE_GLLUA
			gl_open(L_inproc);
			#endif
			iupluaim_open(L_inproc);
		#endif

		luacom_open(L_inproc);
		lua_getregistry(L_inproc);
		lua_pushstring(L_inproc,"inproc");
		luaCompat_pushBool(L_inproc,TRUE);
		lua_settable(L_inproc,-3);
		lua_pop(L_inproc,1);

		if(!lua_dofile(L_inproc,fileName)) {
			lua_pushstring(L_inproc,"StartAutomation");
			lua_gettable(L_inproc,-2);
			if(luaCompat_call(L_inproc, 0, 0, NULL)) {
				luacom_close(L_inproc);
				lua_close(L_inproc);
				L_inproc = NULL;
			}
		} else {
			luacom_close(L_inproc);
			lua_close(L_inproc);
			L_inproc = NULL;
		}
	}

	delete[] key;
	SAFEDELETEARR(fileName);
	return L_inproc;
}

static tLuaCOMClassFactory* luacom_GetInprocFactory(REFCLSID rclsid) {
	BSTR clsid;
	try {
		CHK_COM_CODE(StringFromCLSID(rclsid, &clsid));
      std::string& luaclsidstr = tUtil::bstr2string(clsid); 
		const char* luaclsid = luaclsidstr.c_str();
		CoTaskMemFree(clsid);
		tLuaCOMClassFactory* pFactory = factoryCache_GetFactory(luaclsid);
		if(pFactory == NULL) {
			lua_State* L_inproc = luacom_DoRegistryFile(luaclsid);
			if(L_inproc != NULL) {
				pFactory = new tLuaCOMClassFactory(L_inproc);
				factoryCache_PutFactory(luaclsid,pFactory);
			}
		}
		return pFactory;
 	} catch(class tLuaCOMException&) {
      return NULL;
	}
}

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	tLuaCOMClassFactory* pFactory = luacom_GetInprocFactory(rclsid);
	if(pFactory == NULL) return CLASS_E_CLASSNOTAVAILABLE;
	
	HRESULT hr = pFactory->QueryInterface(riid,ppv);

	if(FAILED(hr)) return hr;

	return S_OK;
}

BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	g_hInstance = (HINSTANCE)hModule;
   switch(ul_reason_for_call)
   {
   case DLL_PROCESS_ATTACH:
      {
         if (!dwTlsIndex){ 
            if ((dwTlsIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
               return FALSE; 
            }
         }
         if (!dwTlsLogFileIndex){ 
            if ((dwTlsLogFileIndex = TlsAlloc()) == TLS_OUT_OF_INDEXES) {
               return FALSE; 
            }
         }
         factoryCache_Init();
      }
      break;
   case DLL_PROCESS_DETACH:
      {
         factoryCache_Release();
         if (dwTlsIndex){
            TlsFree(dwTlsIndex);
            dwTlsIndex = 0;
         }
         if (dwTlsLogFileIndex){
            TlsFree(dwTlsLogFileIndex);
            dwTlsLogFileIndex = 0;
         }
      }
      break;
   case DLL_THREAD_ATTACH:
      factoryCache_Init();
      break;
   case DLL_THREAD_DETACH:
      factoryCache_Release();
      tUtil::CloseLogFile();
      break;
   default:
      break;
   }
   return TRUE;
}

FILE* tLuaGetLogFile(){
   return (FILE*)TlsGetValue(dwTlsLogFileIndex); //TODO - handle error
}

void tLuaSetLogFile(FILE* pNew){
   TlsSetValue(dwTlsLogFileIndex,pNew);
}
