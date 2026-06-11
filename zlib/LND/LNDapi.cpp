//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LNDapi
//
// Description:
//
// Lua Node library functions
//
// Author: Nicolas Lehman
// Date:   Thu Jun 3 2021
//---------------------------------------------------------------------------

#include "LNDapi.h"
#include "LNDutils.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <LUA/LUAheader.h>
#include <LUA/LUAutils.h>
#include <LUA/LUAerror.h>
#include <LUA/LUAcheck.h>
#include <LUAL/LUALutils.h>

#include <LSTR/LSTRstring.h>

#include <NOD/NODplace.h>

#include <NOX/NOXconstant.h>
#include <NOD/NODoperation.h>
#include <NTAB/NTABtableItem.h>

static void LNDvalidateNodeType(lua_State* L, int Protocol, const char* pString){
   COL_FUNCTION(LNDvalidateNodeType);
   switch (Protocol){
      case NOX_XML:
         if (pString != NOXelementName &&
             pString != NOXattributeName &&
             pString != NOXtextName &&
             pString != NOXcdataName){
               LUA_ERROR_STREAM("Type must be xml.ELEMENT, xml.ATTRIBUTE, xml.TEXT or xml.CDATA.");
         }
         break;
      default:
         COLASSERT(false);
   }
}

static int LNDgetProtocolFromNodeArgument(lua_State* L){
   COL_FUNCTION(LNDgetProtocolFromNodeArgument);
   if (lua_gettop(L) > 0){
      NODplace* pNode = LNDgetNode(L, 1);
      if (pNode){
         return pNode->protocolType();
      }
   }
   return -1;
}

static int LNDgetChildIndex(const NODplace& Parent, const NODplace& Child){
   COL_FUNCTION(LNDgetChildIndex);
   int CountOfChild = Parent.size();
   for (int i = 0; i < CountOfChild; i++){
      if (&Parent.child(i) == &Child){
         return i;
      }
   }
   COLASSERT(false);
   return -1; // means unknown/no protocol
}

static int LNDgetType(lua_State* L) {
   COL_FUNCTION(LNDgetType);
   LUAcheckArgCount(L, 1, -1);
   const NODplace* pNodeData = LNDgetNode(L, 1);
   if (!pNodeData){
      luaL_error(L, "Not a node.");
   }
   LUApushString(L, pNodeData->nodeTypeName());
   LUApushString(L, pNodeData->protocolName());
   return 2; // number of results
}

static int LNDgetName(lua_State* L){
   COL_FUNCTION(LNDgetName);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");

   try{
      LUApushString(L, pNode->name());
      return 1;
   } LUA_CATCH();
}

static int LNDnodeToString(lua_State* L) {
   COL_FUNCTION(LNDnodeToString);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }

   COLstring StringValue;
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }

   LUApushString(L, StringValue);
   return 1;
}

static int LNDnodeSub(lua_State* L){
   COL_FUNCTION(LNDnodeSub);
   int ArgCount = LUAcheckArgCount(L, 2, 3);
   COLstring StringValue;
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_insert(L, 1);
   if (ArgCount == 3){ lua_insert(L, 1); }
   lua_pop(L, 1);
   try {
      lua_getglobal(L, "string");
      lua_insert(L, 1);
      lua_getfield(L, 1, "sub");
      lua_insert(L, 2);
      LUApushString(L, StringValue);
      lua_insert(L, 3);
      lua_call(L, ArgCount, 1);
      return 1;
   }  LUA_CATCH();
}

static int LNDnodeGsub(lua_State* L){
   COL_FUNCTION(LNDnodeGsub);
   int ArgCount = LUAcheckArgCount(L, 3, 4);
   COLstring StringValue;
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   for (int i = 0; i < ArgCount-1; i++){
      lua_insert(L, 1);
   }
   lua_pop(L, 1);
   try {
      lua_getglobal(L, "string");
      lua_insert(L, 1);
      lua_getfield(L, 1, "gsub");
      lua_insert(L, 2);
      LUApushString(L, StringValue);
      lua_insert(L, 3);
      lua_call(L, ArgCount, 2);
      return 2;
   }  LUA_CATCH();
}

static int LNDnodeSplit(lua_State* L){
   COL_FUNCTION(LNDnodeSplit);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   COLstring StringNode;
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringNode)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_insert(L, 1);
   lua_pop(L, 1);
   try {
      lua_getglobal(L, "string");
      lua_getfield(L, 2, "split");
      LUApushString(L, StringNode);
      lua_pushvalue(L, 1);
      lua_call(L, 2, 1);
      return 1;
   }  LUA_CATCH();
}

static int LNDnodeUpper(lua_State* L){
   COL_FUNCTION(LNDnodeUpper);
   COLstring StringValue;
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_pop(L, 1); //Popping original node
   try {
      lua_getglobal(L, "string");
      lua_getfield(L, 1, "upper");
      LUApushString(L, StringValue);
      lua_call(L, 1, 1);
      return 1;
   }  LUA_CATCH();
}

static int LNDnodeLower(lua_State* L){
   COL_FUNCTION(LNDnodeLower);
   COLstring StringValue;
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_pop(L, 1); //Popping original node
   try {
      lua_getglobal(L, "string");
      lua_getfield(L, 1, "lower");
      LUApushString(L, StringValue);
      lua_call(L, 1, 1);
      return 1;
   }  LUA_CATCH();
}

static int LNDnodeRxmatch(lua_State* L){
   COL_FUNCTION(LNDnodeRxmatch);
   NODplace* pNode = LNDgetNode(L, 1);
   COLstring StringNode;
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringNode)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   LUApushString(L, StringNode);
   lua_replace(L, 1);
   try {
      return LSTRrxmatch(L);
   } LUA_CATCH();
}

static int LNDnodeRxsub(lua_State* L){
   COL_FUNCTION(LNDnodeRxsub);
   NODplace* pNode = LNDgetNode(L, 1);
   COLstring StringNode;
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringNode)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   LUApushString(L, StringNode);
   lua_replace(L, 1);
   try {
      return LSTRrxsub(L);
   } LUA_CATCH();
}

static int LNDnodeReverse(lua_State* L){
   COL_FUNCTION(LNDnodeReverse);
   COLstring StringValue;
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringValue)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_pop(L, 1); //Popping original node
   try {
      lua_getglobal(L, "string");
      lua_getfield(L, 1, "reverse");
      LUApushString(L, StringValue);
      lua_call(L, 1, 1);
      return 1;
   }  LUA_CATCH();
}

static int LNDnodeFind(lua_State* L){
   COL_FUNCTION(LNDnodeFind);
   int CountOfArgument = LUAcheckArgCount(L, 2, 4);
   NODplace* pNode = LNDgetNode(L, 1);
   COLstring StringNode;
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringNode)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   for (int i = 0; i < CountOfArgument - 1; i++){
      lua_insert(L, 1);
   }
   lua_pop(L, 1);

   try {
      lua_getglobal(L, "string");
      lua_insert(L, 1);
      lua_getfield(L, 1, "find");
      lua_insert(L, 2);
      LUApushString(L, StringNode);
      lua_insert(L, 3);
      lua_call(L, CountOfArgument, 2);
      return 2;
   } LUA_CATCH();
}

static int LNDnodeGfind(lua_State* L){
   COL_FUNCTION(LNDnodeGfind);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   COLstring StringNode;
   if (!pNode) { luaL_error(L, "Not a node."); return 0; }
   if (!LUAextractString(L, 1, &StringNode)) {
      luaL_error(L, "Node cannot be converted to string.");
   }
   lua_insert(L, 1);
   lua_pop(L, 1);
   try {
      lua_getglobal(L, "string");
      lua_insert(L, 1);
      lua_getfield(L, 1, "gfind");
      lua_insert(L, 2);
      LUApushString(L, StringNode);
      lua_insert(L, 3);
      lua_call(L, 2, 1);
      return 1;
   } LUA_CATCH();
}

static int LNDnodeCapitalize(lua_State* L){
   COL_FUNCTION(LNDnodeCapitalize);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }
   try{
      return LSTRstringCapitalize(L);
   } LUA_CATCH();
}

static int LNDnodeTrimWS(lua_State* L){
   COL_FUNCTION(LNDnodeTrimWS);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }
   try {
      return LSTRstringTrimWS(L);
   } LUA_CATCH();
}

static int LNDnodeTrimRWS(lua_State* L){
   COL_FUNCTION(LNDnodeTrimRWS);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }
   try{
      return LSTRstringTrimRWS(L);
   } LUA_CATCH();
}

static int LNDnodeTrimLWS(lua_State* L){
   COL_FUNCTION(LNDnodeTrimLWS);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }
   try{
      return LSTRstringTrimLWS(L);
   } LUA_CATCH();
}

static int LNDnodeCompactWS(lua_State* L){
   COL_FUNCTION(LNDnodeCompactWS);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) {
      luaL_error(L, "Not a node.");
   }
   try {
      return LSTRstringCompactWS(L);
   }  LUA_CATCH();
}

static int LNDgetValue(lua_State* L){
   COL_FUNCTION(LNDgetValue);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");
   else if (!pNode->isSimple()){
      LUA_ERROR_STREAM(COL_T("Branch nodes do not have values."));
   }

   try{
      LUApushString(L, pNode->value());
      return 1;
   } LUA_CATCH();
}

static int LNDnodeRemove(lua_State* L){
   COL_FUNCTION(LNDnodeRemove);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");
   int RemoveIndex = 0;
   try{
      switch(lua_type(L, 2)){
      case LUA_TNUMBER:{
            RemoveIndex = lua_tointeger(L, 2);
            pNode->remove(RemoveIndex-1);
            return 0;
         }
      case LUA_TSTRING:{
            pNode->remove(LUAtoString(L, 2));
            return 0;
         }
      default:
         luaL_error(L, "Index must be a name (a string) or a number.");
         return 0;
     }
   } catch (const COLerror& Error){
      if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS){
         luaL_error(L, "Index %d is out of bounds.", RemoveIndex);
      } else {
         LUA_ERROR_STREAM(Error.description());
      }
      return 0;
   }
}

static int LNDcountOfChild(lua_State* L){
   COL_FUNCTION(LNDcountOfChild);
   int CountOfArgument = LUAcheckArgCount(L, 1, 2);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");
   if (CountOfArgument == 2 && !lua_isstring(L, 2)){
      luaL_error(L, "Name must be a string.");
   }

   try{
      if (CountOfArgument == 2){
         COLstring ChildName = LUAtoString(L, 2);
         lua_pushinteger(L, NODcountOfNamedChildRepeat(*pNode, ChildName));
      } else {
         lua_pushinteger(L, pNode->size());
      }
      return 1;
   } LUA_CATCH();
}

static int LNDgetNamedChild(lua_State* L){
   COL_FUNCTION(LNDgetNamedChild);
   int CountOfArgument = LUAcheckArgCount(L, 2, 3);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");
   if (!lua_isstring(L,2)) luaL_error(L, "Name must be a string.");
   if (CountOfArgument == 3 && !lua_isnumber(L, 3)){
      luaL_error(L, "Repeat must be a number.");
   }

   COLstring ChildName = LUAtoString(L, 2);
   int RepeatIndex = (CountOfArgument == 3 ? lua_tointeger(L, 3) : 1);
   try{
      int ChildIndex = NODgetNamedChildRepeat(*pNode, ChildName, RepeatIndex-1);
      NODplace& Child = pNode->child(ChildIndex);
      if (LNDisNodeWithAddress(L, 1)){
         NODaddress ChildAddress(LNDgetNodeAddress(L, 1));
         ChildAddress.pushAddressIndex(ChildIndex+1);
         LNDpushNodeWithAddress(L, &Child, ChildAddress);
      } else {
         LNDpushNode(L, &Child);
      }
      return 1;
   } LUA_CATCH_INDEX(RepeatIndex);
}

static int LNDcopyAllFields(lua_State* L){
   COL_FUNCTION(LNDcopyAllFields);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pDestination  = LNDgetNode(L, 1);
   const NODplace* pSource = LNDgetNode(L, 2);
   if (!pDestination || !pSource){
      luaL_error(L, "Source and destination must be nodes.");
   }

   try{
      NODmapTree(*pSource, *pDestination);
      return 0; // number of results
   }
   LUA_CATCH();
}

static int LNDcopyFieldRange(lua_State* L){
   COL_FUNCTION(LNDcopyFieldRange);
   int CountOfArgument = LUAcheckArgCount(L, 3, 4);
   NODplace* pDestination  = LNDgetNode(L, 1);
   const NODplace* pSource = LNDgetNode(L, 2);
   if (!pDestination || !pSource){
      luaL_error(L, "Source and destination must be nodes.");
   }

   int Start = luaL_checkint(L, 3) - 1;
   int End = (CountOfArgument > 3 ? luaL_checkint(L, 4) : -1);

   try{
      NODmapRange(*pSource, *pDestination, Start, End);
      return 0; // number of results
   }
   LUA_CATCH();
}

static int LNDnodeText(lua_State* L){
   COL_FUNCTION(LNDnodeText);
   LUAcheckArgCount(L, 1, -1);
   const NODplace* pNodeData = LNDgetNode(L, 1);
   COLstring FullString;
   int CountOfMember = pNodeData->size();
   for (int i = 0; i < CountOfMember; i++){
      const NODplace& Child = pNodeData->child(i);
      if (Child.name() == "Text" && Child.grammarType() == NOD_SIMPLE){
         FullString += Child.value();
      }
   }
   COL_VAR(FullString);
   try{
      LUApushString(L, FullString);
      return 1; //number of results
   } LUA_CATCH();

   //These lines seem redundant
   //This error should really never be reached.
   //TNDL_LUA_ERROR_STREAM(COL_T("Could not get the text of that node."));
   //return 0;
}

static int LNDisNullValue(lua_State* L){
   COL_FUNCTION(LNDisNullValue);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");
   try{
      lua_pushboolean(L, pNode->isNull());
      return 1;
   } LUA_CATCH();
}

static int LNDisLeaf(lua_State* L){
   COL_FUNCTION(LNDisLeaf);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode) luaL_error(L, "Not a node.");

   try{
      lua_pushboolean(L, pNode->isSimple());
      return 1;
   } LUA_CATCH();
}

static int LNDsetValue(lua_State* L) {
   COL_FUNCTION(LNDsetValue);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pNode = LNDgetNode(L, 1);

   if (!pNode) luaL_error(L, "Not a node.");
   if (pNode->isReadOnly()) luaL_error(L, "Cannot set the value of a read-only node.");
   if (!lua_isstring(L,2)) luaL_error(L, "Value must be a string.");

   try {
      pNode->setValue(LUAtoString(L, 2));
      LNDpushNode(L, pNode);
      return 1;  // return this node object back to script to support chaining methods
   } LUA_CATCH();
}

static int LNDnodeSetInnerXml(lua_State* L){
   COL_FUNCTION(LNDnodeSetInnerXml);
   LUAcheckArgCount(L, 2, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode)             luaL_error(L, "Not a node.");
   if (!lua_isstring(L,2)) luaL_error(L, "Content must be a string.");

   if (pNode->protocolType() != NOX_XML){
      LUA_ERROR_STREAM(COL_T("setInnerXml is only defined for XML nodes."));
   }

   try {
      if (!pNode->setTreeValue(LUAtoString(L, 2))){
         LUA_ERROR_STREAM(COL_T("setInnerXml is not available for this type of node."));
      }
      lua_settop(L, 1);  // We return the node we were called with.
      return 1;
   } LUA_CATCH();
}

static int LNDnodeAppendChild(lua_State* L){
   COL_FUNCTION(LNDnodeAppendChild);
   // Get the protocol, if a node was supplied at all.
   int Protocol = LNDgetProtocolFromNodeArgument(L);
   int CountOfArgument = LUAcheckArgCount(L, 3, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode)             luaL_error(L, "Not a node.");
   if (!lua_isstring(L,2)) luaL_error(L, "Type must be a string.");
   if (!lua_isstring(L,3)) luaL_error(L, "Value must be a string.");

   if (pNode->protocolType() != NOX_XML || pNode->nodeType() != NOX_ELEMENT){
      LUA_ERROR_STREAM(COL_T("appendChild is only defined for XML elements."));
   }

   const char* pString1 = (CountOfArgument > 1 ? lua_tostring(L, 2) : NULL);
   const char* pString2 = (CountOfArgument == 3 ? lua_tostring(L, 3) : NULL);

   LNDvalidateNodeType(L, Protocol, pString1);

   try{
      NODplace& Child = pNode->add(pString1, pString2);
      if (LNDisNodeWithAddress(L, 1)){
         NODaddress ChildAddress(LNDgetNodeAddress(L, 1));
         ChildAddress.pushAddressIndex(LNDgetChildIndex(*pNode, Child)+1);
         LNDpushNodeWithAddress(L, &Child, ChildAddress);
      } else {
         LNDpushNode(L, &Child);
      }
      return 1;
   } catch (const COLerror& Error){
      if (Error.code() == NOD_BAD_ARGUMENTS){
         luaL_error(L, "Bad arguments: %s", Error.description().c_str());
      } else {
         LUA_ERROR_STREAM(Error.description());
      }
      return 0;
   }
}

static int LNDnodeInsertChild(lua_State* L){
   COL_FUNCTION(LNDnodeInsertChild);
   // Get the protocol, if a node was supplied at all.
   int Protocol = LNDgetProtocolFromNodeArgument(L);
   int CountOfArgument = LUAcheckArgCount(L, 4, -1);
   NODplace* pNode = LNDgetNode(L, 1);
   if (!pNode)             luaL_error(L, "Not a node.");
   if (!lua_isnumber(L,2)) luaL_error(L, "Index must be a number.");
   if (!lua_isstring(L,3)) luaL_error(L, "Type must be a string.");
   if (!lua_isstring(L,4)) luaL_error(L, "Value must be a string.");

   if (pNode->protocolType() != NOX_XML || pNode->nodeType() != NOX_ELEMENT){
      LUA_ERROR_STREAM(COL_T("insertChild is only defined for XML elements."));
   }

   const int InsertIndex = lua_tointeger(L, 2);
   const char* pString1 = (CountOfArgument > 2 ? lua_tostring(L, 3) : NULL);
   const char* pString2 = (CountOfArgument == 4 ? lua_tostring(L, 4) : NULL);

   LNDvalidateNodeType(L, Protocol, pString1);

   try{
      NODplace& Child = pNode->insert(InsertIndex-1, pString1, pString2);
      if (LNDisNodeWithAddress(L, 1)){
         NODaddress ChildAddress(LNDgetNodeAddress(L, 1));
         ChildAddress.pushAddressIndex(InsertIndex);
         LNDpushNodeWithAddress(L, &Child, ChildAddress);
      } else {
         LNDpushNode(L, &Child);
      }
      return 1;
   } catch (const COLerror& Error){
      if (Error.code() == NOD_INDEX_OUT_OF_BOUNDS){
         luaL_error(L, "Index %d is out of bounds.", InsertIndex);
      } else if (Error.code() == NOD_BAD_ARGUMENTS){
         luaL_error(L, "Bad arguments: %s", Error.description().c_str());
      } else {
         LUA_ERROR_STREAM(Error.description());
      }
      return 0;
   }
}

static int LNDisPrimaryKey(lua_State* L){
   COL_FUNCTION(LNDisPrimaryKey);
   LUAcheckArgCount(L, 1, -1);
   NODplace* pSelf = LNDgetNode(L, 1);
   if (!pSelf) luaL_error(L, "Not a node.");
   NTABtableItem* pItem = dynamic_cast<NTABtableItem*>(pSelf);
   if (pItem){
      lua_pushboolean(L, pItem->isKey());
      return 1;
   } else {
      luaL_error(L, "isPrimaryKey() is only valid for table columns.");
      return 0; // to make compilers happy
   }
}

void LNDaddNodeNamespace(lua_State* L) {
   COL_FUNCTION(LNDaddNodeNamespace);

   lua_newtable(L); // node
   {
      LUAaddMethod(L, "nodeType",   &LNDgetType);
      LUAaddMethod(L, "nodeName",   &LNDgetName);
      LUAaddMethod(L, "S",          &LNDnodeToString);
      LUAaddMethod(L, "sub",        &LNDnodeSub);
      LUAaddMethod(L, "gsub",       &LNDnodeGsub);
      LUAaddMethod(L, "split",      &LNDnodeSplit);
      LUAaddMethod(L, "upper",      &LNDnodeUpper);
      LUAaddMethod(L, "lower",      &LNDnodeLower);
      LUAaddMethod(L, "rxmatch",    &LNDnodeRxmatch);
      LUAaddMethod(L, "rxsub",      &LNDnodeRxsub);
      LUAaddMethod(L, "reverse",    &LNDnodeReverse);
      LUAaddMethod(L, "find",       &LNDnodeFind);
      LUAaddMethod(L, "gfind",      &LNDnodeGfind);
      LUAaddMethod(L, "capitalize", &LNDnodeCapitalize);
      LUAaddMethod(L, "trimWS",     &LNDnodeTrimWS);
      LUAaddMethod(L, "trimRWS",    &LNDnodeTrimRWS);
      LUAaddMethod(L, "trimLWS",    &LNDnodeTrimLWS);
      LUAaddMethod(L, "compactWS",  &LNDnodeCompactWS);
      LUAaddMethod(L, "nodeValue",  &LNDgetValue);
      LUAaddMethod(L, "remove",     &LNDnodeRemove);
      LUAaddMethod(L, "childCount",     &LNDcountOfChild);
      LUAaddMethod(L, "child",          &LNDgetNamedChild); // IMPORTANT - if changed update the intellisense logic
      LUAaddMethod(L, "mapTree",        &LNDcopyAllFields);
      LUAaddMethod(L, "mapRange",       &LNDcopyFieldRange);
      LUAaddMethod(L, "nodeText",       &LNDnodeText);
      LUAaddMethod(L, "isNull",         &LNDisNullValue);
      LUAaddMethod(L, "isLeaf",         &LNDisLeaf);
      LUAaddMethod(L, "setNodeValue",   &LNDsetValue);
      // XML node methods
      LUAaddMethod(L, "setInner",       &LNDnodeSetInnerXml);
      LUAaddMethod(L, "append",         &LNDnodeAppendChild);
      LUAaddMethod(L, "insert",         &LNDnodeInsertChild);
      // Table item methods
      LUAaddMethod(L, "isKey",          &LNDisPrimaryKey);
      // Put a copy in the Lua registry, indexed by our special name.
      lua_pushvalue(L, -1);
      lua_setfield(L, LUA_REGISTRYINDEX, "__node");
   }
   lua_setglobal(L, "node");
}
