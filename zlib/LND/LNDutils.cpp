//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDnode
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, May 18th, 2010 @ 12:47:45 PM
//
//---------------------------------------------------------------------------
#include "LNDutils.h"
#include "LNDdebug.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>


#include <LUAL/LUALutils.h>

#include <TRN/TRNannotationState.h>  // We used to have NODnodeCallback
#include <TRN/TRNannotationNode.h>

#include <NOD/NODplace.h>
#include <NOD/NODaddress.h>
#include <NOD/NODoperation.h>

#include <NTV/NTVtreeValue.h>

#include <NOX/NOXconstant.h>

const char* LND_TREE_METATABLE              ="if.node";
const char* LND_TREE_METATABLE_WITH_ADDRESS ="if.node2";


static NODplace* LNDgetArgumentTree(lua_State* L, int i) {
   void* ud = LUAcheckUdata(L, i, LND_TREE_METATABLE, "'if node' expected");

   return ((COLref<NODplace>*)ud)->get();
}

static LNDplaceAddress* LNDgetArgumentTreeWithAddress(lua_State* L, int i) {
   void* ud = LUAcheckUdata(L, i, LND_TREE_METATABLE_WITH_ADDRESS, "'if node2' expected");

   return (LNDplaceAddress*)ud;
}

bool LNDisNodeWithAddress(lua_State *L, int i){
   COL_FUNCTION(LNDisNodeWithAddress); 
   int StackTop = lua_gettop(L);
   void *p = lua_touserdata(L, i);
   if (p != NULL) {  
      COL_TRC("value is a userdata");
      if (lua_getmetatable(L, i)) { 
         COL_TRC("It have a metatable?");
         lua_getfield(L, LUA_REGISTRYINDEX, LND_TREE_METATABLE_WITH_ADDRESS);  // get correct metatable
         if (lua_rawequal(L, -1, -2)) {  // does it have the correct mt?
            lua_settop(L, StackTop);
            return true;
         }
     }
  }
  COL_TRC("This is not a node with an address."); 
  lua_settop(L, StackTop);
  return false;
}

bool LNDisNode(lua_State* L, int i) {
   COL_FUNCTION(LNDisNode);
   int StackTop = lua_gettop(L);
   void *p = lua_touserdata(L, i);
   if (p != NULL) {  // value is a userdata?
      if (lua_getmetatable(L, i)) {  // does it have a metatable?
         lua_getfield(L, LUA_REGISTRYINDEX, LND_TREE_METATABLE);  // get correct metatable
         if (lua_rawequal(L, -1, -2)) {  // does it have the correct mt?
            lua_settop(L, StackTop);
            COL_TRC("is a node.");
            return true;
         }
         lua_pop(L, 1); // pop LND_TREE_METATABLE
         lua_getfield(L, LUA_REGISTRYINDEX, LND_TREE_METATABLE_WITH_ADDRESS);  // get correct metatable (with address)
         if (lua_rawequal(L, -1, -2)) {  // does it have the correct mt?
            lua_settop(L, StackTop);
            COL_TRC("is a node.");
            return true;
         }
      }
   }
   lua_settop(L, StackTop);
   COL_TRC("false");
   return false;
}

static void LNDassertNodeIsComplex(const NODplace* pPlace){
   // TODO these error messages could be more helpful.  For example, instead of saying
   // "this" or "this node", we can give the name of the node.
   if (pPlace->isSimple()){
      COL_ERROR_STREAM_PLAIN(COL_T("This is a leaf node without children."), NOD_IS_LEAF);
   } else if (pPlace->isVector()){
      COL_ERROR_STREAM_PLAIN(COL_T("This node does not have named members."), NOD_NOT_COMPLEX_TYPE);
   }
}

static NODplace* LNDgetChildToAssignTo(lua_State* L, NODplace* pSelf, int& ChildIndex){
   COL_FUNCTION(LNDgetChildToAssignTo);
   COL_VAR2(pSelf->name(), pSelf->nodeType());
   switch(lua_type(L, 2)){
   case LUA_TSTRING:{
         COL_TRC("Got a string argument");
         COLstring Key = LUAtoString(L,2);
         COL_VAR(Key);
         try{
            LNDassertNodeIsComplex(pSelf);
            ChildIndex = pSelf->childIndex(Key) + 1;
            COL_VAR(ChildIndex);
            if (ChildIndex < 1) NODthrowNoSuchMember(*pSelf, Key);
            return &pSelf->child(ChildIndex - 1);
         }
         catch (const COLerror& Error){
            luaL_error(L, "%s", Error.description().c_str());
            return NULL;
         }
      }
      break;
   case LUA_TNUMBER:{
         COL_TRC("Got a number argument");
         ChildIndex = luaL_checkint(L, 2);
         try{
            return &pSelf->child(ChildIndex - 1);
         }
         catch (const COLerror& Error){
            if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS) luaL_error(L, "Index %d is out of bounds.", ChildIndex);
            else luaL_error(L, "%s", Error.description().c_str());
            return NULL;
         }
      }
      break;
   default:
      LUA_ERROR_STREAM(COL_T("Bad index (string or number expected, got ") << luaL_typename(L, 2) << ')');
      return NULL;
   }
   return NULL;
}

static int LNDclearChild(lua_State* L, NODplace* pSelf){
   COL_FUNCTION(LNDclearChild);
   switch(lua_type(L, 2)){
   case LUA_TSTRING:{
         COL_TRC("Got a string argument");
         COLstring Key = LUAtoString(L,2);
         try
         {
            pSelf->nullify(Key);
         }
         catch (const COLerror& Error)
         {
            luaL_error(L, "%s", Error.description().c_str());
         }
      }
      break;
   case LUA_TNUMBER:{
         COL_TRC("Got a number argument");
         int ItemIndex = luaL_checkint(L, 2);
         try{
            NODclearChild(*pSelf, ItemIndex - 1);
         }catch (const COLerror& Error){
            if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS) luaL_error(L, "Index %d is out of bounds.", ItemIndex);
            else luaL_error(L, "%s", Error.description().c_str());
            return 0;
         }
      }
      break;
   default:
      LUA_ERROR_STREAM(COL_T("Bad index (string or number expected, got ") << luaL_typename(L, 2) << ')');
      return 0;
   }
   return 0;
}

static int LNDsetChild(lua_State* L) {
   COL_FUNCTION(LNDsetChild);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   NODplace* pSelf = LNDgetArgumentTree(L, 1);
   NODplace* pParent = NULL;
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   if (lua_isnil(L, 3) || LUAisJsonNull(L, 3))   { // NOTE this requires the Lua state to have LUA_JSON_NULL_REGISTRY_KEY defined, which is set up by LJSONapiAddJsonNamespace
      return LNDclearChild(L, pSelf);
   }

   int ChildIndex = -1;
   NODplace* pChild = LNDgetChildToAssignTo(L, pSelf, ChildIndex);
   if (!pChild){
      return 0;
   } else if (pChild->isReadOnly()){
      luaL_error(L, "%s is read-only, and cannot have a new value assigned to it.", pChild->name().c_str());
   }
   // TODO - dang this will co-erce
   if (lua_isstring(L, 3)){  // will take a string or number
      try {
         COL_TRC("Assigning string argument.");
         pChild->setValue(LUAtoString(L, 3));
      }
      catch (const COLerror& Error){
         luaL_error(L, "%s", Error.description().c_str());
      }
   }else if (LNDisNode(L, 3)){
      try{
         NODplace* pOrig = LNDgetArgumentTree(L, 3);
         NODcopyNode(*pOrig, *pChild);
         if (pOrig->isSimple() && pOrig->isNull()){
            // If we get here, it means pOrig is a simple null node, and that
            // assigning it to Child is possible (trees are compatible).
            return LNDclearChild(L, pSelf);
         }
      } catch (const COLerror& Error){
         luaL_error(L, "%s", Error.description().c_str());
      }
   }else{
      LUA_ERROR_STREAM(COL_T("Cannot assign value (string, number, tree node, or nil expected, got ") << luaL_typename(L, 3) << ')');
   }
   return 0;
}

static int LNDsetChildWithAddress(lua_State* L) {
   COL_FUNCTION(LNDsetChildWithAddress);
   COL_VAR(LUAstackAsStringCustom(L,&LNDuserDataFormatter));
   LNDplaceAddress* pSelf = LNDgetArgumentTreeWithAddress(L, 1);

   int ChildIndex = -1;
   NODplace* pChild = LNDgetChildToAssignTo(L, pSelf->m_pPlace.get(), ChildIndex);
   if (!pChild){
      COL_TRC("No child to to assign to.");
      return 0;
   } else if (pChild->isReadOnly()){
      luaL_error(L, "%s is read-only, and cannot have a new value assigned to it.", pChild->name().c_str());
   }

   NODaddress DestinationAddress(pSelf->m_Address);
   DestinationAddress.pushAddressIndex(ChildIndex);

   TRNannotationState* pCallback = LNDgetNodeCallback(L);
   COL_VAR(lua_isnil(L,3));

   if (lua_isnil(L, 3) || LUAisJsonNull(L, 3)){
      COL_TRC("Clearing the child.");
      if (pCallback) {
         TRNstandardOnNodeCleared(TRNcurrentFunctionCall(pCallback->m_Stack), TRNcurrentLine(pCallback->m_Stack), DestinationAddress, pCallback->m_LastDestination);
      }
      return LNDclearChild(L, pSelf->m_pPlace.get());
   }

   // TODO - we co-erce the string here!! ugh.
   if (lua_isstring(L, 3)){  // will take a string or number
      try {
         COL_TRC("Assigning string argument.");
         COLstring Value = LUAtoString(L,3);
         pChild->setValue(Value);
         if (pCallback) {
            TRNstandardOnAssignedChild(TRNcurrentFunctionCall(pCallback->m_Stack), TRNcurrentLine(pCallback->m_Stack), DestinationAddress, Value, pCallback->m_LastDestination);
         }
         return 0;
      }
      catch (const COLerror& Error){
         luaL_error(L, "%s", Error.description().c_str());
      }
   } else if (LNDisNodeWithAddress(L, 3)){
      try
      {
         LNDplaceAddress* pOrig = LNDgetArgumentTreeWithAddress(L, 3);
         NODcopyNode(*pOrig->m_pPlace, *pChild);
         if (pOrig->m_pPlace->isSimple() && pOrig->m_pPlace->isNull()){
            // If we get here, it means pOrig->m_pPlace is a simple null node, and that assigning it to Child is
            // possible (trees are compatible).  In this case,we will treat pOrig->m_pPlace as nil (that is, delete Child).
            if (pCallback) {
               TRNstandardOnNodeCleared(TRNcurrentFunctionCall(pCallback->m_Stack), TRNcurrentLine(pCallback->m_Stack), DestinationAddress, pCallback->m_LastDestination);
            }
            return LNDclearChild(L, pSelf->m_pPlace.get());
         }else{
            if (pCallback) {
               TRNstandardOnNodeCopy(TRNcurrentFunctionCall(pCallback->m_Stack), TRNcurrentLine(pCallback->m_Stack), pOrig->m_Address, DestinationAddress, pCallback->m_LastDestination, pCallback->m_LastSource);
            }
         }
      }
      catch (const COLerror& Error){
         luaL_error(L, "%s", Error.description().c_str());
      }
   } else {
      LUA_ERROR_STREAM(COL_T("Cannot assign value (string, number, tree node, or nil expected, got ") << luaL_typename(L, 3) << ')');
   }
   return 0;
}

// A return value of NULL means nil should be returned.
static void LNDgetChildImpl(lua_State* L, NODplace* pSelf, COLref<NODplace>& pChild){
   COL_FUNCTION(LNDgetChildImpl);
   COL_VAR(LUAstackAsStringCustom(L,&LNDuserDataFormatter));

   switch(lua_type(L, 2)){
   case LUA_TSTRING:{
         COL_TRC("Got a string argument");
         try {
            pChild = &pSelf->child(LUAtoString(L,2));
            return;
         } catch (const COLerror& Error){  // TODO - problematic from a performance point of view???
            // For XML nodes, we return nil instead of throwing an error (#15858).
            if (Error.code() == NOD_NO_SUCH_MEMBER && pSelf->gracefulNoSuchMember()){
               return;
            }
            luaL_error(L, "%s", Error.description().c_str());
         }
      }
      break;
   case LUA_TNUMBER:{
         COL_TRC("Got a number argument");
         int ItemIndex = luaL_checkint(L, 2);
         try {
            if (ItemIndex > pSelf->size() && pSelf->isVector() && pSelf->isHomogeneous() && pSelf->isReadOnly()){
               pChild = pSelf->createDummyItem();
            } else {
               pChild = &pSelf->child(ItemIndex - 1);
            }
            return;
         }
         catch (const COLerror& Error){
            if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS) luaL_error(L, "Index %d is out of bounds.", ItemIndex);
            else luaL_error(L, "%s", Error.description().c_str());
         }
      }
      break;
   default:
      LUA_ERROR_STREAM(COL_T("Bad index (string or number expected, got ") << luaL_typename(L, 2) << ')');
   }
}

//TODO review implementation along with TNDLincludeFunction
bool LNDincludeFunction(const NODplace* pPlace, const COLstring& FunctionName){
   if (FunctionName == "setInner"){
      return pPlace->protocolType() == NOX_XML;
   } else if (FunctionName == "insert" || FunctionName == "append"){
      return pPlace->protocolType() == NOX_XML && pPlace->nodeType() == NOX_ELEMENT;
//   } else if (FunctionName == "isKey"){
//      return pPlace->protocolType() == NTAB_TABLE_PROTOCOL && pPlace->isSimple();
   } else {
      return true;
   }
}

static bool LNDfoundMethod(lua_State* L, const NODplace* pSelf){
   COL_FUNCTION(LNDfoundMethod);

   if (lua_type(L, 2) != LUA_TSTRING) return false;
   lua_getfield(L, LUA_REGISTRYINDEX, "__node");
   lua_pushvalue(L, 2);  // Copy the string to the top of the stack
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   lua_rawget(L, 3);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   if (lua_type(L, 4) == LUA_TFUNCTION && LNDincludeFunction(pSelf, LUAtoString(L, 2))){
      COL_TRC("Found method.");
      return true;
   } else {
      COL_TRC("No method found.");
      lua_pop(L, 2);  // reset the stack to the original ... might not be necessary...
      return false;
   }
}

static int LNDgetChild(lua_State* L){
   COL_FUNCTION(LNDgetChild);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   NODplace* pSelf = LNDgetArgumentTree(L, 1);

   if (LNDfoundMethod(L, pSelf)) return 1;  // we hit a method.

   COLref<NODplace> pChild;
   LNDgetChildImpl(L, pSelf, pChild);

   if (pChild.get()){
      LNDpushNode(L, pChild.get());
   } else {
      lua_pushnil(L);
   }

   return 1;
}

static int LNDgetChildWithAddress(lua_State* L){
   COL_FUNCTION(LNDgetChildWithAddress);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   LNDplaceAddress* pSelf = LNDgetArgumentTreeWithAddress(L, 1);
   COL_VAR(LUAstackAsStringCustom(L, &LNDuserDataFormatter));
   if (LNDfoundMethod(L, pSelf->m_pPlace.get())) return 1;  // we hit a method.

   COLref<NODplace> pChild;
   NODaddress ChildAddress(pSelf->m_Address);

   switch(lua_type(L, 2)){
   case LUA_TSTRING:{
         COL_TRC("Got a string argument");
         COLstring Key = LUAtoString(L,2);
         try{
            COL_VAR(Key);
            LNDassertNodeIsComplex(pSelf->m_pPlace.get());
            int ChildIndex = pSelf->m_pPlace->childIndex(Key);
            if (ChildIndex < 0) NODthrowNoSuchMember(*pSelf->m_pPlace, Key);
            ChildAddress.pushAddressIndex(ChildIndex+1);
            pChild = &pSelf->m_pPlace->child(ChildIndex);
         } catch (const COLerror& Error){
            // For XML nodes, we return nil instead of throwing an error (#15858).
            if (Error.code() == NOD_NO_SUCH_MEMBER && pSelf->m_pPlace->gracefulNoSuchMember()){
               lua_pushnil(L);
               return 1;
            }
            luaL_error(L, "%s", Error.description().c_str());
            return 0;
         }
      }
      break;
   case LUA_TNUMBER:{
         COL_TRC("Got a number argument");
         int ItemIndex = luaL_checkint(L, 2);
         try{
            NODplace* pSelfPlace = pSelf->m_pPlace.get();
            if (ItemIndex > pSelfPlace->size() && pSelfPlace->isVector() && pSelfPlace->isHomogeneous() && pSelfPlace->isReadOnly()){
               pChild = pSelfPlace->createDummyItem();
            } else {
               pChild = &pSelfPlace->child(ItemIndex - 1);
            }
            ChildAddress.pushAddressIndex(ItemIndex);
         } catch (const COLerror& Error){
            if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS) luaL_error(L, "Index %d is out of bounds.", ItemIndex);
            else luaL_error(L, "%s", Error.description().c_str());
            return 0;
         }
      }
      break;
   default:
      LUA_ERROR_STREAM(COL_T("Bad index (string or number expected, got ") << luaL_typename(L, 2) << ')');
      return 0;
   }

   LNDpushNodeWithAddress(L, pChild.get(), ChildAddress);
   return 1;
}

int LNDgetSize(lua_State* L){
   COL_FUNCTION(LNDgetSize);
   NODplace* pSelf = LNDgetArgumentTree(L, 1);
   lua_pushnumber(L, pSelf->size());
   return 1;
}

int LNDgetSizeWithAddress(lua_State* L){
   COL_FUNCTION(LNDgetSizeWithAddress);
   LNDplaceAddress* pSelf = LNDgetArgumentTreeWithAddress(L, 1);
   lua_pushnumber(L, pSelf->m_pPlace->size());
   return 1;
}

int LNDasString(lua_State* L){
   COL_FUNCTION(LNDasString);
   NODplace* pSelf = LNDgetArgumentTree(L, 1);
   COLstring Result;
   COLostream ResultStream(Result);
   NTVtreeValue(ResultStream, *pSelf, false);
   LUApushString(L, Result);
   return 1;
}

int LNDasStringWithAddress(lua_State* L){
   COL_FUNCTION(LNDasString);
   LNDplaceAddress* pSelf = LNDgetArgumentTreeWithAddress(L, 1);
   COLstring Result;
   COLostream ResultStream(Result);
   NTVtreeValue(ResultStream, *pSelf->m_pPlace, false);
   LUApushString(L, Result);
   return 1;
}

int LNDdelete(lua_State* L){
   COL_FUNCTION(LNDdelete);
   void* ud = LUAcheckUdata(L, 1, LND_TREE_METATABLE, "'if node' expected");

   ((COLref<NODplace>*)ud)->~COLref<NODplace>(); // Call destructor
   return 0;
}

int LNDdeleteWithAddress(lua_State* L){
   COL_FUNCTION(LNDdeleteWithAddress);
   LNDplaceAddress* pSelf = LNDgetArgumentTreeWithAddress(L, 1);
   pSelf->~LNDplaceAddress();
   return 0;
}

int LNDconcat(lua_State* L){
   COL_FUNCTION(LNDconcat);
   bool LeftIsString  = lua_isstring(L, 1) !=0;
   bool RightIsString = lua_isstring(L, 2) !=0;

   if (!LeftIsString && !LNDisNode(L, 1)){
      LUA_ERROR_STREAM(COL_T("Bad left side of concatenation (expected string or node, got ") << luaL_typename(L, 1) << ')');
   } else if (!RightIsString && !LNDisNode(L, 2)){
      LUA_ERROR_STREAM(COL_T("Bad right side of concatenation (expected string or node, got ") << luaL_typename(L, 2) << ')');
   }

   COLstring Result;
   if (LeftIsString){
      NODplace* pRightSide = LNDgetArgumentTree(L, 2);
      if (!pRightSide->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = LUAtoString(L, 1) + pRightSide->value();
   } else if (RightIsString) {
      NODplace* pLeftSide = LNDgetArgumentTree(L, 1);
      if (!pLeftSide->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = pLeftSide->value() + LUAtoString(L, 2);
   } else {
      NODplace* pLeftSide = LNDgetArgumentTree(L, 1);
      NODplace* pRightSide = LNDgetArgumentTree(L, 2);
      if (!pRightSide->isSimple() || !pLeftSide->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = pLeftSide->value() + pRightSide->value();
   }
   LUApushString(L, Result);
   return 1;
}

int LNDconcatWithAddress(lua_State* L){
   COL_FUNCTION(LNDconcatWithAddress);
   bool LeftIsString  = lua_isstring(L, 1) !=0;
   bool RightIsString = lua_isstring(L, 2) !=0;

   if (!LeftIsString && !LNDisNodeWithAddress(L, 1)){
      LUA_ERROR_STREAM(COL_T("Bad left side of concatenation (expected string or node, got ") << luaL_typename(L, 1) << ')');
   } else if (!RightIsString && !LNDisNodeWithAddress(L, 2)){
      LUA_ERROR_STREAM(COL_T("Bad right side of concatenation (expected string or node, got ") << luaL_typename(L, 2) << ')');
   }

   COLstring Result;
   if (LeftIsString){
      LNDplaceAddress* pRightSide = LNDgetArgumentTreeWithAddress(L, 2);
      if (!pRightSide->m_pPlace->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = LUAtoString(L, 1) + pRightSide->m_pPlace->value();
   }
   else if (RightIsString){
      LNDplaceAddress* pLeftSide = LNDgetArgumentTreeWithAddress(L, 1);
      if (!pLeftSide->m_pPlace->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = pLeftSide->m_pPlace->value() + LUAtoString(L, 2);
   } else{
      LNDplaceAddress* pLeftSide = LNDgetArgumentTreeWithAddress(L, 1);
      LNDplaceAddress* pRightSide = LNDgetArgumentTreeWithAddress(L, 2);
      if (!pRightSide->m_pPlace->isSimple() || !pLeftSide->m_pPlace->isSimple()) luaL_error(L, "Nodes with children cannot be used in concatenation.");
      Result = pLeftSide->m_pPlace->value() + pRightSide->m_pPlace->value();
   }
   LUApushString(L, Result);
   return 1;
}

void LNDdefineTree(lua_State* L){
   COL_FUNCTION(LNDdefineTree);
   luaL_newmetatable(L, LND_TREE_METATABLE);
   LUAaddMethod(L, "__newindex", &LNDsetChild);   // blah[x] = y  or blah.x = y  blah:key(i)  then it will called with 'key'
   LUAaddMethod(L, "__index", &LNDgetChild);
   LUAaddMethod(L, "__len", &LNDgetSize);
   LUAaddMethod(L, "__gc", &LNDdelete);
   LUAaddMethod(L, "__tostring", &LNDasString);
   LUAaddMethod(L, "__concat", &LNDconcat);
   lua_pop(L, 1);

   luaL_newmetatable(L, LND_TREE_METATABLE_WITH_ADDRESS);
   LUAaddMethod(L, "__newindex", &LNDsetChildWithAddress);   // blah[x] = y  or blah.x = y  blah:key(i)  then it will called with 'key'
   LUAaddMethod(L, "__index", &LNDgetChildWithAddress);
   LUAaddMethod(L, "__len", &LNDgetSizeWithAddress);
   LUAaddMethod(L, "__gc", &LNDdeleteWithAddress);
   LUAaddMethod(L, "__tostring", &LNDasStringWithAddress);
   LUAaddMethod(L, "__concat", &LNDconcatWithAddress);
   lua_pop(L,1);
}

#define SIMPLE_NODE_CACHE     "NODE_OBJECT_CACHE_SIMPLE"
#define ADDRESSED_NODE_CACHE  "NODE_OBJECT_CACHE_WITH_ADDRESS"
static void LNDpushNodeCache(lua_State *L, const char* pCacheKey){
   COL_FUNCTION(LNDpushNodeCache);
   lua_getfield(L, LUA_REGISTRYINDEX, pCacheKey);
   if(!lua_istable(L,-1)) {  // Need to create the cache with weak values.
      COL_TRC("Creating cache with weak values.");
      lua_pop(L, 1);
      lua_newtable(L);           // The node cache.
      lua_createtable(L, 0, 1);  // Its metatable.
      lua_pushliteral(L, "v"); lua_setfield(L, -2, "__mode");
      lua_setmetatable(L, -2);
      lua_pushvalue(L, -1);  // Duplicate the node cache.
      lua_setfield(L, LUA_REGISTRYINDEX, pCacheKey);
   }
}
static void LNDgetFromCache(lua_State *L, int CacheIndex, NODplace* pNode){
   COL_FUNCTION(LNDgetFromCache);
   COL_VAR(CacheIndex);
   if(CacheIndex < 0){
      CacheIndex += lua_gettop(L) + 1;
   }
   lua_pushlightuserdata(L, pNode);
   lua_gettable(L, CacheIndex);
}
static void addToCache(lua_State* L, int CacheIndex, NODplace* pNode)
{
   if(CacheIndex < 0) CacheIndex += lua_gettop(L) + 1;
   lua_pushlightuserdata(L, pNode);
   lua_insert(L, -2);  // Before the real object.
   lua_settable(L, CacheIndex);
}

void LNDpushNodeWithoutCallback(lua_State* L, NODplace* pNode){
   LUApushObject< COLref<NODplace> >(L, pNode, LND_TREE_METATABLE);
}

void LNDpushNodeWithCallback(lua_State* L, NODplace* pNode){
   COL_FUNCTION(LNDpushNodeWithCallback);
   LNDplaceAddress *pAddress =
      LUApushObject<LNDplaceAddress>(L, LND_TREE_METATABLE_WITH_ADDRESS);
   pAddress->m_pPlace = pNode;
   pAddress->m_Address.setParent(pNode);
}

void LNDpushNode(lua_State* L, NODplace* pNode){
   COL_FUNCTION(LNDpushNode);
   bool UseCallback = LNDnodDebug(L);
   LNDpushNodeCache(L, UseCallback ? ADDRESSED_NODE_CACHE : SIMPLE_NODE_CACHE);
   LNDgetFromCache(L, -1, pNode);
   if (lua_isnil(L,-1)) {
      lua_pop(L, 1);
      if (UseCallback) LNDpushNodeWithCallback(L, pNode);
      else             LNDpushNodeWithoutCallback(L, pNode);
      lua_pushvalue(L, -1);  // Make a copy for the cache.
      addToCache(L, -3, pNode);
   }
   lua_remove(L, -2);  // Drop the cache.
}

void LNDpushNodeWithAddress(lua_State* L, NODplace* pNode, const NODaddress& Address){
   LNDplaceAddress* pAddress;
   LNDpushNodeCache(L, ADDRESSED_NODE_CACHE);
   LNDgetFromCache(L, -1, pNode);
   if (!lua_isnil(L,-1)) {
      pAddress = LNDgetArgumentTreeWithAddress(L, -1);
   } else {
      lua_pop(L, 1);
      pAddress = LUApushObject<LNDplaceAddress>
         (L, LND_TREE_METATABLE_WITH_ADDRESS);
      pAddress->m_pPlace = pNode;
      lua_pushvalue(L, -1);  // Make a copy for the cache.
      addToCache(L, -3, pNode);
   }
   pAddress->m_Address = Address;  // The address may change.
   lua_remove(L, -2);  // Drop the cache.
}

NODplace* LNDgetNode(lua_State* L, int i) {
   COL_FUNCTION(LNDgetNode);
   if (lua_isuserdata(L, i)) {
      COL_VAR(LNDnodDebug(L));
      if (LNDnodDebug(L)) {
         LNDplaceAddress* pPlaceAddressData = LNDgetArgumentTreeWithAddress(L, i);
         if (pPlaceAddressData) {
            COL_VAR(pPlaceAddressData->m_pPlace.get());
            return pPlaceAddressData->m_pPlace.get();
         }
      } else {
         NODplace* pNodeData = LNDgetArgumentTree(L, i);
         if (pNodeData) {
            COL_VAR(pNodeData);
            return pNodeData;
         }
      }
   }
   return NULL;
}

const NODaddress& LNDgetNodeAddress(lua_State* L, int i){
   LNDplaceAddress* pPlaceAddress = LNDgetArgumentTreeWithAddress(L, i);
   return pPlaceAddress->m_Address;
}
