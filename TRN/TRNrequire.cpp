//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNrequire
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 17th, 2010 @ 12:55:34 PM
//---------------------------------------------------------------------------

#include <TRN/TRNrequire.h>
#include <TRN/TRNfile.h>
#include <TRN/TRNfileFind.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNcompile.h>
#include <LUACOM/LUACOMcreateDestroy.h>

#include <LUA/LUAutils.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static bool TRNisFileIncluded(lua_State* L, const COLstring& FileLabel){
   COL_FUNCTION(TRNisFileIncluded);
   lua_settop(L, 1);  // _LOADED table will be at index 2
   lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
   lua_getfield(L, 2, FileLabel.c_str()); // get _LOADED[name]
   bool IsIncluded = !!lua_toboolean(L, -1); // is it there?
   lua_settop(L, 1);
   COL_VAR2(FileLabel, IsIncluded);
   return IsIncluded;
}

static void TRNregisterLoadedFile(lua_State* L, const COLstring& FileLabel){
   COL_FUNCTION(TRNregisterLoadedFile);
   COL_VAR(FileLabel);
   lua_settop(L, 1);  // _LOADED table will be at index 2
   lua_getfield(L, LUA_REGISTRYINDEX, "_LOADED");
   lua_pushboolean(L, true);
   lua_setfield(L, 2, FileLabel.c_str());  // _LOADED[name] = true
}

static void TRNincludeFile(lua_State* L, const TRNfile* pFile){
   COL_FUNCTION(TRNincludeFile);
   TRNregisterLoadedFile(L, pFile->m_Label);
   lua_settop(L, 1);
   COLstring Error;
   if (!TRNcompileDouble(L, pFile->m_Source, pFile->m_Label, &Error)){
      luaL_error(L, TRN_REQUIRE_SENTINEL "%s", Error.c_str());
   }
}
// Turns a module name (as used in a Lua script) into a POSIX-style path.
static COLstring TRNluaFileName(const COLstring& Label) {
   COL_FUNCTION(TRNluaFileName);
   COL_VAR(Label);
   COLstring AdjustedLabel(Label);
   AdjustedLabel.replace(".", FIL_DIR_SEPARATOR);
   return AdjustedLabel + COL_T(".lua");
}

// This is our require...
static int TRNrequire(lua_State* L){
   COL_FUNCTION(TRNrequire);   
   TRNannotationState* pState = TRNgetAnnotationState(L);
   COL_VAR((void*)pState);
   COLstring ModuleName = LUAtoString(L, -1);
   COL_VAR(ModuleName);
   if ( ModuleName.substr(ModuleName.size()-4, 4) == ".lua" ) {
      lua_pushfstring(L, "Do not include '.lua' at the end of the filename");
      lua_error(L);
      return 1;
   }
   COL_VAR(ModuleName);
   ModuleName = TRNluaFileName(ModuleName);
   COLstring Label = ModuleName; // Label = foo/bar.lua
   COLstring FilePath = FILpathAppend(pState->m_ProjectPath, ModuleName); // FilePath = Translator/guid/foo/bar.lua
   // check local lua file
   if (!FILfileExists(FilePath)) {
      lua_pushfstring(L, "\n\tModule was not found in the translator workspace");
      return 1;
   }
   COL_VAR3(Label, ModuleName, FilePath);
   try {
      if (!TRNisFileIncluded(L, Label)) {
         TRNfile* pFile = TRNfileFindOrAdd(pState->m_LuaFileList, pState->m_ProjectPath, Label);
         TRNincludeFile(L, pFile);
      }
      // If no C++ exceptions or Lua errors were thrown, load was successful,
      // and the loaded code is on top of the stack, as a function.
   } catch (const COLerror& Error) {
      lua_pushfstring(L, "\n\t%s", Error.description().c_str());   // Loader functions return error strings, they do not throw them.
   }
   return 1;
}

#ifdef _WIN32
static int TRNrequireLuacom(lua_State* L){
   COL_FUNCTION(TRNrequireLuacom);
   TRNannotationState* pState = TRNgetAnnotationState(L);
   COL_VAR((void*)pState);
   COLstring ModuleName = LUAtoString(L, -1);
   COL_VAR(ModuleName);
   if (ModuleName == "luacom"){
      COL_TRC("Handle require for luacom");
      // The way the loader works is that we need a runnable function when TRNrequireLuacom returns, so here we push LUACOMloadLuacom
      // LUACOMloadLuacom will be run by the require logic, and will put to the top of the stack the table containing all of the 
      // luacom functions. 
      lua_pushcfunction(L, &LUACOMloadLuacom);
      TRNannotationStateSetLuacom(pState);
      return 1;
   } else {
      lua_pushfstring(L, "\n\tModule not handled by luacom loader");
      return 1;
   }
}

static void TRNaddLuacomLoader(lua_State* L){
   COL_FUNCTION(TRNaddLuacomLoader);
   lua_pushinteger(L, 5); // push 5
   lua_pushcfunction(L, &TRNrequireLuacom);
   lua_settable(L, -3); // package.loaders[5] = TRNrequireLuacom
}
#endif
// Overrides the 2nd loader in the standard packages.loaders array.
// This is the loader which looks for .lua files.
// Also adds a 5th loader specifically for luacom
// This handles require("luacom") and sets up luacom apporpriately.
void TRNrequireOverride(lua_State* L){
   COL_FUNCTION(TRNrequireOverride);
   lua_getfield(L, LUA_GLOBALSINDEX, "package"); // push package
   lua_getfield(L, -1, "loaders"); // push package.loaders
   lua_pushinteger(L, 2); // push 2
   lua_pushcfunction(L, &TRNrequire);
   lua_settable(L, -3); // package.loaders[2]
#ifdef _WIN32 // luacom only works on Windows as COM is a Windows protocol
   TRNaddLuacomLoader(L);
#endif
   lua_pop(L, 2); // pop package.loaders and package
}
