# windows only build: luacom.dll
LIBRARY = luacom

IFW_MAKE_SHARED_LIBRARY = 1

CPPEXTRA += \
$(call MAKEDEFINEFLAG, "_CRT_SECURE_NO_DEPRECATE" "COMPAT51") \
$(call MAKEINCLUDEFLAG, src/dll src/library include) \
$(IFW_LUA_INCLUDE)

SRC=\
src/dll/luacom_dll.cpp\
src/library/LuaAux.cpp\
src/library/luabeans.cpp\
src/library/luacom.cpp\
src/library/LuaCompat.cpp\
src/library/tCOMUtil.cpp\
src/library/tLuaCOM.cpp\
src/library/tLuaCOMClassFactory.cpp\
src/library/tLuaCOMConnPoints.cpp\
src/library/tLuaCOMEnumerator.cpp\
src/library/tLuaCOMException.cpp\
src/library/tLuaCOMTypeHandler.cpp\
src/library/tLuaControl.cpp\
src/library/tLuaDispatch.cpp\
src/library/tLuaObject.cpp\
src/library/tLuaObjList.cpp\
src/library/tLuaTLB.cpp\
src/library/tLuaVector.cpp\
src/library/tUtil.cpp\

DEF_FILE = src/dll/luacom_dll.def
WIN_AREXTRA += \
$(call QUOTEFLAG, /DEF:, $(DEF_FILE)) \
shlwapi.lib htmlhelp.lib

include ../makefiles/library.makefile

ifeq ($(IFW_MAKE_32_BIT), 1)
   # 32-bit
   EXPORT_DEF = ../Iguana/iguana_lua_export32.def
   EXPORT_LIB = iguana_lua_export32.lib
else
   # 64-bit
   EXPORT_DEF = ../Iguana/iguana_lua_export64.def
   EXPORT_LIB = iguana_lua_export64.lib
endif

ifeq ($(IFW_WIN), 1)
$(LIBNAME): $(EXPORT_LIB)
$(EXPORT_LIB): $(EXPORT_DEF)
	$(call MAKEIMPORTLIB, $<, $@)

clean::
	rm -f $(EXPORT_LIB)

endif
