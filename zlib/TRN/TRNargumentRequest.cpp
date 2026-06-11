//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentRequest
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//---------------------------------------------------------------------------
#include <TRN/TRNargumentRequest.h>
#include <TRN/TRNannotationState.h>
#include <TRN/TRNtraverser.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNtitleFromLongString.h>
#include <TRN/TRNrenderNodTree.h>
#include <TRN/TRNwriteStringVar.h>

#include <BRO/BROargumentPopulate.h>

#include <CHNK/CHNKviewString.h>
#include <CHNK/CHNKrenderChunks.h>
#include <CHNK/CHNKsplitIntoChunks.h>

#include <NOB/NOBcopy.h>
#include <NOB/NOBsimple.h>
#include <NOB/NOBcomplexLuaNode.h>
#include <LND/LNDutils.h>
#include <LUA/LUAutils.h>
#include <LUAL/LUALutils.h>
#include <SFI/SFIhtmlEscape.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNarg::TRNarg() : m_Call(-1), m_Line(-1), m_ProtocolType(-1), m_NodeType(-1), m_ViewMode(CHNK_VIEW_PLAIN_TEXT) {
   COL_METHOD(TRNarg::TRNarg); 
}

static void TRNgetString(lua_State* L, int i, COLstring& OutOriginalString, COLstring& OutTitle){
   OutOriginalString = LUAtoString(L, i);
   TRNtitleFromLongString(OutOriginalString, OutTitle);
}

static void TRNgetNumber(lua_State* L, int i, COLstring& OutValue, COLstring& OutTitle){
   COLostream Stream(OutValue);
   LUAformatNumber(L, i, Stream);
   TRNtitleFromLongString(OutValue, OutTitle);
}

static void TRNgetLightUserdata(lua_State* L, int i, COLstring& OutValue, COLstring& OutTitle){
   COL_FUNCTION(TRNgetLightUserdata);
   COLstring UserdataName = luaL_typename(L, i);
   SFIhtmlEscapeNoNewline(UserdataName.c_str(), UserdataName.size(), OutValue);
   TRNtitleFromLongString(UserdataName, OutTitle);
}

static void TRNluaTableToNobTreeImpl(TRNtraverser* pTraverser, NOBcomposite* pParent, NOBmemoizer* pMemoizer, int Depth){
   COL_FUNCTION(TRNluaTableToNobTreeImpl);
   COL_VAR(Depth);
   int ChildCount = 0;

   while (pTraverser->stepNext()){
      COLauto<NODplace> pChild;
      COLstring ChildKey;
      pTraverser->getTreeviewKey(ChildKey);
      int ProtocolType, NodeType;
      pTraverser->getTypeInfo(ProtocolType, NodeType);
      const NODplace* pNode = pTraverser->nodPlace();
      if (pNode){
         pChild = NOBcopy(*pNode, ChildKey.is_null() ? NULL : ChildKey.c_str());
      } else if (pTraverser->isLeaf() || Depth == TRN_MAX_RECURSION_LEVEL - 1){
         NOBsimple* pSimple = new NOBsimple(pMemoizer);
         pChild = pSimple;
         COLstring SimpleValue;
         pTraverser->getTreeviewValue(SimpleValue);
         pSimple->setCachedValue(SimpleValue);
         pSimple->setName(ChildKey);
         pSimple->setProtocolType(ProtocolType);
         pSimple->setNodeType(NodeType);
      } else {
         COLstring ToString;
         NOBcomposite* pComposite = NULL;
         if (pTraverser->isComplexLuaItem()){
            COLstring ShortLabel, LongLabel;
            pTraverser->getTreeviewText(ShortLabel, LongLabel);
            pComposite = new NOBcomplexLuaNode(pMemoizer, ShortLabel, LongLabel);
         } else {
            // Not a complex Lua item.  We can reproduce treeview labels on the fly,
            // since no metamethods are required to generate them.
            pComposite = new NOBcomposite(pMemoizer);
         }
         pChild = pComposite;
         pComposite->setName(ChildKey);
         pComposite->setProtocolType(ProtocolType);
         pComposite->setNodeType(NodeType);
         COLauto<TRNtraverser> pChildTraverser = pTraverser->traverser();
         TRNluaTableToNobTreeImpl(pChildTraverser.get(), pComposite, pMemoizer, Depth+1);
      }
      pParent->addChild(pChild.release());
   }
}

// Note that nodeTypeName() and protocolName() should not be called
// on the resulting NOB tree, since we won't bother to populate the
// NOBmemoizer with the string representations of these types.
static NODplace* TRNluaTableToNobTree(lua_State* L, int i){
   COL_FUNCTION(TRNluaTableToNobTree);
   COLauto<TRNtraverser> pTraverser = TRNcreateTraverser(L, i, true, 0);
   COLauto<NOBcomposite> pRoot;

   if (pTraverser->isComplexLuaItem()){
      COLstring ShortLabel, LongLabel;
      pTraverser->getTreeviewText(ShortLabel, LongLabel);
      pRoot = new NOBcomplexLuaNode(new NOBmemoizer(), ShortLabel, LongLabel);
   } else {
      // Not a complex Lua item.  We can reproduce treeview labels on the fly,
      // since no metamethods are required to generate them.
      pRoot = new NOBcomposite(new NOBmemoizer());
   }
   COLstring RootKey;
   pTraverser->getTreeviewKey(RootKey);
   pRoot->setName(RootKey);
   int ProtocolType, NodeType;
   pTraverser->getTypeInfo(ProtocolType, NodeType);
   pRoot->setNodeType(NodeType);
   pRoot->setProtocolType(ProtocolType);
   TRNluaTableToNobTreeImpl(pTraverser.get(), pRoot.get(), pRoot->memoizer(), 0);
   return pRoot.release();
}

TRNargRequest::TRNargRequest()
 : m_IsReturn(false),
   m_ArgumentIndex(0),
   m_ViewMode(CHNK_VIEW_PLAIN_TEXT),
   m_FirstChunk(0),
   m_SecondChunk(0) {
}

void TRNargumentReached(TRNargRequest& ArgRequest, lua_State* L, int i, const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line, COLhashmap<COLstring, TRNarg>& ArgCache){
   COL_FUNCTION(TRNargumentReached);
   if (i > lua_gettop(L)) { COL_TRC("Argument index larger than lua stack top - early return"); return; }
   TRNarg& Arg = ArgCache[ArgRequest.m_Id];
   TRNargumentRequestPopulate(ArgRequest, L, i, FileName, FunctionName, Call, Line, Arg);
   #ifdef BRO_ENABLED
   BROargumentPopulate(L, i, &ArgRequest.m_BroResult);  // CONNECTOR BRO code.
   #endif
   if (!TRNargIsTree(Arg) && Arg.m_NodeType != LUA_TSTRING){
      COL_TRC("Removing arg request from cache - not a tree or string so we don't need to get more data");
      // Remove the item from the cache, since it's not a string or a tree, and therefore
      // we will have no need to retrieve more chunks or change the view mode.
      ArgCache.removeKey(ArgRequest.m_Id);
   }
}

void TRNargumentRequestPopulate(TRNargRequest& ArgRequest, lua_State* L, int i, const COLstring& FileName, const COLstring& FunctionName, const int Call, const int Line, TRNarg& Arg){
   COL_FUNCTION(TRNargumentRequestPopulate);
   Arg.m_Call = Call;
   Arg.m_Line = Line;
   Arg.m_ViewMode = ArgRequest.m_ViewMode;
   Arg.m_FunctionName.clear();
   Arg.m_FileName.clear();
   Arg.m_Title.clear();
   SFIhtmlEscape(FunctionName.c_str(), FunctionName.size(), Arg.m_FunctionName);
   SFIhtmlEscape(FileName.c_str(), FileName.size(), Arg.m_FileName);
   switch (lua_type(L, i)){
      case LUA_TSTRING:        TRNgetString(L, i, Arg.m_OriginalString, Arg.m_Title); CHNKrenderViewString(Arg.m_OriginalString, Arg.m_ViewMode, &Arg.m_ViewString); break;
      case LUA_TNIL:           Arg.m_ViewString = "nil";                                                        break;
      case LUA_TBOOLEAN:       Arg.m_ViewString = lua_toboolean(L, i) ? "true":"false";                         break;
      case LUA_TFUNCTION:      Arg.m_ViewString = "function";                                                   break;
      case LUA_TNUMBER:        TRNgetNumber(L, i, Arg.m_ViewString, Arg.m_Title);                               break;
      case LUA_TLIGHTUSERDATA: TRNgetLightUserdata(L, i, Arg.m_ViewString, Arg.m_Title);                        break;
      case LUA_TTABLE:
      case LUA_TUSERDATA:
         if (LNDisNode(L, i)){
            NODplace* pPlace = LNDgetNode(L, i);
            COLPRECONDITION(pPlace);
            TRNrenderNodTree(*pPlace, Arg, ArgRequest.m_RootAddressJson, ArgRequest.m_ExpansionTreeJson, &ArgRequest.m_VarResult);
            Arg.m_pTree = NOBcopy(*pPlace); // Store copied tree in case we need it later.
         } else {
            COL_TRC("Ordinary Lua table");
            Arg.m_pTree = TRNluaTableToNobTree(L, i);
            TRNrenderNodTree(*Arg.m_pTree, Arg, ArgRequest.m_RootAddressJson, ArgRequest.m_ExpansionTreeJson, &ArgRequest.m_VarResult);
         }
         break;
      default: COLPRECONDITION(1==0);
   }

   if (TRNargIsTree(Arg)){
      COL_TRC("Tree arg.");
      // We should have already written out the JSON
   } else {
      COL_TRC("Scalar arg.");
      CHNKsplitIntoChunks(Arg.m_ViewString, &Arg.m_Chunks);
      COLarray<CHNKrenderedChunk> RenderedChunks;
      CHNKrenderChunks(Arg.m_Chunks, ArgRequest.m_FirstChunk, ArgRequest.m_SecondChunk, RenderedChunks);
      Arg.m_ProtocolType = LUAL_LUA_PROTOCOL;
      Arg.m_NodeType = lua_type(L, i);
      TRNwriteStringVar(RenderedChunks, Arg, &ArgRequest.m_VarResult);
   }
   COL_VAR(ArgRequest.m_VarResult.json());

}

void TRNargumentRequestSetViewMode(TRNargRequest& Arg, const COLstring& ViewModeString){
   COL_FUNCTION(TRNargumentRequestSetViewMode);
   if (ViewModeString == "string_et"){
      Arg.m_ViewMode = CHNK_VIEW_ESCAPED_TEXT;
   } else if (ViewModeString == "string_hx"){
      Arg.m_ViewMode = CHNK_VIEW_HEX;
   }
   // else, keep it as default (CHNK_VIEW_PLAIN_TEXT).
}

void TRNargumentRequestSetMembersFromId(TRNargRequest& Arg){
   COL_FUNCTION(TRNargumentRequestSetMembersFromId);
   COLstring Remainder;
   Arg.m_Id.split(Arg.m_FunctionAddress, Remainder, "-");
   Arg.m_ArgumentIndex = atoi(Remainder.c_str());
   Arg.m_IsReturn = Arg.m_Id[Arg.m_Id.size()-1] == 'r';
   COL_VAR3(Arg.m_FunctionAddress, Arg.m_ArgumentIndex, Arg.m_IsReturn);
}

void TRNargumentRequestClear(TRNarg* pArg){
   COL_FUNCTION(TRNargumentRequestClear);
   pArg->m_OriginalString.clear();
   pArg->m_ViewString.clear();
   pArg->m_Chunks.clear();
   pArg->m_Title.clear();
   pArg->m_FileName.clear();
   pArg->m_FunctionName.clear();
   pArg->m_Call = -1;
   pArg->m_Line = -1;
   pArg->m_ProtocolType = -1;
   pArg->m_NodeType = -1;
   pArg->m_ViewMode = CHNK_VIEW_PLAIN_TEXT;
}

int TRNargLinesInLastChunk(const TRNarg& Arg){
   return Arg.m_Chunks.size() ? Arg.m_Chunks[Arg.m_Chunks.size()-1].m_Lines.size() : 0;
}

bool TRNargIsTree(const TRNarg& Arg){
   return Arg.m_pTree.get() != NULL;
}

COLostream& operator<<(COLostream& Stream, TRNargRequest& ArgRequest){
   Stream << "Request id: " << ArgRequest.m_Id << newline;
   Stream << "view mode (1-plain, 2-escaped, 3-hex): " << ArgRequest.m_ViewMode << newline;
   Stream << "response: " << ArgRequest.m_VarResult.json(1) << newline;
   return Stream;  
}
