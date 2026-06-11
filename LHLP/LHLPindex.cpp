//---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LHLPindex
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
//---------------------------------------------------------------------------
#include <CACHE/CACHEfile.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <HELP/HELPenumerate.h>
#include <HELP/HELPresolve.h>
#include <HELP/HELPutils.h>
#include <LHLP/LHLPindex.h>
#include <LUA/LUAutils.h>
#include <LUAC/lua.h>
COL_LOG_MODULE;

void LHLPindex(COLhashmap<COLstring, COLstring>* pMap) {
   COL_FUNCTION(LHLPindex);
   if(CACHEexists()) {
      COLvar HelpList;
      CACHEgetHelpList(&HelpList);
      if(!HelpList.isArray()) { return; }
      const auto& List = HelpList.array();
      for(const auto& i : List) {
         COL_VAR(i.json(1));
         pMap->add(i["name"].asString(), i["location"].asString());
      }
   }
   else { HELPenumerateHelp(pMap, HELPglobalDir(), ""); }
}

// This function processes the function parts and determines whether we have a function on top of the stack.
static bool LHLPprocessFunction(COLarray<COLstring>& FunctionParts, lua_State* L){
   COL_FUNCTION(LHLPprocessFunction);
   int Size = FunctionParts.size();
   if (Size == 0) { return false; }
   COL_TRC("Getting " << FunctionParts[0] << " from _G");
   lua_getglobal(L, FunctionParts[0].c_str());  // the first part of a function name is in the globals table
   if (lua_type(L, -1) != LUA_TTABLE) {
      COL_TRC("Function is of type " << lua_type(L, -1) << newline);
      return Size == 1 && lua_type(L, -1) == LUA_TFUNCTION; }
   for (int i = 1; i < Size; i++){
      COL_TRC("Getting subfield " << FunctionParts[i]);
      lua_getfield(L, -1, FunctionParts[i].c_str());
      if (lua_type(L, -1) == LUA_TNIL) { return false; }
      if (i == Size - 1){ return lua_type(L, -1) == LUA_TFUNCTION; }
   }
   return false;
}

static bool LHLPprocessMethod(COLarray<COLstring>& FunctionParts, lua_State* L){
   COL_FUNCTION(LHLPprocessMethod);
   int Size = FunctionParts.size(); // limitation - size > 1.
   COLstring MetatableName, MethodName;
   if (Size == 2) {
      MetatableName = FunctionParts[0];
      MethodName = FunctionParts[1];
   } else {
      for (int i = 0; i < Size; i++){
         if (i == Size - 1) {
            MethodName = FunctionParts[i];
            break;
         }
         MetatableName.append(FunctionParts[i] + ".");
      }
   }
   MetatableName = MetatableName.strip(COLstring::Trailing, '.');
   COL_VAR2(MetatableName, MethodName);
   luaL_getmetatable(L, MetatableName.c_str());
   if(lua_type(L, -1) == LUA_TNIL){
      COL_TRC("Could not find metatable with this name");
      return false;
   }
   lua_getfield(L, -1, "__index");
   if(lua_type(L, -1) == LUA_TNIL){
      COL_TRC("Could not find __index table");
      return false;
   } 
   lua_getfield(L, -1, MethodName.c_str());
   if(lua_type(L, -1) == LUA_TNIL){
      COL_TRC("Could not find field with this name");
      return false;
   } else if (lua_type(L, -1) != LUA_TFUNCTION) {
      COL_TRC("Table entry was not a function");
      return false;
   }
   return true;
}

void LHLPluaHelpIndex(const COLhashmap<COLstring, COLstring>& Map, lua_State* L){
   COL_FUNCTION(LHLPluaHelpIndex);
   lua_getglobal(L, "_help");
   int HelpTableStackPos = lua_gettop(L); // store the absolute index of the _help table
   COL_VAR(HelpTableStackPos);
   COL_VAR(lua_typename(L, lua_type(L,HelpTableStackPos)));

   for(const auto& it : Map) {
      COLarray<COLstring> Parts;
      COL_VAR2(it->first, it->second);
      bool IsMethod;
      HELPresolveLua(it->first, &Parts, &IsMethod);
      if(IsMethod && LHLPprocessMethod(Parts, L)) {
         COL_TRC("Adding method entry: help for " + it->first + " at " + it->second);
         LUApushString(L, it->second);
         lua_settable(L, HelpTableStackPos);
      } else {
         if(LHLPprocessFunction(Parts, L)) {
            COL_TRC("Adding function entry: help for " + it->first + " at " + it->second);
            LUApushString(L, it->second);
            COL_VAR2(lua_typename(L, lua_type(L, -1)), lua_typename(L, lua_type(L, -2)));
            COL_VAR(HelpTableStackPos);
            COL_VAR(lua_typename(L, lua_type(L, HelpTableStackPos)));
            lua_settable(L, HelpTableStackPos);
         }
      }
      lua_settop(L, HelpTableStackPos);  // this clears up any leftover tables on the stack not consumed by lua_settable.
   }
   lua_pop(L, 1);  // pop _help
}

static void LHLPregisterCustomFunction(lua_State* L, const COLstring& FunctionName, const COLstring& FilePath) {
   COL_FUNCTION(LHLPregisterCustomFunction);
   COL_TRC("Registering " << FunctionName << ": " << FilePath);
   lua_getglobal(L, "_help"); // -1. _help
   lua_getglobal(L, FunctionName.c_str());
   lua_pushstring(L, FilePath.c_str()); // -1. FilePath, -2 FunctionName, -3. _help
   if(lua_type(L, -1) == LUA_TSTRING && lua_type(L, -2) == LUA_TFUNCTION) {
      COL_VAR2(lua_tostring(L, -1), lua_tostring(L, -2));
      lua_settable(L, -3); // -1. FilPath, -2. FunctionName, -3. _help
   } else {
      lua_pop(L, 2);
   }
   lua_pop(L, 1);
}

void LHLPregisterCustomHelp(lua_State* L, COLhashmap<COLstring, COLstring>* pHelpMap, const COLstring& ProjectPath) {
   COL_FUNCTION(LHLPregisterCustomHelp);
   if (L == NULL){
      COL_TRC("No lua state so return");
      return;
   }
   COLhashmap<COLstring, COLstring> CustomHelpMap;
   HELPenumerateHelp(&CustomHelpMap, ProjectPath, "");
   for(const auto& it : CustomHelpMap) {
      COLarray<COLstring> FunctionParts;
      COL_VAR2(it->first, it->second);
      bool IsMethod;
      HELPresolveLua(it->first, &FunctionParts, &IsMethod);
      COLstring Function = FunctionParts[FunctionParts.size() - 1];
      COL_TRC("Registering " << Function << ": " << it->second << newline);
      LHLPregisterCustomFunction(L, Function, it->second);
      pHelpMap->add(Function, it->second);
   }
}

void LHLPinit(COLhashmap<COLstring, COLstring>* pMap, lua_State* L){
   COL_FUNCTION(LHLPinit);
   if (L == NULL){
      COL_TRC("No lua state so return");
      return;
   }
   if (lua_gettop(L) == -1){
      COL_TRC("Compile failed so we must adieu");
      return;
   }
   LHLPindex(pMap);
   LHLPluaHelpIndex(*pMap, L);
}

void LHLPcreateHelpTable(lua_State* L){
   COL_FUNCTION(LHLPcreateHelpTable);
   lua_newtable(L); // _help
   lua_setglobal(L, "_help");
}
