//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday, April 26th, 2010 @ 01:28:58 PM
//
//---------------------------------------------------------------------------
#include "LUAutils.h"

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
#include <LUAC/lua.h>
COL_LOG_MODULE;

#include <math.h>  // for modf
#include <time.h>
#include <new>

//t30588 - split up portion of encryption keys
//Check NODLdecrpytionContext.cpp for more details
const COLstring BV4 = "A66E445C";
const COLstring L3I = "2A961989";
const COLstring IL1 = "BF164D11";
const COLstring B2K = "5A45FE06";

const char* LUA_JSON_NULL_REGISTRY_KEY = "json.NULL";

// See #15436 - lua_tostring unfortunately actually alters the variable which breaks the functionality - ouch!
// but it does format doubles more nicely.
void LUAformatNumber(lua_State* L, int i, COLostream& Stream){
   double Integer;
   double FractionPart = fabs(modf(lua_tonumber(L,i), &Integer));  // 1.000000000000000112 --> {1, 1.12e-12}
   if (FractionPart < 1e-10){   // TODO - fairly arbitrary definition.  Good enough for now  - we aren't really handling the output of floats that well here.
      Stream << (COLint64)Integer;   // TODO - probably not good way to convert a double into a number.
   } else {
      Stream << lua_tonumber(L,i);
   }
}

// When reviewing this code please check LUAprintItem in LUAprint.cpp which is similar... yet different Eliot
void LUAformatVariableOnStack(lua_State* L, int i, COLostream& Stream, LUAudFormat pUserDataFormatter){
   COL_FUNCTION(LUAformatVariableOnStack);
   int t = lua_type(L, i);
   COL_VAR(t);
   switch (t) {
   case LUA_TSTRING:
      Stream << '\'' << lua_tostring(L, i) << '\'';
      break;
   case LUA_TBOOLEAN:
      Stream << (bool)(lua_toboolean(L, i) == 1);
      break;
   case LUA_TNUMBER:{
      LUAformatNumber(L, i, Stream);
      }
      break;
   case LUA_TUSERDATA:
      (*pUserDataFormatter)(L, i, Stream);
      break;
   case LUA_TLIGHTUSERDATA:
      (*pUserDataFormatter)(L, i, Stream);
      break;
   case LUA_TFUNCTION:
      Stream << "function";
      break;
   case LUA_TNIL:
      Stream << "nil";
      break;
   case LUA_TTHREAD:
      Stream << "thread";
      break;
   case LUA_TTABLE:
      Stream << "table";
      break;
   default:
      Stream << "unknown type";
      break;
   }
}

void LUAudFormatDefault(lua_State* L, int StackIndex, COLostream& Stream)
{
   Stream << "User data";
}

COLstring LUAstackAsStringCustom(lua_State* L, LUAudFormat pFormatter) {
   COLstring Result;
   COLostream Stream(Result);
   int i;
   int top = lua_gettop(L);
   if (top == 0) {
      return "Empty Stack";
   }
   for (i = 1; i <=top; i++){
      LUAformatVariableOnStack(L, i, Stream, pFormatter);
      if (i != top) Stream << ", ";
   }
   return Result;
}

COLstring LUAlocalsAsString(lua_State* L, lua_Debug* pD){
   COLstring Result;
   COLostream Stream(Result);
   const char* pLocalVarName = NULL;
   int i = 0;
   int TopIndex = lua_gettop(L) + 1;
   while ((pLocalVarName = lua_getlocal(L, pD, ++i))){
      Stream << pLocalVarName << " = ";
      LUAformatVariableOnStack(L, TopIndex, Stream, &LUAudFormatDefault);
      Stream << " ";
      lua_pop(L,1);
   }
   return Result;
}

COLstring LUAlocalsAsString(lua_State* L, lua_Debug* pD, int Start, int End){
   COLstring Result;
   COLostream Stream(Result);
   const char* pLocalVarName = NULL;
   int TopIndex = lua_gettop(L) + 1;
   for (int i= Start; i <= End; i++){
      pLocalVarName = lua_getlocal(L, pD, i);
      if (pLocalVarName){
         LUAformatVariableOnStack(L, TopIndex, Stream, &LUAudFormatDefault);
         Stream << " ";
         lua_pop(L,1);
      }
   }
   return Result;
}

int LUAcountOfLocal(lua_State* L, lua_Debug* pD){
   const char* pLocalVarName = NULL;
   int i = 0;
   while ((pLocalVarName = lua_getlocal(L, pD, ++i))){
      lua_pop(L,1);
   }
   return i-1;
}

COLstring LUAstackAsString(lua_State* L) {
   const int CountOfParam = lua_gettop(L);
   COLstring Result;
   COLostream Stream(Result);
   int i;
   int top = lua_gettop(L);
   if (top == 0){
      COLASSERT (lua_gettop(L) == CountOfParam);  // Should not leave anything on stack.
      return "Empty Stack";
   }
   for (i = 1; i <=top; i++) {
      LUAformatVariableOnStack(L, i, Stream, &LUAudFormatDefault);
      if (i != top) Stream << COL_T(", ");
   }
   COLASSERT (lua_gettop(L) == CountOfParam);  // Should not leave anything on stack.
   return Result;
}

COLstring LUAparametersAsString(lua_State* L, int CountOfParam){
   COLstring Result;
   COLostream Stream(Result);
   for (int i=1; i <= CountOfParam; i++){
      LUAformatVariableOnStack(L, i, Stream, &LUAudFormatDefault);
      if (i != CountOfParam) Stream << COL_T(", ");
   }
   return Result;
}

// Wee bit more efficient for memory allocation
COLstring LUAtoString(lua_State* L, int i){
   COL_FUNCTION(LUAtoString);
   size_t len = 0;
   const char* str = lua_tolstring(L, i, &len);
   COL_VAR(COLstring(str,len));
   return COLstring(str, len);
}

// This will first check for a __tostring meta method, if found will return the result of that
// otherwise it will return LUAtoString(L, i);
COLstring LUAtoStringIncludeMeta(lua_State* L, int i){
   COL_FUNCTION(LUAtoStringIncludeMeta);
   const int LuaType = lua_type(L, i);
   if (LuaType != LUA_TUSERDATA && LuaType != LUA_TTABLE) { return LUAtoString(L, i); }
   COLstring Out;
   COL_VAR(LUAstackAsString(L));
   if (luaL_callmeta(L, -1, "__tostring")) {
      Out = LUAtoString(L, -1);
      lua_pop(L, 1);
   } else {
      COL_DBG("no __tostring metamethod");
   }
   return Out;
}

void LUApushString(lua_State* L, const COLstring& String){
   lua_pushlstring(L, String.c_str(), String.size());
}

static void LUAsetFieldFunction(lua_State* L, const char* FieldName, int (*pFunc)(lua_State* L)) {
   COL_FUNCTION(LUAsetFieldFunction);
   lua_pushcfunction(L, pFunc);
   lua_setfield(L, -2, FieldName);
}

void LUAosSuppress(lua_State* L){
   COL_FUNCTION(LUAosSuppress);
   //disallow some lua api calls
   luaL_dostring(L,"os.exit = nil");
   luaL_dostring(L,"os.setlocale = nil");
   luaL_dostring(L,"io.tmpfile = nil");
   // Need to seed the random number generator on Windows, likely due to
   // it being loaded as a DLL.
   luaL_dostring(L, "math.randomseed(os.time())");
   // TODO - the above might not be needed - Eliot since we are not loading Lua as a DLL
}

void* LUAqueryUserData(lua_State* L, int i, const char* MetaTableName) {
   COL_FUNCTION(LUAqueryUserData);
   void* pResult = lua_touserdata(L, i);
   if (pResult != NULL)
   {  // value is a userdata
      if (lua_getmetatable(L, i))
      {  // does it have a metatable?
         lua_getfield(L, LUA_REGISTRYINDEX, MetaTableName);  // get correct metatable
         if (!lua_rawequal(L, -1, -2)) pResult = NULL;

         lua_pop(L, 2); // Clean up the stack
      }
   }
   return pResult;
}

const char* LUAgetMetatableName(lua_State* L, int i){
   COL_FUNCTION(LUAgetMetatableName);
   if (lua_getmetatable(L, i)){
      const void* pMetatable = lua_topointer(L, -1);
      lua_pop(L, 1); // pop metatable
      lua_pushnil(L); // first key
      while (lua_next(L, LUA_REGISTRYINDEX)){
         // Key is at -2, value is at -1
         if (lua_istable(L, -1) && lua_topointer(L, -1) == pMetatable && lua_type(L, -2) == LUA_TSTRING){
            const char* pMetatableName = lua_tostring(L, -2);
            lua_pop(L, 2); // pop key and value
            return pMetatableName;
         }
         lua_pop(L, 1); // pop value, leave key on top for next lua_next() call.
      }
   }
   return NULL;
}

// Push json.NULL onto the stack
void LUAgetJsonNullConstant(lua_State* L){
   lua_getfield(L, LUA_REGISTRYINDEX, LUA_JSON_NULL_REGISTRY_KEY);
}

bool LUAisJsonNull(lua_State* L, int Index){
   COL_FUNCTION(LUAisJsonNull);
   LUAgetJsonNullConstant(L);
   const void* pJsonNull = lua_topointer(L, -1);
   COLASSERT(pJsonNull);  // Eliot - Ugly if this is null then json.NULL hasn't been registered using LJSONapiAddJsonNamespace
   lua_pop(L, 1);
   return pJsonNull == lua_topointer(L, Index);
}

bool LUAisInt(lua_State* L, int i) {
   COL_FUNCTION(LUAisInt);
   lua_Number x;
   return lua_isnumber(L,i) &&
      (x = lua_tonumber(L,i), (lua_Number)(int)x == x);
}

// In order for a value to be an array, it must be a table with all
// integer keys ranging from 1 to L, where L is also the number of keys
// in the table (i.e., every integer between 1 and L must be a key in the
// table, and every key must be an integer between 1 and L).
//
bool LUAisArray(lua_State* L, int i) {
   COL_FUNCTION(LUAisArray);
   int Top=lua_gettop(L), Size=0, Largest=0, Idx;

   if(i<0) i+=Top+1;  // Make absolute.
   if(!lua_istable(L,i))
      return false;  // All arrays are tables.
   for(lua_pushnil(L); lua_next(L,i); lua_pop(L,1)) {
      // Explicit cast to int to suppress compiler warning. ptrdiff_t is probably
      // the largest integer type the machine supports, as is an int.
      if(!LUAisInt(L,-2) || (Idx = (int)lua_tointeger(L,-2)) <= 0) {
         lua_settop(L,Top);
         return false;  // Key is not a positive integer.
      }
      Size++;  // The number of keys in the table so far.
      if(Idx > Largest) Largest = Idx;  // The largest key in the table so far.
   }
   // We now know that every key in the table is an integer between 1 and
   // Largest (inclusive), and that there are Size keys total.  If Largest
   // is equal to the Size of the table, all integers between 1 and Largest
   // must be keys in the table.  I.e., the table is also an array.
   lua_settop(L,Top);
   return Size == Largest;
}

int LUAwithIPairs(lua_State* L, int i, int (*f)(lua_State*,int,void*), void* v){
   COL_FUNCTION(LUAwithIPairs);
   int Top=lua_gettop(L), Idx, Sp, Ret=0;
   if(i<0) i+=Top+1;  // Make index absolute.
   for(Idx=1, Sp=Top;
       lua_pushinteger(L,Idx), lua_gettable(L,i), !lua_isnil(L,-1);
       lua_settop(L,Sp), Idx++)
   {
      Ret = f(L,Idx,v);
      if(Ret < 0) {
         lua_settop(L,Top);  // Discard new stack contents.
         return Ret;
      } else if(Ret > 0) {
         int Max = lua_gettop(L) - Sp;
         if(Ret >= Max) {
            Sp += Max;  // Preserve the whole stack.
         } else {
            // Rotate the stack so that the return values begin at Sp+1.
            for(int j=Ret; j--;) {
               lua_insert(L, Sp+1);
            }
            Sp += Ret;  // The other values will be cut.
         }
      }
   }
   lua_settop(L,Sp);
   COL_TRC("Returning " << Sp - Top << " items");
   return Sp - Top;  // Return the number of new items.
}

void LUAappendToTable(lua_State* L, int index) {
   COL_FUNCTION(LUAappendToTable);
   // Verify that the value at the given index is a table.
   if (!lua_istable(L, index)) {
      return;
   }

   size_t Key = lua_objlen(L, index) + 1;
   lua_pushinteger(L, Key);

   // lua_settable requires that the value to insert is at the top of the stack
   // with the key below that.
   lua_insert(L, -2);

   // The index of the table is now one less than the index that was passed in
   // to the function.
   lua_settable(L, index - 1);
}

// This will issue a nicer message rather than raising an ugly Lua exception
// like luaL_checkudata()
void* LUAcheckUdata(lua_State* L, int ud, const char* tname, const char* Message){
   COL_FUNCTION(LUAcheckUdata);
   void* p = lua_touserdata(L, ud);
   if (p != NULL) {  /* value is a userdata? */
     if (lua_getmetatable(L, ud)) {  /* does it have a metatable? */
        lua_getfield(L, LUA_REGISTRYINDEX, tname);  /* get correct metatable */
        if (lua_rawequal(L, -1, -2)) {  /* does it have the correct mt? */
           lua_pop(L, 2);  /* remove both metatables */
           return p;
       }
     }
   }

   // Non-returning branch; just raise an error.
   luaL_error(L, "%s", Message);
   return 0; //NOT_REACHED
}

// Attempts to get the string representation of a supplied UserData onto the
// top of the stack
// NOTE:
// Regardless of success, there will be two extra values pushed onto the stack
// Functions using this should pop two before returning
static bool LUApushUdataToStringValue(lua_State* L, int i){
   COL_FUNCTION(LUApushUdataToStringValue);
   // Access the UserData metatable
   // Since we'll need it as an argument later, we need the absolute position of the UserData on the stack
   i = (i < 0) ? lua_gettop(L) + i + 1 : i;
   if (lua_getmetatable(L, i)){
      // Look for an implementation of __tostring and call it
      lua_pushstring(L, "__tostring");
      lua_rawget(L, -2);
      COL_VAR(lua_type(L, -1));
      if (lua_type(L, -1) == LUA_TFUNCTION) {
         COL_TRC("Found a callable __tostring function. Attempting call.");
         // Now to call __tostring, the only argument is the UserData, so get
         // that onto the top of the stack
         lua_pushvalue(L, i);
         lua_call(L, 1, 1);
         // Lua exception triggers longjmp which seems to have objects on stack destroyed as expected
         // Destructors for objects on the stack will be invoked, which should prevent memory leaks
         COL_TRC("Success - __tostring value is now on the stack.");
         // Currently additionally on the stack: Metatable, StringValue
         // (__tostring function and UserData are popped off stack on lua_call)
         return true;
      } else {
         COL_TRC("No valid __tostring function to call.");
      }
      // else, additionally on stack: Metatable, __tostringFunction
      // Either way, two extra values on stack
   } else {
      COL_TRC("No metatable found.");
      // but to be consistent - leave two extra values on the stack
      lua_pushnil(L);
      lua_pushnil(L);
   }
   return false;
}

bool LUAudToString(lua_State* L, int i, const char** pValue, size_t* pLength) {
   COL_FUNCTION(LUAudToString);
   int Top = lua_gettop(L);
   bool Success;

   if ((Success = LUApushUdataToStringValue(L, i))) {
      // The string value is on the stack, so get it into the return
      // Use temporaries in the raw read so the supplied parameters don't get modified
      size_t StringLength;
      const char* StringValue = lua_tolstring(L, -1, &StringLength);
      // There is really no reason that lua_tolstring should fail at this
      // point, but if it does, then this check prevents filling the return parameters with garbage
      if (StringValue){
         *pValue  = StringValue;
         *pLength = StringLength;
         COL_VAR2(*pValue, *pLength);
      } else {
         COL_TRC("Failed to read string value");
         Success = false;
      }
   }
   // LUApushUdataToStringValue always gets two extra values on the top of the stack
   lua_pop(L, 2);
   COL_VAR3(Success, Top, lua_gettop(L));
   COLASSERT(Top == lua_gettop(L));
   return Success;
}

bool LUAextractString(lua_State* L, int i, const char** pValue, size_t* pLength) {
   COL_FUNCTION(LUAextractString);
   int LuaType = lua_type(L, i);
   COL_VAR(LuaType);
   switch(LuaType){
   case LUA_TSTRING:
      // String value can be read directly
      *pValue = lua_tolstring(L, i, pLength);
      COL_VAR2(i, pLength);
      return true;
   case LUA_TTABLE:
   case LUA_TUSERDATA:
   case LUA_TLIGHTUSERDATA:
      // Use the UserData to string helper method
      return LUAudToString(L, i, pValue, pLength);
   }
   return false;
}

bool LUAextractString(lua_State* L, int i, COLstring* pValue){
   COL_FUNCTION(LUAextractString-COLstring);
   // Just make a COLstring out of the result from the other version
   const char* pData;
   size_t Length;
   bool Result = LUAextractString(L, i, &pData, &Length);
   if (Result){
      *pValue = COLstring(pData, Length);
      COL_VAR4(pData, Length, Result, *pValue);
   }
   return Result;
}

void LUAsetStringField(lua_State* L, const char* Name, const COLstring& Value) {
   COL_FUNCTION(LUAsetStringField);
   lua_pushlstring(L, Value.c_str(), Value.size());
   lua_setfield   (L, LUA_REGISTRYINDEX, Name);
}

void LUAsetBooleanField(lua_State* L, const char* Name, bool Value) {
   COL_FUNCTION(LUAsetBooleanField);
   lua_pushboolean(L, Value);
   lua_setfield   (L, LUA_REGISTRYINDEX, Name);
}

void LUAsetPointerField(lua_State* L, const COLstring& Name, void* Value) {
   COL_FUNCTION(LUAsetPointerField);
   lua_pushlightuserdata(L, Value);
   lua_setfield         (L, LUA_REGISTRYINDEX, Name.c_str());
}

COLstring LUAgetRegistryString(lua_State* L, const char* Name) {
   COL_FUNCTION(LUAgetRegistryString);
   lua_getfield(L, LUA_REGISTRYINDEX, Name);
   COLstring Value = LUAtoString(L, -1);
   lua_pop(L, 1);
   return Value;
}

bool LUAgetBooleanField(lua_State* L, const char* Name) {
   COL_FUNCTION(LUAgetBooleanField);
   lua_getfield(L, LUA_REGISTRYINDEX, Name);
   bool Value = lua_toboolean(L, -1);
   lua_pop(L, 1);
   return Value;
}

void* LUAgetPointerField(lua_State* L, const char* Name) {
   COL_FUNCTION(LUAgetPointerField);
   lua_getfield(L, LUA_REGISTRYINDEX, Name);
   void* Value = lua_touserdata(L, -1);
   lua_pop(L, 1);
   return Value;
}

// Used by DBD to push data to lua_state after one is constructed in TRN and BLUA.
// TODO: It's either one static var here or two static vars in TRN and BLUA.
// Find a better way.
static COLauto<COLclosure2<lua_State*, LUAmode>> LUAonStateCreated;
void LUAsetOnLuaStateCreated(COLauto<COLclosure2<lua_State*, LUAmode>> pCallback) {
   LUAonStateCreated = pCallback;
}

void LUArunLuaCallBack(lua_State* L, LUAmode Mode){
   COL_FUNCTION(LUArunLuaCallBack);
   if (LUAonStateCreated.get()){
      COL_TRC("Running Lua callback");
      LUAonStateCreated->run(L, Mode);
   } else {
      COL_TRC("No Lua Callback");
   }
}

static COLauto<COLclosure1<lua_State*>> LUAonStateIdle;
void LUAsetOnLuaStateIdle(COLauto<COLclosure1<lua_State*>> pCallback) {
   LUAonStateIdle = pCallback;
}

void LUArunLuaIdleCallBack(lua_State* L){
   COL_FUNCTION(LUArunLuaIdleCallBack);
   if (LUAonStateIdle.get()){
      COL_TRC("Running Lua callback");
      LUAonStateIdle->run(L);
   } else {
      COL_TRC("No Lua Callback");
   }
}

void LUAaddMethod(lua_State* L, const char* pName, lua_CFunction pFunction){
   COL_FUNCTION(LUAaddMethod);
   lua_pushstring(L, pName);
   lua_pushcfunction(L, pFunction);
   lua_settable(L, -3);
}


void LUAaddConstant(lua_State* L, const char* pName, int Value){
   lua_pushstring(L, pName);
   lua_pushinteger(L, Value);
   lua_settable(L, -3);
}

void LUAaddConstant(lua_State* L, const char* pName, const COLstring& Value){
   lua_pushstring(L, pName);
   LUApushString(L, Value);
   lua_settable(L, -3);
}

bool LUAfunctionExists(lua_State* L, const COLstring& FunctionName){
   COL_FUNCTION(LUAfunctionExists);
   COL_VAR(FunctionName);
   lua_getglobal(L, FunctionName.c_str());
   bool FunctionExists = lua_isfunction(L, -1);
   lua_pop(L, 1);
   return FunctionExists;
}
