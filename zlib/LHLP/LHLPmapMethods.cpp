//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: HELPmapMethods.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   30/11/23 12:32 PM
//  ---------------------------------------------------------------------------
#include <LHLP/LHLPmapMethods.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAopen.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Returns false if the inner table at the top of the Lua stack is of the form 
// { name : <lua function>, filepath : <string> }
static bool HELPverifyInnerTable(lua_State* L){
   COL_FUNCTION(HELPverifyInnerTable);
   lua_getfield(L, -1, "name");
   if (lua_type(L, -1) != LUA_TFUNCTION) { 
      lua_pop(L, 1);
      return true; 
   }
   lua_pop(L, 1);
   lua_getfield(L, -1, "filepath");
   if (lua_type(L, -1) != LUA_TSTRING) { 
      lua_pop(L, 1);
      return true; 
   }
   lua_pop(L, 1);
   return false;
}

// Check that the methods table has at least one string key mapped to a function value
static bool HELPverifyMethodsTable(lua_State* L){
   COL_FUNCTION(HELPverifyMethodsTable);
   bool TableError = true;
   lua_pushnil(L); // -1. key(nil), -2. table
   while (lua_next(L, -2)){ // lua_next pops the key and pushes a key, value pair while the table has entries, and nothing on completion.
      COL_VAR2(lua_typename(L, lua_type(L, -2)), lua_typename(L, lua_type(L, -1)));
      if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TFUNCTION){
         // In this case we expect the file name of the help file to match the function name
         TableError = false;
      } else if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TTABLE){
         // This case is for mapping a function to a custom path
         TableError = HELPverifyInnerTable(L);
      }
      lua_pop(L, 1);
   }
   return TableError;
}

static COLstring HELPmakeFilePath(lua_State* L, const COLstring& RootDirectory, const COLstring& Filename){
   COLstring HelpFileLocation = LUAprojectRoot(L); 
   HelpFileLocation = FILpathAppend(HelpFileLocation, RootDirectory);
   HelpFileLocation = FILpathAppend(HelpFileLocation, Filename);
   COL_VAR(HelpFileLocation);
   return HelpFileLocation;
}

static void HELPmapSimpleFilepath(lua_State* L, const COLstring& Dir){
   COL_FUNCTION(HELPmapSimpleFilepath);
   COLstring HelpFileLocation = HELPmakeFilePath(L, Dir, LUAtoString(L, -2) + ".help");
   COL_TRC("Mapping " + HelpFileLocation);
   lua_pushstring(L, HelpFileLocation.c_str());  // -1. HelpFileLocation, -2. value, -3. key, -4. _help, -5. methods
   lua_settable(L, -4);  // _help[value (Lua function)] = HelpFileLocation
}

static bool HELPmapCustomFilepath(lua_State* L, const COLstring& Dir){
   COL_FUNCTION(HELPmapCustomFilepath);
   lua_getfield(L, -1, "filepath");          // -1.filepath, -2. value(table), -3. key, -4. _help, -5. methods table
   if (lua_isnil(L, -1)){
      lua_pop(L, 2);
      return false;
   }
   COLstring HelpFileLocation = HELPmakeFilePath(L, Dir, LUAtoString(L, -1));
   lua_pop(L, 1);
   lua_getfield(L, -1, "name");              // -1.name, -2. value(table), -3. key, -4. _help, -5. methods table
   if (lua_isnil(L, -1)){
      lua_pop(L, 2);
      return false;
   }
   COL_TRC("Mapping " + HelpFileLocation);
   lua_pushstring(L, HelpFileLocation.c_str()); // -1.filepath, -2. name, -3. value(table), -4. key, -5. _help, -6. methods table
   lua_settable(L, -5);   // _help[value (Lua function)] = HelpFileLocation
   lua_pop(L, 1);
   return true;
}

static void HELPregisterMethods(lua_State* L, const COLstring& Dir){
   COL_FUNCTION(HELPregisterMethods);
   lua_getglobal(L, "_help");               // -1. _help, -2. methods table
   lua_pushnil(L);                          // -1. nil, -2. _help, -3. methods table
   while (lua_next(L, -3)){                 // -1. value, -2. key, -3. _help, -4. methods table
      if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TFUNCTION){
         COL_TRC("found basic function");
         HELPmapSimpleFilepath(L, Dir);
      } else if (lua_type(L, -2) == LUA_TSTRING && lua_type(L, -1) == LUA_TTABLE){
         COL_TRC("found table");            // -1. value(table), -2. key, -3. _help, -4. methods table
         if (!HELPmapCustomFilepath(L, Dir)) { continue; }
      } else {
         lua_pop(L, 1);
      }
   }
}

// stack is 1 : dir where help files are, 2: lua table of methods
int LHLPmapMethods(lua_State* L){
   COL_FUNCTION(LHLPmapMethods);
   try {
      COLstring Dir = LUAexpectedStringParam(L, 1, "dir");
      COL_VAR(Dir);
      COLstring MetatableName(Dir);
      MetatableName.replace(FIL_DIR_SEPARATOR, ".");
      LUAexpectedTableParam(L, 1, "methods");
      if(HELPverifyMethodsTable(L)) { LUA_ERROR_STREAM("The methods table must have at least one string mapped to a function."); }
      COL_TRC("Adding method entries to _help");
      HELPregisterMethods(L, Dir);
      lua_settop(L, 0); // clean up stack prior to handing back to lua
   } LUA_CATCH();
   return 0;
}

void LHLPaddHelpNamespace(lua_State* L){
   COL_FUNCTION(LHLPaddHelpNamespace);
   lua_newtable(L);  // "help" Namespace
   LUAaddMethod(L, "map", &LHLPmapMethods);
   lua_setglobal(L, "help");
}