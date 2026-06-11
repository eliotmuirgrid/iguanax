//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODLutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, October 14th, 2010 @ 12:48:53 PM
//
//---------------------------------------------------------------------------
// #include "NODLprecomp.h"
// #pragma hdrstop

#include "NODLutils.h"
// #include "NODLglobals.h"
// #include "NODLnode.h"
// #include "NODLjson.h"
// #include "NODLhl7.h"
// #include "NODLenvironment.h"
// #include "NODLcurl.h"
#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAutils.h>
// #include <LUAC/lauxlib.h>
#include <LUAL/LUALutils.h>
#include <LND/LNDutils.h>
#include <NOD/NODtype.h>
// #include <DBC/DBCsslOptions.h>


COL_LOG_MODULE;

const int NODL_LUA_PROTOCOL = 0;

const COLstring NODLluaProtocolName("lua");

const COLstring NODLnullDisplayType("null");
const COLstring NODLunknownDisplayType("unknown");
const COLstring NODLerrorDisplayType("error");
const COLstring NODLwarningDisplayType("warning");

const COLstring NODLnilDisplayType("nil");
const COLstring NODLbooleanDisplayType("boolean");
const COLstring NODLnumberDisplayType("number");
const COLstring NODLstringDisplayType("string");
const COLstring NODLtableDisplayType("table");
const COLstring NODLfunctionDisplayType("function");
const COLstring NODLuserdataDisplayType("userdata");
const COLstring NODLthreadDisplayType("thread");

#define NODL_LUA_ERROR_STREAM(_Message) { COLstring _ErrorString; COLostream _ErrorStream(_ErrorString); _ErrorStream << _Message; LUAraiseLuaError(L, _ErrorString);}

void NODLutilsRegisterTypes(){
   COL_FUNCTION(NODLutilsRegisterTypes);
   // Original Lua types
   NODtypeMapAdd(NODLnilDisplayType,      LUA_TNIL,      NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLbooleanDisplayType,  LUA_TBOOLEAN,  NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLnumberDisplayType,   LUA_TNUMBER,   NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLstringDisplayType,   LUA_TSTRING,   NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLtableDisplayType,    LUA_TTABLE,    NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLfunctionDisplayType, LUA_TFUNCTION, NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLuserdataDisplayType, LUA_TUSERDATA, NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLthreadDisplayType,   LUA_TTHREAD,   NODLluaProtocolName, NODL_LUA_PROTOCOL);

   // Our additional Lua types (for display (icon) purposes only)
   NODtypeMapAdd(NODLnullDisplayType,    NODL_JSON_NULL_DISPLAY_TYPE,  NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLunknownDisplayType, NODL_LUA_UNKNOWN_TYPE,        NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLerrorDisplayType,   NODL_ERROR_DISPLAY_TYPE,      NODLluaProtocolName, NODL_LUA_PROTOCOL);
   NODtypeMapAdd(NODLwarningDisplayType, NODL_WARNING_DISPLAY_TYPE,    NODLluaProtocolName, NODL_LUA_PROTOCOL);
}

void NODLaddMethod(lua_State* L, const char* pName, lua_CFunction pFunction){
   COL_FUNCTION(NODLaddMethod);
   lua_pushstring(L, pName);
   lua_pushcfunction(L, pFunction);
   lua_settable(L, -3);
}

void NODLaddConstant(lua_State* L, const char* pName, int Value){
   COL_FUNCTION(NODLaddConstant-int);
   lua_pushstring(L, pName);
   lua_pushinteger(L, Value);
   lua_settable(L, -3);
}

void NODLaddConstant(lua_State* L, const char* pName, const COLstring& Value){
   COL_FUNCTION(NODLaddConstant-COLstring);
   lua_pushstring(L, pName);
   LUApushString(L, Value);
   lua_settable(L, -3);
}

bool NODLfetchParam (lua_State* L, const COLstring& Param){
   COL_FUNCTION(NODLfetchParam);
   if (!lua_istable(L, -1)){
      return false;
   }
   LUApushString(L, Param);
   COL_VAR (lua_gettop (L));
   lua_gettable(L, lua_gettop (L) - 1);
   return !lua_isnil (L, lua_gettop (L));
}

void NODLtableExpectedError(lua_State* L, int i){
   NODL_LUA_ERROR_STREAM("Parameter table expected, got " << lua_typename(L, lua_type(L,i)) << '.');
}

void NODLfetchRequiredParam(lua_State* L, int i, const COLstring& Param, int ExpectedType, const char* pExpectedTypeName){
   COL_FUNCTION(NODLfetchRequiredParam);
   if (!lua_istable(L, i)){
      COL_TRC("parameter " << i << " is not a table.");
      NODLtableExpectedError(L, i);
      return;
   }
   LUApushString(L, Param);
   lua_gettable(L, i);
   COL_VAR2(ExpectedType, lua_type(L,-1));
   if (lua_type(L, -1) != ExpectedType){
      if (lua_type(L,-1) == LUA_TNIL){
         COL_TRC("Got nil expected type.");
         NODL_LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
      } else {
         NODL_LUA_ERROR_STREAM("Parameter '" << Param << "' should be of type " << pExpectedTypeName << '.');
      }
   }
}

bool NODLexpectedBoolParam (lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(NODLexpectedBoolParam);
   NODLfetchRequiredParam(L, i, Param, LUA_TBOOLEAN, "boolean");
   bool Result = lua_toboolean(L, -1)!=0;
   lua_pop(L,1);
   return Result;
}

int NODLexpectedIntParam (lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(NODLexpectedIntParam);
   NODLfetchRequiredParam(L, i, Param, LUA_TNUMBER, "integer");
   int Result = (int)lua_tointeger(L, -1);
   lua_pop(L,1);
   return Result;
}

double NODLexpectedDoubleParam (lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(NODLexpectedDoubleParam);
   NODLfetchRequiredParam(L, i, Param, LUA_TNUMBER, "number");
   double Result = lua_tonumber(L, -1);
   lua_pop(L,1);
   return Result;
}

const NODplace* NODLexpectedNodeParam (lua_State* L, int i, const COLstring& Param){
   COL_FUNCTION(NODLexpectedNodeParam);
   COL_VAR(Param);
   NODLfetchRequiredParam(L, i, Param, LUA_TUSERDATA, "node");
   const NODplace* pNodeData = LNDgetNode(L, -1);  // TODO - not sure what error message this will through.
   lua_pop(L,1);
   return pNodeData;
}

COLstring NODLexpectedStringParamFlexible(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(NODLexpectedStringParamFlexible);
   if (!lua_istable(L, i)) {
      NODLtableExpectedError(L, i);
   }
   LUApushString(L, Param);
   lua_gettable(L, i);

   if (lua_isstring(L, -1)) {
      const COLstring& Value = LUAtoString(L, -1);
      lua_pop(L,1); //pop Param value
      return Value;
   } else if (lua_istable(L, -1)){
      COL_TRC("HIT TEMP ERROR STOP");
      // NODLnativeJsonSerialize(L);
      const COLstring Value = LUAtoString(L, -1);
      COL_VAR(Value);
      lua_pop(L, 1); //NODLnativeJsonSerialize pushes another item onto the stack I'm fairly certain, so we need to pop 2 items.
      return Value;
   } else {
      if (lua_type(L,-1) == LUA_TNIL) {
         COL_TRC("Got nil expected type.");
         NODL_LUA_ERROR_STREAM ("Parameter '" << Param << "' is required.");
      } else if (luaL_getmetafield(L, -1, "__tostring")) {
         if (!lua_isfunction(L, -1)){               
            NODL_LUA_ERROR_STREAM ("Parameter '" << Param << "' must either be a string or have a '__tostring' metamethod.");
         } else {
            lua_pushvalue(L, -2);
            lua_call(L, 1, 1); //Calling __tostring on the Param value
            if (lua_isstring(L, -1)) {
               const COLstring& Value = LUAtoString(L, -1);
               lua_pop(L,2); //pop Param value at i=-2 and __tostring(Param value) at i=-1
               return Value;
            } else {
               NODL_LUA_ERROR_STREAM ("'__tostring' metamethod of parameter '" << Param << "' must return a string.");
            }
         }          
      } else {
         NODL_LUA_ERROR_STREAM ("Parameter '" << Param << "' must either be a string or have a '__tostring' metamethod.");
      }
   }
   return ""; //NOTREACHED - i hope
}

bool NODLfetchOptionalParam(lua_State* L, int i, const COLstring& Param, int ExpectedType, const char* pExpectedTypeName){
   COL_FUNCTION(NODLfetchOptionalParam);
   if (!lua_istable(L, i)){
      NODLtableExpectedError(L, i);
   }
   LUApushString(L, Param);
   lua_gettable(L, i);
   if (lua_type(L, -1) == LUA_TNIL){
      lua_pop(L, 1);
      return false;
   }
   if (lua_type(L, -1) != ExpectedType){
      NODL_LUA_ERROR_STREAM("Parameter '" << Param << "' should be of type " << pExpectedTypeName << '.');
   }
   return !lua_isnil (L, lua_gettop (L));
}

double NODLoptionalDoubleParam(lua_State* L, int i, const COLstring& Param, double DefaultValue){
   COL_FUNCTION(NODLoptionalDoubleParam);
   if (!NODLfetchOptionalParam(L, i, Param, LUA_TNUMBER, "number")){
      return DefaultValue;
   }
   double Result = lua_tonumber(L, -1);
   lua_pop (L, 1); // Left on the stack by NODLfetchOptionalParam
   return Result;    
}

static bool hasParam(lua_State* L, int i, const COLstring& Param, int Type, const char* pTypeName)
{
   COL_FUNCTION(hasParam);
   if(!NODLfetchOptionalParam(L, i, Param, Type, pTypeName))
      return false;
   lua_pop(L,1);
   return true;
}

bool NODLhasIntParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(NODLhasIntParam);
   return hasParam(L, i, Param, LUA_TNUMBER, "integer");
}
bool NODLhasBoolParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(NODLhasBoolParam);
   return hasParam(L, i, Param, LUA_TBOOLEAN, "boolean");
}
bool NODLhasDoubleParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(NODLhasDoubleParam);
   return hasParam(L, i, Param, LUA_TNUMBER, "number");
}
bool NODLhasStringParam(lua_State* L, int i, const COLstring& Param) {
   COL_FUNCTION(NODLhasStringParam);
   return hasParam(L, i, Param, LUA_TSTRING, "string");
}

bool NODLhasFuncParam(lua_State* L, int i, const COLstring& Param) {
   
   return hasParam(L, i, Param, LUA_TFUNCTION, "function");
}

void NODLexpectedFuncParam(lua_State* L, int i, const COLstring& Param) {
   NODLfetchRequiredParam(L, i, Param, LUA_TFUNCTION, "function");
}

void NODLdbConnectionParamToTable(lua_State* L, const char* ParamName) {
   COL_FUNCTION(NODLdbConnectionParamToTable);
   if (lua_gettop(L) == 2 && lua_isuserdata(L,1) && !lua_istable(L,2)) {
      lua_newtable(L);
      lua_pushvalue(L,2); //pushes parameter to top
      lua_setfield(L, -2, ParamName);
      lua_remove(L,2); //removes original parameter
   }   
}

const COLdateTime NODLconvertStringToDateTime(const COLstring& DateTimeAsString){
   COL_FUNCTION(NODLconvertStringToDateTime);
   COL_VAR(DateTimeAsString);
   int nYear=0, nMonth=0, nDay=0, nHour=0, nMin=0, nSec=0;
   int ReturnCode = sscanf(DateTimeAsString.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &nYear, &nMonth, &nDay,  &nHour, &nMin, &nSec);
   COLdateTime DateTime(nYear, nMonth, nDay, nHour, nMin, nSec);
   return DateTime;
}

/**
 * NODLcheckParamsEx - Extended parameter checks that allow for
 * syntactic sugar.
 *
 * As a convenience, we would like to allow functions that take only
 * one required named string param to be called as fnord(xyzzy) instead
 * of fnord{foo=xyzzy}.
 *
 * Those functions should call this wrapper around the regular
 * param check to coerce the single parameter to a table form
 * before forwarding to the real check.
 * 
 * NOTE: We get the named param from the first option in the options
 * array.
 */
void
NODLcheckParamsEx(lua_State* L, int i, const char *const aOptions[])
{
   COL_FUNCTION(NODLcheckParamsEx);
   // If we are being called with a single string-like param, coerce
   // it to a table using the named param in the options.
   if (lua_gettop(L) == 1 && !lua_istable(L, 1)) {
      const char *opt = aOptions[0];
      if (*opt) {
         lua_newtable(L);
         lua_pushvalue(L, 1);    //push parameter to top
         lua_setfield(L, 2, opt);
         lua_remove(L, 1);       //remove original parameter
      }
   }

   // Now, do the real work of validating the (new or existing)
   // param table.
   NODLcheckParams(L, i, aOptions);
}

void NODLcheckParams(lua_State* L, int i, const char *const aOptions[]){
   COL_FUNCTION(NODLcheckParams);
   int Top = lua_gettop(L);
   if(i<0) i+=Top+1;  // Table position needs to be absolute.

   COL_VAR2(lua_istable(L,i), lua_typename(L, lua_type(L,i)));
   if(!lua_istable(L,i)) {
      NODL_LUA_ERROR_STREAM("Parameter table expected, got "
         << lua_typename(L, lua_type(L,i)) << '.');
   }

   for(lua_pushnil(L); lua_next(L,i); lua_pop(L,1)) {
      if(lua_type(L,-2) != LUA_TSTRING) {
         NODL_LUA_ERROR_STREAM("Use named argument form e.g. {data=Data}.");
      }
      const char* pKey = lua_tostring(L,-2);
      for(const char *const *opt=aOptions; *opt; opt++) {
         if(strcmp(pKey, *opt) == 0) goto known_option;
      } 
      NODL_LUA_ERROR_STREAM("Unknown argument " << pKey << " or not using named argument form e.g. {data=Data}");
known_option:;
   }
   lua_settop(L, Top);
}
