//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LJSONapi
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, April 12th, 2011 @ 02:43:06 PM
//
//---------------------------------------------------------------------------
#include "LJSONapi.h"

#include <LUA/LUAerror.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAutils.h>

#include <LUAL/LUALutils.h>
#include <LUA/LUAcheck.h>

#include <COL/COLvar.h>
#include <COL/COLsink.h>
#include <COL/COLref.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const char* LJSON_NAMESPACE = "json";
static const char* JSON_NULL = "NULL";
static const char* JSON_NULL_DISPLAY = "json.NULL"; // See also: LUAutils.cpp
static const char* JSON_NULL_REGISTRY_KEY = JSON_NULL_DISPLAY;
static const char* JSON_NULL_METATABLE = "json_null";
static const char* EMPTY_JSON_OBJECT_METATABLE = "json_empty_object";

#define LJSON_THROW_BAD_TABLE_ERROR(_Depth, _pKey, _Message)\
   if (_pKey){\
   COL_ERROR_STREAM_PLAIN("Error at depth " << _Depth << ", item with key " << *_pKey << ": " << _Message, COLerror::PreCondition);\
   } else {\
   COL_ERROR_STREAM_PLAIN("Error at depth " << _Depth << ": " << _Message, COLerror::PreCondition);\
   }

// enum LJSONjsonCollectionType{ // not used???
//    LJSON_JSON_UNKNOWN,
//    LJSON_JSON_OBJECT,
//    LJSON_JSON_ARRAY
// };


static int LJSONjsonNullToString(lua_State* L){
   lua_pushstring(L, JSON_NULL);
   return 1;
}

static int LJSONjsonNullDisplay(lua_State* L){
   lua_pushstring(L, JSON_NULL_DISPLAY);
   lua_pushstring(L, JSON_NULL_DISPLAY);
   LUApushString(L, "null");
   return 3;
}

// Assign to the __newindex metamethod.  Will give the table an
// empty metatable (so this function won't be called again) and
// does the default table assignment.
static int LJSONremoveMetatable(lua_State* L){
   // Table at index 1, key at index 2, value at index 3
   lua_pushnil(L); // no metatable
   lua_setmetatable(L, 1);
   lua_settable(L, 1); // the default table assignment.
   return 0;
}

void LJSONaddJsonConstants(lua_State* L){
   // Table T on stack
   lua_pushstring(L, JSON_NULL);
   lua_newuserdata(L, 0);
   if (luaL_newmetatable(L, JSON_NULL_METATABLE)){
      // If we're in here, the new meta table is on the stack.
      lua_pushcfunction(L, &LJSONjsonNullToString);
      lua_setfield(L, -2, "__tostring"); // metatable.__tostring = LJSONjsonNullToString
      lua_pushcfunction(L, &LJSONjsonNullDisplay);
      lua_setfield(L, -2, "__display"); // metatable.__display = LJSONjsonNullDisplay
   }
   lua_setmetatable(L, -2); // set newuserdata's metatable, and pop the metatable
   // json.NULL userdata is on top of the stack
   lua_pushvalue(L, -1); // push another reference to json.NULL
   lua_setfield(L, LUA_REGISTRYINDEX, JSON_NULL_REGISTRY_KEY);
   lua_settable(L, -3); // T[JSON_NULL] = json.NULL
}

bool LJSONisEmptyJsonObject(lua_State* L, int ItemIndex){
   const int OriginalStackTop = lua_gettop(L);
   lua_getmetatable(L, ItemIndex);
   lua_getfield(L, LUA_REGISTRYINDEX, EMPTY_JSON_OBJECT_METATABLE);
   bool Result = !!lua_rawequal(L, -1, -2);
   lua_settop(L, OriginalStackTop);
   return Result;
}

static const char *const JSON_PARSE_OPTIONS[] = { "data", NULL };

int LJSONparseString(lua_State* L) {
   COL_FUNCTION(LJSONparseString);
   try {
      LUAcheckParamsEx(L, 1, JSON_PARSE_OPTIONS);
      LUAexpectedStringParamPushed(L,1,"data");
      size_t StrLen = 0;
      const char* pData = lua_tolstring(L,-1,&StrLen);
      COLvar TheVar;
      if (! TheVar.parse(pData, strlen(pData))) {
         COL_ERROR_STREAM_PLAIN(TheVar.asString(), COLerror::PreCondition);
      }
      TheVar.toLuaNullsNotNils(L);

      return 1; // number of results
   }
   LUA_CATCH();
}

static void LJSONvalidateArrayIndices(lua_State* L, int TableIndex, int ItemCount, const COLstring* pKey, int Depth){
   const int OriginalStackTop = lua_gettop(L);
   for (int i = 1; i <= ItemCount; i++){
      lua_pushinteger(L, i);
      lua_gettable(L, TableIndex);
      if (lua_isnil(L, -1)){
         LJSON_THROW_BAD_TABLE_ERROR(Depth, pKey, "Array index " << i << " is missing.\nArray indices must be contiguous integers starting from 1.");
      }
      lua_pop(L, 1);
   }
   COLASSERT(lua_gettop(L) == OriginalStackTop);
}

static inline bool LJSONisInteger(lua_Number Num){ return (lua_Number)(lua_Integer)Num == Num; }

static void LJSONgenerateJsonWithVar(lua_State* L, bool Compact, COLstring* pJsonString){
   COL_FUNCTION(LJSONgenerateJsonWithVar);
   COLvar Tree;
   Tree.fromLua(L, 1);
   *pJsonString = Tree["data"].json(!Compact, COLvar::StrEncJs);
}

static const char *const JSON_SERIALIZE_OPTIONS[] = { "data", "compact", "alphasort", NULL };

int LJSONserialize(lua_State* L){
   COL_FUNCTION(LJSONserialize);
   try{
      LUAcheckParams(L, 1, JSON_SERIALIZE_OPTIONS);
      bool Compact = LUAoptionalBoolParam(L, 1, "compact", false);
      bool AlphaSort = LUAoptionalBoolParam(L, 1, "alphasort", false);
      COL_VAR2(Compact, AlphaSort);
      LUAexpectedTableParam(L, 1, "data");
      COLstring JsonString;
      // TODO - remove the alphasort argument
      LJSONgenerateJsonWithVar(L, Compact, &JsonString);
      COL_VAR(JsonString);
      LUApushString(L, JsonString);
      return 1;
   }
   LUA_CATCH();
}

int LJSONnewObject(lua_State* L){
   COL_FUNCTION(LJSONnewObject);
   if (lua_gettop(L) != 0){
      LUA_ERROR_STREAM("No arguments expected.");
   }
   lua_newtable(L); // push empty object onto stack
   if (luaL_newmetatable(L, EMPTY_JSON_OBJECT_METATABLE)){
      // If we're in here, the new meta table is on the stack.
      lua_pushcfunction(L, &LJSONremoveMetatable);
      lua_setfield(L, -2, "__newindex"); // metatable.__newindex = LJSONremoveMetatable
   }
   lua_setmetatable(L, 1);
   return 1; // number of results
}

void LJSONapiAddJsonNamespace(lua_State* L){
   COL_FUNCTION(LJSONapiAddJsonNamespace);
   lua_newtable (L);
   LUAaddMethod(L, "parse",        &LJSONparseString);
   LUAaddMethod(L, "serialize",    &LJSONserialize);
   LUAaddMethod(L, "createObject", &LJSONnewObject);
   LJSONaddJsonConstants(L);
   lua_setglobal(L, LJSON_NAMESPACE);
}
