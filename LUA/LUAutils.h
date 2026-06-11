#ifndef __LUA_UTILS_H__
#define __LUA_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAutils
//
// Description:
//
// Utilities for dealing with LUA language
//
// Author: Eliot Muir
// Date:   Monday, April 26th, 2010 @ 01:28:58 PM
//
//---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLstring.h>
#include <LUA/LUAheader.h>
#include <LUA/LUAudFormat.h>
#include <new>

extern const COLstring BV4;
extern const COLstring L3I;
extern const COLstring IL1;
extern const COLstring B2K;

class COLsink;
class COLdateTime;
class COLostream;
class COLstring;

class LUAprintCallback;

void LUAformatVariableOnStack(lua_State* L, int i, COLostream& Stream, LUAudFormat Formatter);

COLstring LUAstackAsString(lua_State* L);
COLstring LUAtoStringIncludeMeta(lua_State* L, int i);
COLstring LUAlocalsAsString(lua_State* L, lua_Debug* ar);
COLstring LUAlocalsAsString(lua_State* L, lua_Debug* ar, int Start, int End);

COLstring LUAparametersAsString(lua_State* L, int CountOfParam);  //Experimental debug function by Eliot

int LUAcountOfLocal(lua_State* L, lua_Debug* ar);

COLstring LUAstackAsStringCustom(lua_State* L, LUAudFormat pFormatter);


// Convert element on stack at i to a COLstring - this more efficient than luaL_checkstring since
// it gets the length of the string from Lua and therefore saves a call to strlen for creating our
// COLstring
COLstring LUAtoString(lua_State* L, int i);

void LUAgetJsonNullConstant(lua_State* L);

// True iff the item indicated is our particular flavour of light userdata that marks a JSON null.
bool LUAisJsonNull(lua_State* L, int Index);

// True iff the item indicated is a number that can be converted to
// an int without loss of information.
bool LUAisInt(lua_State*, int);

// True iff the item indicated is a Lua array: a table with keys that
// are all the integers in the range from 1 to N, for some N.
bool LUAisArray(lua_State*, int);

// Iterate over an array at position i in increasing key order, calling f()
// on each element.  F() is passed the Lua state, the index of the element
// in the array (its key as an int), and the void* value v.  Inside f(),
// the element can be accessed at stack position -1.
//
// At any time iteration can be interrupted by returning a negative value
// from f().  That value will be returned by LUAwithIPairs(), after the
// stack is restored to its original size upon entry of LUAwithIPairs().
//
// Otherwise, f() should return a non-negative value, indicating how many
// items on the stack should be preserved and returned by LUAwithIPairs().
// The value returned by LUAwithIPairs() will be final count of items
// left on the stack by calls to f().
//
// F() can dirty up the stack all it likes, just don't remove more than
// the top two elements: the element and its index.  LUAwithIPairs()
// preserves values from the end, when requested, the rest are forfeit.
//
// Note that for simplicity this function does not verify that the value
// at i is actually an array (or a table).  See LUAisArray().
//
int LUAwithIPairs(lua_State*, int i, int (*f)(lua_State*,int,void*), void* v);

void LUApcall(lua_State* L, int CountOfArgument, int CountOfReturn, int Error);

// Push a string on to the Lua stack - this more efficient than other methods since
// it gets the length of the string from COLstring::size() and therefore saves a call to strlen
void LUApushString(lua_State* L, const COLstring& String);

void LUAopenBaseLibraries(lua_State* L);

lua_State* LUAopenInterpreter(COLsink* pSink);

// Returns C++ pointer to user data if it matches the given MetaTableName
void* LUAqueryUserData(lua_State* L, int i, const char* MetaTableName);

void LUAformatNumber(lua_State* L, int i, COLostream& Stream);

/*
 * LUApushObject<OfType>(LuaState, MetaTableName)
 * LUApushObject<OfType>(LuaState, CopyFrom, MetaTableName)
 *
 * Use to add new or copies of existing C++ objects into LUA.
 * A metatable will be created, unless one already exists, to
 * call the object's destructor before garbage collection.
 */
template <class T>
class LUApushObject {
   T* pObject;
   void createMeta(lua_State*, const char*);
public:
   LUApushObject(lua_State* L, const char* pMeta) {
      void *v = lua_newuserdata(L, sizeof(T));
      pObject = new (v) T();
      createMeta(L, pMeta);
   }
   template <class X>
   LUApushObject(lua_State* L, X& Obj, const char* pMeta) {
      void *v = lua_newuserdata(L, sizeof(T));
      pObject = new (v) T(Obj);
      createMeta(L, pMeta);
   }
   operator T* ()    const { return pObject; }
   T* operator -> () const { return pObject; }
   static int delObject(lua_State*);
};
template <class T>
int LUApushObject<T>::delObject(lua_State* L) {
   void *v = lua_touserdata(L, -1);
   ((T*)v)->~T();
   return 0;
}
template <class T>
void LUApushObject<T>::createMeta(lua_State* L, const char* pMeta) {
   if(luaL_newmetatable(L, pMeta)) {
      lua_pushliteral(L, "__gc");
      lua_pushcfunction(L, &LUApushObject<T>::delObject);
      lua_settable(L, -3);
   }
   lua_setmetatable(L, -2);
}

/*
 * LUApushPointer<OfType>(LuaState, CopyFrom, MetaTableName)
 *
 * Use to add new or copies of existing C++ pointers into LUA.
 * A metatable will be created, unless one already exists, to
 * call delete on the pointer before garbage collection.
 */
template <class T>
class LUApushPointer {
   T*  pPointer;
   void createMeta(lua_State*, const char*);
public:
   template <class X>
   LUApushPointer(lua_State* L, X& Obj, const char* pMeta) {
      void *v = lua_newuserdata(L, sizeof(T));
      pPointer = new (v) T(Obj);
      createMeta(L, pMeta);
   }
   operator T* ()    const { return pPointer; }
   T* operator -> () const { return pPointer; }
   static int delObject(lua_State*);
};
template <class T>
int LUApushPointer<T>::delObject(lua_State* L) {
   void *v = lua_touserdata(L, -1);
   delete (*(T*)v);
   return 0;
}
template <class T>
void LUApushPointer<T>::createMeta(lua_State* L, const char* pMeta) {
   if(luaL_newmetatable(L, pMeta)) {
      lua_pushliteral(L, "__gc");
      lua_pushcfunction(L, &LUApushPointer<T>::delObject);
      lua_settable(L, -3);
   }
   lua_setmetatable(L, -2);
}

// Returns the name of the metatable of the item at index i.
// Returns NULL if the item has no metatable.
const char* LUAgetMetatableName(lua_State* L, int i);

// Appends the value at stack - 1 to the end of the table located at index.
// This function should only be used with numerically indexed tables.
void LUAappendToTable(lua_State* L, int index);

// Like luaL_checkudata(), but allows us to issue a nicer message.
void* LUAcheckUdata(lua_State* L, int ud, const char* tname, const char* msg);

// Attempts to set pValue to a string representation of supplied UserData
// Returns true if conversion was successful, and false otherwise
bool LUAudToString(lua_State* L, int i, const char** pValue, size_t* pLength);

// This function will attempt to get a string from an argument.  The possible
// inputs are going be either a UserData object which have a __tostring meta
// tag or a string.
bool LUAextractString(lua_State* L, int i, COLstring* pValue);
bool LUAextractString(lua_State* L, int i, const char** pValue, size_t* pLength);


void      LUAsetStringField (lua_State* L, const char* Name, const COLstring& Value);
COLstring LUAgetRegistryString(lua_State* L, const char* Name);

void LUAsetBooleanField(lua_State* L, const char* Name, bool  Value);
bool LUAgetBooleanField(lua_State* L, const char* Name);

void  LUAsetPointerField(lua_State* L, const COLstring& Name, void* Value);
void* LUAgetPointerField(lua_State* L, const char* Name);

// Modes for OnLuaStateCreated callback
typedef enum LUAmode {
   LUA_PRODUCTION_MODE,
   LUA_ANNOTATION_MODE,
   LUA_INTELLISENSE_MODE,
} LUAmode;

void LUAsetOnLuaStateCreated(COLauto<COLclosure2<lua_State*, LUAmode>> pCallback);
void LUArunLuaCallBack(lua_State* L, LUAmode Mode);

void LUAsetOnLuaStateIdle(COLauto<COLclosure1<lua_State*>> pCallback);
void LUArunLuaIdleCallBack(lua_State* L);

void LUAaddMethod(lua_State* L, const char* pName, lua_CFunction pFunction);

void LUAaddConstant(lua_State* L, const char* pName, int Value);
void LUAaddConstant(lua_State* L, const char* pName, const COLstring& Value);

// This suppresses some parts of Lua - I refactored this from LUAopenBaseLibraries.
void LUAosSuppress(lua_State* L);

bool LUAfunctionExists(lua_State* L, const COLstring& FunctionName);

#endif // end of defensive include
