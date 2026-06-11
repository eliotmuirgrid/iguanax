//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUALprint
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Friday, December 23rd, 2011 @ 11:44:22 AM
//
//---------------------------------------------------------------------------
#include "LUALprint.h"

#include "LUALutils.h"

#include <TRN/TRNconstant.h>
#include <TRN/TRNannotationState.h>
#include <THTM/THTMprinter.h>
#include <NOD/NODtype.h>

#include <LUA/LUAtest.h>
#include <LUA/LUAutils.h>

#include <COL/COLsink.h>
#include <COL/COLlog.h>

COL_LOG_MODULE;


static void LUALprintItemImpl(lua_State* L, int i, bool LUALprint, COLostream& Stream, COLarray<const void*>& TableStack);

bool LUALisTableInStack(const void* pTable, const COLarray<const void*>& TableStack, COLostream& Stream, bool LUALprint){
   COL_FUNCTION(LUALisTableInStack);
   int MostRecentTable = TableStack.size()-1;
   int i = TableStack.indexOf(pTable);
   if (i < 0){
      return false;
   } else {
      COLstring TableIdentifier;
      if (i == MostRecentTable){
         TableIdentifier = "<self>";
      } else if (i == MostRecentTable-1) {
         TableIdentifier = "<parent table up 1 level>";
      } else {
         TableIdentifier = "<parent table up " + COLintToString(i) + " levels>";
      }
      if (LUALprint){
         THTMbeginSpan(Stream, NOD_SPAN_LIGHT);
         THTMprintEscaped(Stream, TableIdentifier.c_str(), TableIdentifier.size());
         THTMendSpan(Stream);
      } else {
         Stream << TableIdentifier;
      }
      return true;
   }
}

bool LUALhtmlFilterTableItem(lua_State* L, int TableIndex, int KeyIndex, int ValueIndex) {
   COL_FUNCTION(LUALhtmlFilterTableItem);
   return lua_rawequal(L, TableIndex, LUA_GLOBALSINDEX) &&
          lua_type(L, KeyIndex) == LUA_TSTRING &&
          strcmp(lua_tostring(L, KeyIndex), TRN_GLOBAL_INTELLISENSE_VARIABLE) == 0;
}

static void LUALprintTable(lua_State* L, int i, bool LUALprint, COLostream& Stream, COLarray<const void*>& TableStack){
   COL_FUNCTION(LUALprintTable);
   int top = lua_gettop(L);
   COL_VAR(LUAstackAsString(L));
	COL_VAR2(i, top);
	COL_VAR(lua_type(L, i));
	COL_VAR(TableStack.size());
	COLASSERT(i > 0); // won't work unless i is a real index, not a stack-top relative index.

   const void* pThisTable = lua_topointer(L, i);
   if (LUALisTableInStack(pThisTable, TableStack, Stream, LUALprint)){
      return;
   }

   lua_pushnil(L);  // first key
   bool First = true;
   Stream << '{';
   TableStack.push_back(pThisTable);
   while (lua_next(L, i) != 0){
      if (!LUALprint || !LUALhtmlFilterTableItem(L, top, top+1, top+2)){
         if (First){ 
            First = false;
         } else Stream << COL_T(", ");
         Stream << '[';
         LUALprintItemImpl(L, top+1, LUALprint, Stream, TableStack);
         Stream << COL_T("]=");
         LUALprintItemImpl(L, top+2, LUALprint, Stream, TableStack);
      }
      // removes 'value'; keeps 'key' for next iteration 
      lua_pop(L, 1);
   }
   TableStack.pop_back();
   Stream << '}';
   //lua_pop(L, 1);
}

// Like luaL_callmeta(), except that errors are not thrown.
// Also we switch off LUA the debug hook just in case a customer has
// written their own __tostring meta method.
// Here's a code fragment to exercise this edge case
/* 
MyObj = {} 
function MyObj.__tostring(O)
   print(O)
   return O.x..''
end

function main()
   local T1 = {x=783}
   setmetatable(T1, MyObj)
end
*/
static bool LUALsafeCallMeta(lua_State* L, int i, const char* pField, int CountOfResults){
   COL_FUNCTION(LUALsafeCallMeta);
   if(!luaL_getmetafield(L, i, pField)) {
      COL_TRC("Meta table does not contain " << pField);
      return false;
   }
   if(!lua_isfunction(L, -1)) {
      COL_TRC(pField << " is not a function.");
      lua_pop(L, 1);
      LUApushString(L, COLstring(pField) + " in meta table should be a function.");
      LUApushString(L, COLstring(pField) + " in meta table should be a function.");
      LUApushString(L, COLstring(pField) + " in meta table should be a function.");
      return true;
   }   
   lua_pushvalue(L, i);
   if (LUALisEnvironmentTest(L)){ TRNgetAnnotationState(L)->m_AnnotationsEnabled = false; }
   COL_TRC("About to call lua_pcall");
   int Result = lua_pcall(L, 1, CountOfResults, 0);
   COL_VAR(Result);
   if (LUALisEnvironmentTest(L)){ TRNgetAnnotationState(L)->m_AnnotationsEnabled = true; }
   if (Result != 0){
      COL_TRC("Error thrown from " << pField << " call.");
      lua_pushvalue(L,-1);
      LUApushString(L, "error");
   }
   return true;
}

static void LUALprintItemLuaString(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemLuaString);
   // beware lua_tostring converts numbers into strings.
   if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_STRING);
      Stream << '\'';
      THTMprintEscapedLiteral(Stream, lua_tostring(L,i), lua_strlen(L,i));
      Stream << '\'';
      THTMendSpan(Stream);
   } else {
      Stream.sink()->write(lua_tostring(L,i), lua_strlen(L,i));
   }
}

static void LUALprintItemNil(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemNil);
   if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream.sink()->write("nil", 3);
      THTMendSpan(Stream);
   } else {
      Stream.sink()->write("nil", 3);
   }
}

static void LUALprintItemBool(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemBool);
   if (LUALprint) THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
   if (lua_toboolean(L,i)){ Stream.sink()->write("true",4);  } 
   else                   { Stream.sink()->write("false",5); }
   if (LUALprint) THTMendSpan(Stream);
}

static void LUALprintItemFunction(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemFunction);
   if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream.sink()->write("function", 8);
      THTMendSpan(Stream);
   } else {
      Stream.sink()->write("function", 8);
   }
}

static void LUALprintItemLightUserData(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemLightUserData);
    if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream << Stream.sink()->write("lightuserdata", 13);
      THTMendSpan(Stream);
   } else {
      Stream << Stream.sink()->write("lightuserdata", 13);
   }
}

static void LUALprintItemThread(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemThread);
   if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_KEYWORD);
      Stream.sink()->write("thread", 6);
      THTMendSpan(Stream);
   } else {
      Stream.sink()->write("thread", 6);
   }
}

static void LUALprintItemNumber(lua_State* L, int i, bool LUALprint, COLostream& Stream){
   COL_FUNCTION(LUALprintItemNumber);
   if (LUALprint){
      THTMbeginSpan(Stream, NOD_SPAN_NUMBER);
      LUAformatNumber(L, i, Stream);
      THTMendSpan(Stream);
   } else {
      LUAformatNumber(L, i, Stream);
   }  
}

static void LUALprintItemUserDataOrTable(lua_State* L, int i, bool LUALprint, COLostream& Stream, COLarray<const void*>& TableStack){
   COL_FUNCTION(LUALprintItemUserDataOrTable);
   COLstring ShortLabel, LongLabel;
   int ProtocolType, NodeType; // not used
   LUALdisplayComplexItem(L, i, &ShortLabel, &LongLabel, ProtocolType, NodeType, &TableStack, LUALprint);
   Stream << (LongLabel.is_null() ? ShortLabel : LongLabel);
}

// When reviewing this code please check LUAformatVariableOnStack in LUAutils.cpp which is similar... yet different Eliot
static void LUALprintItemImpl(lua_State* L, int i, bool LUALprint, COLostream& Stream, COLarray<const void*>& TableStack){
   int t = lua_type(L,i);  // We've handled all the types lua_type can return in 5.1 so we should never hit the default.
   switch(t){
      case LUA_TSTRING:        LUALprintItemLuaString      (L, i, LUALprint, Stream);             break;
      case LUA_TNIL:           LUALprintItemNil            (L, i, LUALprint, Stream);             break;
      case LUA_TBOOLEAN:       LUALprintItemBool           (L, i, LUALprint, Stream);             break;
      case LUA_TFUNCTION:      LUALprintItemFunction       (L, i, LUALprint, Stream);             break;
      case LUA_TLIGHTUSERDATA: LUALprintItemLightUserData  (L, i, LUALprint, Stream);             break;  // shouldn't really happen - our userdata objects should have their own meta print methods
      case LUA_TTHREAD:        LUALprintItemThread         (L, i, LUALprint, Stream);             break;
      case LUA_TNUMBER:        LUALprintItemNumber         (L, i, LUALprint, Stream);             break;
      case LUA_TUSERDATA:
      case LUA_TTABLE:         LUALprintItemUserDataOrTable(L, i, LUALprint, Stream, TableStack); break;
      default:
         COLASSERT(1==0);
   }
}

static void LUALparseProtocolAndNodeType(lua_State* L, int i, int& ProtocolTypeOut, int& NodeTypeOut, const int DefaultNodeType){
   int ProtocolType = -1, NodeType = -1;
   if (lua_isstring(L, i)){
      COLstring DisplayTypeString = LUAtoString(L, i);
      if (DisplayTypeString.find(":") == npos){
         DisplayTypeString.prepend(LUALluaProtocolName + ":");
      }
      NODprotocolAndNodeTypeNameToIds(DisplayTypeString, ProtocolType, NodeType);
   }
   ProtocolTypeOut = (ProtocolType == -1 ? LUAL_LUA_PROTOCOL : ProtocolType);
   NodeTypeOut     = (NodeType     == -1 ? DefaultNodeType   : NodeType);
}

static inline void LUALstringFromStack(lua_State* L, int i, COLstring& Out, bool LUALprint){
   if (LUALprint){
      Out.clear();
      COLostream OutStream(Out);
      THTMprintEscaped(OutStream, lua_tostring(L,i), lua_strlen(L,i));
   } else {
      Out = LUAtoString(L, i);
   }
}

static void LUALprintMaxTableStackSizeMessage(bool LUALprint, COLostream& Stream){
	COL_FUNCTION(LUALprintMaxTableStackSizeMessage);
	COLstring Message = COL_T("Maximum recursion viewing level of ") + COLintToString(TRN_MAX_RECURSION_LEVEL) + COL_T(" reached.");
	int MessageSize = Message.size();
	if (LUALprint){
		THTMbeginSpan(Stream, NOD_SPAN_ERR);
		Stream << '\'';
		THTMprintEscapedLiteral(Stream, Message.c_str(), MessageSize);
		Stream << '\'';
		THTMendSpan(Stream);
	} else {
		Stream << Message;
	}
}

static void LUALmetamethodDisplay(lua_State* L, COLstring* pShortLabel, COLstring* pLongLabel, int& ProtocolType, int& NodeType, bool LUALprint, const int LuaType){
   COL_FUNCTION(LUALmetamethodDisplay);
   if (!lua_isnil(L, -3))  { LUALstringFromStack(L, -3, *pShortLabel, LUALprint); }
   if (pLongLabel){
      if (lua_isnil(L, -2)){ (*pLongLabel) = *pShortLabel; } 
      else                 { LUALstringFromStack(L, -2, *pLongLabel, LUALprint); }
   }
   LUALparseProtocolAndNodeType(L, -1, ProtocolType, NodeType, LuaType);
   lua_pop(L, 3); // pop _display results
}

static void LUALmetamethodToString(lua_State* L, COLstring* pShortLabel, COLstring* pLongLabel, int& ProtocolType, int& NodeType, bool LUALprint, const int LuaType){
   COL_FUNCTION(LUALmetamethodToString);
   if (!lua_isnil(L, -1)){ LUALstringFromStack(L, -1, *pShortLabel, LUALprint); }
   if (pLongLabel) (*pLongLabel) = *pShortLabel;
   lua_pop(L, 1); // pop __tostring result
   ProtocolType = LUAL_LUA_PROTOCOL;
   NodeType     = LuaType;
}

static void LUALprintMetaTable(const char* pMetatableName, COLstring* pShortLabel, COLstring* pLongLabel, int& ProtocolType, int& NodeType, bool LUALprint, const int LuaType){
   COL_FUNCTION(LUALprintMetaTable);
   if (LUALprint){
      COLostream Stream(*pShortLabel);
      THTMprintEscaped(Stream, pMetatableName, strlen(pMetatableName)); // need to dig into this more
   } else {
      (*pShortLabel) = pMetatableName;
   }
   if (pLongLabel) (*pLongLabel) = *pShortLabel;
   ProtocolType = LUAL_LUA_PROTOCOL;
   NodeType     = LuaType;
}

bool LUALdisplayComplexItem(
   lua_State* L,
   int i,
   COLstring* pShortLabel, // output
   COLstring* pLongLabel, // output, can be NULL
   int& ProtocolType, // output
   int& NodeType, // output
   COLarray<const void*>* pTableStack,
   bool LUALprint
){
   COL_FUNCTION(LUALdisplayComplexItem);
	COLPRECONDITION(pShortLabel);
   pShortLabel->clear();
   if (pLongLabel) pLongLabel->clear();
   const int LuaType = lua_type(L, i);
   if (LuaType != LUA_TUSERDATA && LuaType != LUA_TTABLE){
      // In case the item isn't actually complex, let's just handle it anyway.
      COLostream LuaItemStream(*pShortLabel);
      if (pTableStack){
         LUALprintItemImpl(L, i, LUALprint, LuaItemStream, *pTableStack);
      } else {
         COLarray<const void*> TableStack;
         LUALprintItemImpl(L, i, LUALprint, LuaItemStream, TableStack);
      }
      if (pLongLabel) (*pLongLabel) = *pShortLabel;
      ProtocolType = LUAL_LUA_PROTOCOL;
      NodeType     = LuaType;
      return false;
   } else if (LUALsafeCallMeta(L, i, "__display", 3)){  // otherwise first check for a __display method, then a __tostring method to try to display it that way
      LUALmetamethodDisplay(L, pShortLabel, pLongLabel, ProtocolType, NodeType, LUALprint, LuaType);
      return true;
   } else if (LUALsafeCallMeta(L, i, "__tostring", 1)){
      LUALmetamethodToString(L, pShortLabel, pLongLabel, ProtocolType, NodeType, LUALprint, LuaType);
      return true;
   } else if (const char* pMetatableName = LUAgetMetatableName(L, i)) { // next see if its a metatable, if so print its name
      LUALprintMetaTable(pMetatableName, pShortLabel, pLongLabel, ProtocolType, NodeType, LUALprint, LuaType);
      return true;
   } else {  // else print a basic table out.
      COL_TRC("Have a basic table");
      (*pShortLabel) = lua_typename(L, LuaType);
      if (pLongLabel && LuaType == LUA_TTABLE){
         COLostream SerializedTableStream(*pLongLabel);
         if (pTableStack){
            if(pTableStack->size() < TRN_MAX_RECURSION_LEVEL){
				   LUALprintTable(L, i, LUALprint, SerializedTableStream, *pTableStack);
				}else{
			      //Maximum safe recursion depth has been reached.
				   //Print message for user as long label. 
			      LUALprintMaxTableStackSizeMessage(LUALprint, SerializedTableStream);
		      }
         } else {
            COLarray<const void*> TableStack;
            LUALprintTable(L, i, LUALprint, SerializedTableStream, TableStack);
         }
      }
      ProtocolType = LUAL_LUA_PROTOCOL;
      NodeType     = LuaType;
      // One might think we could return false here (saying that nothing but
      // standard serialization has been done) in all cases, as the serialized Lua
      // items should be reproducible.  But there may be table loops, which result
      // in non-standard labels (see LUALisTableInStack()).  So, if the "long label"
      // is being used, it may contain information that would be hard to reproduce
      // later from a copy of the tree.
      return (pLongLabel && LuaType == LUA_TTABLE ? true : false);
   }
}

void LUALprintItem(lua_State* L, int i, COLostream& Stream, bool LUALprint){
   COLarray<const void*> TableStack;
   // The calls to LUALprintItem seem to only want string representations, so omitting setting up a COLlist<COLstring> to catch function names.
   LUALprintItemImpl(L, i, LUALprint, Stream, TableStack); 
}

