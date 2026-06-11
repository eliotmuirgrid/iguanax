//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUAbvar
//
// Description:
//
// Routine to translate a Lua table into a COLvar.
//
// Author: Eliot Muir
// Date:   Thu  6 Mar 2014 12:50:06 EST
//---------------------------------------------------------------------------
#include "LUAprecomp.h"
#pragma hdrstop

#include "LUAheader.h"
#include "LUAutils.h"
#include "LUAvar.h"

#include <COL/COLauto.h>
#include <COL/COLvar.h>
#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Extract the Lua (1-based) array index
inline static int LUAintKey(lua_State* L){
   COL_FUNCTION(LUAintKey);
   COL_VAR(lua_tointeger(L,-2));
   return lua_tointeger(L,-2); 
}

// JSON doesn't allow mixed keys (numbers and strings). If a mismatch is
// detected then grab the keys in the current object and return the so
// the error message can provide some context.
COLstring LUAlistKeys(lua_State* L, int i){
   COL_FUNCTION(LUAlistKeys);
   COL_VAR(lua_gettop(L));
   COLstring Result;

   lua_pushnil(L);  /* first key */
   while (lua_next(L, i) != 0) {
      switch(lua_type(L, -2)) { // Switch on the key type.
         case LUA_TNUMBER:{
               int NumKey = (int) lua_tonumber(L, -2);
               Result += COLintToString(NumKey) + " ";
               break;
            }
         case LUA_TSTRING:{
               COLstring StrKey = LUAtoString(L,-2);
               Result += "'" + StrKey + "' ";
               break;
            }
         default:
            break;
      }
      lua_pop(L, 1);
   }
   COL_VAR(lua_gettop(L));
   return Result;
}

LUA::LUAconversionResult LUAtoVar(
      lua_State*  L,
      int         StackPos,
      COLvar*     pVar,
      COLlist<COLstring>*     pTraversalStack,
      COLarray<const void*>* pSeenTables
) {
   COL_FUNCTION(LUAtoVar);
   COL_VAR3(lua_gettop(L), StackPos, lua_type(L, StackPos));
   if (StackPos < 0) {
      COL_TRC("Converting relative stack position to absolute");
      StackPos = lua_gettop(L) + 1 + StackPos;
      COL_VAR(StackPos);
      if (StackPos < 0) {
         COL_TRC("Invalid relative position...");
         return LUA::ERROR_LUA_VALUE_UNKNOWN;
      }
   }

   COLauto< COLarray<const void*> > pSafeSeenTables;
   if (pSeenTables == NULL) {
      COL_TRC("Creating pSeenTables");
      pSeenTables = new COLarray<const void*>();
      pSafeSeenTables = pSeenTables;
   }

   switch (lua_type(L, StackPos)) {
   case LUA_TNONE:
      COL_TRC("No args to convert, i.e. the Lua stack was empty or the index was invalid.");
      pVar->reset();
      break;
   case LUA_TSTRING: {
      COL_TRC("String");
         // direct lua API use to avoid creating a temporary COLstring with LUAtoString
         size_t len = 0;
         const char* str = lua_tolstring(L, StackPos, &len);
         pVar->set(str, len);
      }
      break;
   case LUA_TNUMBER:{
      COL_TRC("Number");
         const double DoubleValue = lua_tonumber(L, StackPos);
         const long long IntValue = (long long)(DoubleValue);
         if (IntValue == DoubleValue) {
            pVar->set(IntValue);
         } else {
            pVar->set(DoubleValue);
         }
      }
      break;
   case LUA_TBOOLEAN:
      COL_TRC("Boolean");
      pVar->set((bool)lua_toboolean(L, StackPos));
      break;
   case LUA_TNIL:
      COL_TRC("Nil");
      pVar->reset();
      break;
   case LUA_TTABLE: {
      COL_TRC("Table");
      pVar->reset();

      const void* pThisTable = lua_topointer(L, StackPos);
      int TableIndex = pSeenTables->indexOf(pThisTable);
      COL_VAR(TableIndex);
      if (TableIndex != -1) {
         return LUA::ERROR_LUA_CYCLIC_TABLE;
      }

      LUA::LUAconversionResult RecursionResult;
      pSeenTables->push_back(pThisTable);

      lua_pushnil(L);  /* first key */
      while (lua_next(L, StackPos) != 0) {
 
         switch(lua_type(L, -2)) { // Switch on the key type.
         case LUA_TNUMBER: {
               // It's an array.
               if (pVar->size() > 0 && pVar->isMap()) {
                  // With the traversal stack, it's possible to figure out which table and keys produced this issue
                  return LUA::ERROR_TABLE_MIXED_KEYS;
               }
               int IntKey = LUAintKey(L);
               if (IntKey <= 0) {
                  return LUA::ERROR_ARRAY_INVALID_INDEX;
               }
               COL_TRC("Traversing into index " << IntKey);
               if (pTraversalStack) {
                  pTraversalStack->push_back(COLintToString(IntKey));
               }
               // Note that IntKey-1 is used in the recursion since C++ indices start at 0
               RecursionResult = LUAtoVar(L, lua_gettop(L), &(*pVar)[IntKey-1], pTraversalStack, pSeenTables);
               if (RecursionResult) {
                  // Greater than 0 is failure.
                  return RecursionResult;
               }
               if (pTraversalStack) {
                  pTraversalStack->pop_back();
               }
               COL_VAR(*pVar);
               break;
            }
         case LUA_TSTRING: {
               // It's a dictionary.
               if (pVar->size() > 0 && pVar->isArray()) {
                  return LUA::ERROR_TABLE_MIXED_KEYS;
               }
               // LUAtoString use ok here - can't avoid a COLstring for key lookup
               // Also, the key is being saved here since the stack will be manipulated along the way
               COLstring CurrentKey = LUAtoString(L, -2);
               COL_TRC("Traversing into key " << CurrentKey);
               if (pTraversalStack) {
                  pTraversalStack->push_back(CurrentKey);
               }

               RecursionResult = LUAtoVar(L, lua_gettop(L), &(*pVar)[CurrentKey], pTraversalStack, pSeenTables);
               if (RecursionResult) {
                  return RecursionResult;
               }
               if (pTraversalStack) {
                  pTraversalStack->pop_back();
               }
               COL_VAR(*pVar);
               break;
            }
         default:
            // Only integers and strings allowed as Lua table keys
            return LUA::ERROR_TABLE_INVALID_KEY;
         }
         lua_pop(L, 1);
      }
      if (pVar->isNone()) {
         // empty Lua tables are represented as an empty COLvar Map
         pVar->setMapType();
      }
      break;
   }
   case LUA_TUSERDATA:
      COL_TRC("Userdata");
   case LUA_TLIGHTUSERDATA:{
      COL_TRC("Light userdata");
      if (LUAisJsonNull(L, StackPos)) {
         pVar->reset();
      } else {
         const char* pValue;
         size_t Length;
         if (LUAextractString(L, StackPos, &pValue, &Length)) {
            pVar->set(pValue, Length);
         } else {
            // Could not convert this userdata to string
            return LUA::ERROR_UD_TO_STRING;
         }
      }
      break;
   }
   case LUA_TTHREAD:  // we silently ignore co-routines. 
   case LUA_TFUNCTION:
      // It's not clear what to do with a function at this point
      pVar->reset();
      break;
   default:
      COLASSERT(1==0);
   }

   COL_VAR2(lua_gettop(L), *pVar);

   return LUA::CONVERSION_SUCCESSFUL;
}

void COLvar::fromLua(lua_State* L, int i) {
   COL_METHOD(COLvar::fromLua);
   if (i < 0) {
      i = lua_gettop(L) + i + 1; // XXX revisit this code later
   }
   COLlist<COLstring> TraversalStack;
   LUA::LUAconversionResult Result = LUAtoVar(L, i, this, &TraversalStack);
   if (Result != LUA::CONVERSION_SUCCESSFUL) {
      // There was an error in the traversal, so generate the appropriate error message
      COLstring ErrorDescription;
      switch(Result) {
      case LUA::ERROR_TABLE_MIXED_KEYS:
         ErrorDescription = "Table contains a mix of integers and strings";
         break;
      case LUA::ERROR_TABLE_INVALID_KEY:
         ErrorDescription = "Table index is nil (did you intend to use a string index in quotes)";
         break;
      case LUA::ERROR_ARRAY_INVALID_INDEX:
         ErrorDescription = "Array index is either zero or negative";
         break;
      case LUA::ERROR_UD_TO_STRING:
         ErrorDescription = "Userdata could not be converted to string";
         break;
      case LUA::ERROR_LUA_VALUE_UNKNOWN:
         ErrorDescription = "Could not convert value to string";
         break;
      case LUA::ERROR_LUA_CYCLIC_TABLE:
         ErrorDescription = "This Lua table contains cyclic references and cannot be serialized.";
         break;
      default:
         ErrorDescription = "Unknown error.";
         break;
      }
      COL_VAR(ErrorDescription);
      if (!TraversalStack.size()) { throw COLerror(ErrorDescription); } // This prevents trying to access an empty TraversalStack which would cause a crash due to a failed assertion see IX-2901
      // Also generate a description of where the exception happened
      COLstring ErrorLocation = TraversalStack.front();
      TraversalStack.pop_front();
      // Use the TraversalStack to unwind from where the issue started
      for (COLlist<COLstring>::const_iterator i = TraversalStack.cbegin(); i != TraversalStack.cend(); i++) {
         ErrorLocation += "[" + (*i) + "]";
      }

      COL_ERROR_STREAM_PLAIN(ErrorDescription << " in " << ErrorLocation << ".", COLerror::PreCondition);
   }
   COLASSERT(TraversalStack.empty());
}

void COLvar::toLua(lua_State* L) const {
   COL_METHOD(COLvar::toLua);
   switch (type_) {
   case None:
      lua_pushnil(L);
      break;
   case String:
      LUApushString(L, *(COLstring*)u_.colstring_);
      break;
   case Integer:
      lua_pushinteger(L, u_.int_);
      break;
   case Boolean:
      lua_pushboolean(L, u_.int_);
      break;
   case Double: {
         const int n = int(u_.double_);
         if (n == u_.double_) {
            lua_pushinteger(L, n);
         } else {
            lua_pushnumber(L, u_.double_);
         }
      }
      break;
   case Array: {
         lua_createtable(L, size(), 0);
         if (u_.vec_ && u_.vec_->size() > 0) {
            int i = 1; // one added to COLvar Array index to convert to lua 1-based indexing
            for (COLvarVec::iterator it = u_.vec_->begin(); it != u_.vec_->end(); ++it, ++i) {
               it->toLua(L);
               lua_rawseti(L, -2, i);
            }
         }
      }
      break;
   case Map: {
         lua_createtable(L, 0, size());
         if (u_.map_ && u_.map_->size() > 0) {
            for (COLvarMap::iterator it = u_.map_->begin(); it != u_.map_->end(); ++it) {
               LUApushString(L, it->first);
               it->second.toLua(L);
               lua_rawset(L, -3);
            }
         }
      }
      break;
   default:
      break;
   }
}

void COLvar::toLuaNullsNotNils(lua_State* L) const {
   COL_METHOD(COLvar::toLuaNullsNotNils);
   switch (type_) {
   case None:
      LUAgetJsonNullConstant(L);
      break;
   case String:
      LUApushString(L, *(COLstring*)u_.colstring_);
      break;
   case Integer:
      lua_pushinteger(L, u_.int_);
      break;
   case Boolean:
      lua_pushboolean(L, u_.int_);
      break;
   case Double: {
         const int n = int(u_.double_);
         if (n == u_.double_) {
            lua_pushinteger(L, n);
         } else {
            lua_pushnumber(L, u_.double_);
         }
      }
      break;
   case Array: {
         lua_createtable(L, size(), 0);
         if (u_.vec_ && u_.vec_->size() > 0) {
            int i = 1; // one added to COLvar Array index to convert to lua 1-based indexing
            for (COLvarVec::iterator it = u_.vec_->begin(); it != u_.vec_->end(); ++it, ++i) {
               it->toLuaNullsNotNils(L);
               lua_rawseti(L, -2, i);
            }
         }
      }
      break;
   case Map: {
         lua_createtable(L, 0, size());
         if (u_.map_ && u_.map_->size() > 0) {
            for (COLvarMap::iterator it = u_.map_->begin(); it != u_.map_->end(); ++it) {
               LUApushString(L, it->first);
               it->second.toLuaNullsNotNils(L);
               lua_rawset(L, -3);
            }
         }
      }
      break;
   default:
      break;
   }
}

void LUApushVar(lua_State* L, const COLvar& Var) {
   Var.toLua(L);
}

