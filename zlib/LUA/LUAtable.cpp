//---------------------------------------------------------------------------
// Copyright (C) 1997-2020 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALtable
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wed 31 Aug 2022 14:35:10 EDT
//---------------------------------------------------------------------------

#include <LUA/LUAutils.h>
#include <LUA/LUAtable.h>
#include <LUA/LUAerror.h>

#include <LUAL/LUALutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


void LUALtableExpectedError(lua_State* L, int i){
   LUA_ERROR_STREAM("Parameter table expected, got "
      << lua_typename(L, lua_type(L,i)) << '.');
}

// Checks if param is table, if not throw lua error
void LUAparamIsTable(lua_State* L, int i){
   COL_FUNCTION(LUAparamIsTable);
   if (!lua_istable(L, i)){
      COL_TRC("parameter " << i << " is not a table.");
      LUALtableExpectedError(L, i);
   }
}
// checks if parameter is present and pushes value to stack
bool LUApushParamByName(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUApushParamByName);
   LUApushString(L, Param);
   lua_gettable(L, i);
   if (lua_type(L,-1) == LUA_TNIL){
      COL_TRC("Got nil type.");
      return false;
   }
   return true;
}
// checks if value is expected type
void LUALparamIsType(lua_State* L, const COLstring& Param, int ExpectedType, const char* pExpectedTypeName){
   COL_FUNCTION(LUALparamIsType);
   COL_VAR2(ExpectedType, lua_type(L,-1));
   if (lua_type(L, -1) != ExpectedType){
      LUA_ERROR_STREAM("Parameter '" << Param << "' should be of type " << pExpectedTypeName << '.');
   }
}
// checks if value is string type
void LUALparamIsStringType(lua_State* L, const COLstring& Param){
   COL_FUNCTION(LUALparamIsStringType);
   if (!lua_isstring(L, -1)) {
      LUA_ERROR_STREAM("Parameter '" << Param << "' is not string and cannot be converted to string.");
   }
}
// checks if value is number type
void LUALparamIsNumberType(lua_State* L, const COLstring& Param){
   COL_FUNCTION(LUALparamIsNumberType);
   if (!lua_isnumber(L, -1)) {
      LUA_ERROR_STREAM("Parameter '" << Param << "' is not number and cannot be converted to number.");
   }
}

COLstring LUAexpectedStringParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(LUAexpectedStringParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)){ // required param does not exist, throw error
      lua_pop(L,1);
      LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
   }
   LUALparamIsStringType(L, Param); // make sure it is string type
   const COLstring& Value = LUAtoString(L, -1);
   lua_pop(L,1);
   return Value;
}

int LUAexpectedIntParam (lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAexpectedIntParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)){  // required param does not exist, throw error
      lua_pop(L,1);
      LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
   }
   LUALparamIsNumberType(L, Param); // make sure it is number type
   int Result = (int)lua_tointeger(L, -1);
   lua_pop(L,1);
   return Result;
}

void LUAexpectedTableParam(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAexpectedTableParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)){  // required param does not exist, throw error
      lua_pop(L,1);
      LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
   }
   LUALparamIsType(L, Param, LUA_TTABLE, "table"); // make sure it is table type
}

void LUAexpectedFuncParam(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAexpectedFuncParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)){ // required param does not exist, throw error
      lua_pop(L,1);
      LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
   }
   LUALparamIsType(L, Param, LUA_TFUNCTION, "function"); // make sure it is function type
}

bool LUAexpectedBoolParam(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAexpectedBoolParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)){
      lua_pop(L,1);
      LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
   }
   if (!lua_isboolean(L, -1)){
      lua_pop(L,1);
      LUA_ERROR_STREAM("Parameter '" << Param << "' should be a boolean.");
   }
   bool Result = lua_toboolean(L, -1);
   lua_pop(L,1);
   return Result;
}

COLstring LUAoptionalStringParam (lua_State* L, int i, const COLstring& Param, const COLstring& DefaultValue) {
   COL_FUNCTION(LUAoptionalStringParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC("returning default value for " + Param);
      return DefaultValue;
   }
   LUALparamIsStringType(L, Param);
   const COLstring& Result = LUAtoString(L, -1);
   lua_pop (L, 1);
   return Result;
}

bool LUAoptionalStringParamPushed(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAoptionalStringParamPushed);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      COL_TRC("Parameter '" + Param + "' does not exist");
      lua_pop(L,1);  // pop the nil on top of the stack
      return false;
   }
   LUALparamIsStringType(L, Param);
   // the string parameter is on top of the stack
   return true;
}

bool LUAoptionalBoolParam (lua_State* L, int i, const COLstring& Param, bool DefaultValue){
   COL_FUNCTION(LUAoptionalBoolParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC("returning default value for " + Param);
      return DefaultValue;
   }
   LUALparamIsType(L, Param, LUA_TBOOLEAN, "boolean");
   bool Result = lua_toboolean(L, -1) != 0;
   lua_pop (L, 1);
   return Result;
}

int LUAoptionalIntParam(lua_State* L, int i, const COLstring& Param, int DefaultValue) {
   COL_FUNCTION(LUAoptionalIntParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC("returning default value for " + Param);
      return DefaultValue;
   }
   LUALparamIsNumberType(L, Param);
   int Result = lua_tointeger(L, -1);
   lua_pop (L, 1);
   return Result;
}

double LUAoptionalDoubleParam(lua_State* L, int i, const COLstring& Param, double DefaultValue){
   COL_FUNCTION(LUAoptionalDoubleParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC("returning default value for " + Param);
      return DefaultValue;
   }
   LUALparamIsNumberType(L, Param);
   double Result = lua_tonumber(L, -1);
   lua_pop (L, 1);
   return Result;
}

bool LUAoptionalTableParam(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAoptionalTableParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC(Param << " is nil");
      return false;
   }
   LUALparamIsType(L, Param, LUA_TTABLE, "table");
   return true;
}

bool LUAoptionalFuncParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(LUAoptionalFuncParam);
   LUAparamIsTable(L, i);
   if (!LUApushParamByName(L, i, Param)) { // param does not exist
      lua_pop(L,1);
      COL_TRC(Param << " is nil");
      return false;
   }
   LUALparamIsType(L, Param, LUA_TFUNCTION, "function");
   return true;
}

void LUAexpectedStringParamPushed(lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(LUAexpectedStringParamPushed);
   LUAparamIsTable(L, i);
   LUApushParamByName(L, i, Param);
   LUALparamIsStringType(L, Param);
}