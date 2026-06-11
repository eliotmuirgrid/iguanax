//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtraverser
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, April 26th, 2011 @ 01:08:07 PM
//---------------------------------------------------------------------------
#include <TRN/TRNtraverser.h>
#include <TRN/TRNtraverserNode.h>
#include <TRN/TRNtraverserLua.h>
#include <TRN/TRNhtmlEscape.h>

#include <LND/LNDutils.h>

#include <LUA/LUAescape.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

const COLstring TRN_ROOT_KEY("r");
const COLstring TRN_STRING_KEY_PREFIX("s_");
const COLstring TRN_NUMBER_KEY_PREFIX("n_");
const COLstring TRN_BOOL_KEY_PREFIX  ("b_");

const COLstring TRN_NODE_TRAVERSER_TYPE("NODE");
const COLstring TRN_LUA_TRAVERSER_TYPE("LUA");

// We don't do "pretty escaping" for search matches because the highlighted
// escaped characters could distract from the search match highlighting.
COLstring TRNsearchMatchEscaper(const COLstring& UnescapedString){
   COLstring LuaEscaped = LUAstringEscape(UnescapedString.c_str(), UnescapedString.size());
   COLstring LuaAndHtmlEscaped;
   TRNhtmlEscape(LuaEscaped, LuaAndHtmlEscaped);
   return LuaAndHtmlEscaped;
}

TRNtraverser::TRNtraverser(){
   //COL_METHOD(TRNtraverser::TRNtraverser);
}

TRNtraverser::~TRNtraverser(){
  // COL_METHOD(TRNtraverser::~TRNtraverser);
}

TRNtraverser* TRNcreateTraverser(lua_State* L, int ItemIndex, bool ForDisplay, char IndexChar){
   COL_FUNCTION(TRNcreateTraverser);
   COL_VAR3(ItemIndex, ForDisplay, IndexChar);
   if (LNDisNode(L, ItemIndex)){
      return new TRNtraverserNode(*LNDgetNode(L, ItemIndex));
   } else {
      return new TRNtraverserLua(L, ItemIndex, ForDisplay, IndexChar);
   }
}

COLstring TRNsearchResult(TRNtraverser::eSearchResult Result){
   switch (Result){
      case TRNtraverser::NO_MATCH:     return "NO_MATCH";
      case TRNtraverser::STRING_MATCH: return "STRING_MATCH";
      case TRNtraverser::INDEX_MATCH:  return "INDEX_MATCH";
      default: return "UNKNOWN";
   } 
}