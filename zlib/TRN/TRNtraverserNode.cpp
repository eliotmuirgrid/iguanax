// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtraverserNode
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Thursday 04 May 2023 - 08:52AM
// ---------------------------------------------------------------------------

#include <TRN/TRNtraverserNode.h>
#include <TRN/TRNtreeFormatter.h>
#include <TRN/TRNconstant.h>
#include <TRN/TRNhtmlEscape.h>
#include <TRN/TRNhtmlEscapeString.h>
#include <TRN/TRNisValidLuaName.h>
#include <TRN/TRNwriteLuaStringKey.h>
#include <THTM/THTMprinter.h>

#include <NOD/NODprintCallback.h>

#include <NTV/NTVtreeValue.h>

#include <SFI/SFIhtmlCrop.h>

#include <REX/REXmatcher.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

TRNtraverserNode::TRNtraverserNode(const NODplace& Place)
   : m_pParent(&Place),
   m_pChild(0),
   m_Index(-1),
   m_RepeatIndex(-1)
{
   COL_METHOD(TRNtraverserNode::TRNtraverserNode);
   m_pFormatter = TRNgetFormatter(*m_pParent);
   m_pFormatter->pushNode(*m_pParent, -1);
}

TRNtraverserNode::TRNtraverserNode(const NODaddress& RootAddress)
   : m_pParent(RootAddress.parent()),
   m_pChild(0),
   m_Index(-1),
   m_RepeatIndex(-1)
{
   COL_METHOD(TRNtraverserNode::TRNtraverserNode);
   COLPRECONDITION(m_pParent);
   m_pFormatter = TRNgetFormatter(*m_pParent);
   m_pFormatter->pushNode(*m_pParent, -1);
   int TreeRootDepth = RootAddress.depth();
   for (int Level = 0; Level < TreeRootDepth; Level++){
      int IntegerIndex = RootAddress.addressIndex(Level)-1;
      m_pParent = &m_pParent->child(IntegerIndex);
      m_pFormatter->pushNode(*m_pParent, IntegerIndex);
   }
}

TRNtraverserNode::TRNtraverserNode(const NODplace& Place, TRNtreeFormatter* pFormatter)
   : m_pParent(&Place),
   m_pChild(0),
   m_Index(-1),
   m_RepeatIndex(-1)
{
   COL_METHOD(TRNtraverserNode::TRNtraverserNode);
   m_pFormatter = pFormatter;
   m_pFormatter.setIsOwner(false);
}

TRNtraverserNode::~TRNtraverserNode() {
   COL_METHOD(TRNtraverserNode::~TRNtraverserNode);
}

// Returns the number of nodes with this name found so far, and increases this number.
static int TRNgetRepeatIndex(COLhashmap<COLstring, int>& RepeatsByName, const COLstring& Name){
   COLhashmapPlace NamePlace = RepeatsByName.find(Name);
   if (!NamePlace){
      RepeatsByName.add(Name, 1);
      return 0;
   } else {
      int RepeatIndex = RepeatsByName[NamePlace];
      RepeatsByName[NamePlace] = RepeatIndex + 1;
      return RepeatIndex;
   }
}

bool TRNtraverserNode::stepNext(){
   if (m_pParent->isSimple()){
      return false;
   } else if (m_pChild){
      m_pFormatter->popNode();
   }
   if (!m_pChild && m_pParent->canCreateDummyItem()){
      m_pDummyItem = m_pParent->createDummyItem();
      m_pChild = m_pDummyItem.get();
      m_Index = 0;
      m_RepeatIndex = 0;
      m_pFormatter->pushNode(*m_pDummyItem, -1);
      return true;
   } else {
      if (++m_Index < m_pParent->size()){
         m_pChild = &m_pParent->child(m_Index);
         if (indexByName()){  
            m_RepeatIndex = TRNgetRepeatIndex(m_RepeatsByName, m_pChild->name());
         } else {
            m_RepeatIndex = 0;
         }
         m_pFormatter->pushNode(*m_pChild, m_Index);
         return true;
      } else {
         return false;
      }
   }
}

TRNtraverser* TRNtraverserNode::traverser() const {
   COLPRECONDITION(!isLeaf());
   COLPRECONDITION(m_pChild);

   if (m_pParent->protocolType() != m_pChild->protocolType()){
      return TRNcreateTraverser(*m_pChild);
   } else {
      return new TRNtraverserNode(*m_pChild, m_pFormatter.get());
   }
}

bool TRNtraverserNode::isLeaf() const {
   if (m_pChild){
      return m_pChild->isSimple();
   } else {
      return m_pParent->isSimple();
   }
}

bool TRNtraverserNode::isNull() const {
   return m_pChild ? m_pChild->isNull() : m_pParent->isNull();
}

void TRNtraverserNode::getTypeInfo(int& ProtocolType, int& NodeType) const{
   const NODplace* pPlace = m_pChild ? m_pChild : m_pParent;
   ProtocolType = pPlace->protocolType();
   NodeType     = pPlace->nodeType();
}

void TRNtraverserNode::getUniqueKey(COLstring& Key) const{
   if (m_pChild){
      if (indexByName()){
         Key = TRN_STRING_KEY_PREFIX + COLintToString(m_RepeatIndex) + m_pChild->name();
      } else {
         Key = TRN_NUMBER_KEY_PREFIX + COLint32ToString(m_Index);
      }
   } else {
      Key = TRN_ROOT_KEY;
   }
}

bool TRNtraverserNode::isLowPriority() const{
   COLPRECONDITION(m_pChild);
   return m_pChild->isNull();
}

TRNtraverser::eSearchResult TRNtraverserNode::findMatch(const REXmatcher &SearchRegex, int SearchIndex) const{
   COL_METHOD(TRNtraverser::findMatch);
   COLPRECONDITION(m_pChild);
   const COLstring& ChildName = m_pChild->name();
   COL_VAR3(ChildName, m_pChild->isSimple(), m_pChild->isNull());


   if (m_Index == SearchIndex){
      return TRNtraverser::INDEX_MATCH;
   }
   if (m_pChild->isNamed() && !ChildName.is_null()){
      COLindex DummyIndex = 0;
      if (SearchRegex.findMatchWithLength(ChildName.c_str(), ChildName.size(), 0, DummyIndex, DummyIndex)){
         return TRNtraverser::STRING_MATCH;
      }
   }
   if (m_pChild->isSimple() && !m_pChild->isNull()){
      const COLstring& ChildValue = m_pChild->value();
      COL_VAR(ChildName);
      COLindex DummyIndex = 0;
      if (!ChildValue.is_null() && SearchRegex.findMatchWithLength(ChildValue.c_str(), ChildValue.size(), 0, DummyIndex, DummyIndex)){
         return TRNtraverser::STRING_MATCH;
      }
   }
   return TRNtraverser::NO_MATCH;
}

// TODO - would be better if intellisense library doesn't know NODe types.
COLstring TRNtraverserNode::optionDisplayText(COLstring* pFuncName) const{
   COLPRECONDITION(m_pChild);

   COLstring Result;
   COLostream ResultStream(Result);
   if (m_pParent->isVector()){
      switch (m_pParent->protocolType()){
      case 101:
         ResultStream << COL_T("Repeat ");
         break;
      }
      ResultStream << (m_Index + 1);
   } else {
      if (!m_pParent->favorIndexByName()){
         ResultStream << (m_Index + 1) << ' ';
      }
      const COLstring& Name = m_pChild->name();
      if (Name.size() > TRN_INTELLISENSE_MAX_LENGTH){
         TRNhtmlEscape(Name.substr(0, TRN_INTELLISENSE_MAX_LENGTH), Result);
         ResultStream << TRN_INTELLISENSE_TRUNCATED;
      } else {
         TRNhtmlEscape(Name, Result);
      }
   }
   writeTreeValue(Result, pFuncName);
   return Result;
}

COLstring TRNtraverserNode::deepOptionDisplayText(const REXmatcher& SearchRegex, const COLstring& RootDisplayText, bool IsIndexMatch) const {
   COLPRECONDITION(m_pChild);
   const COLstring& ChildName = m_pChild->name();

   COLstring Result(RootDisplayText);
   COLostream ResultStream(Result);
   if (ChildName.is_null()){
      if (IsIndexMatch){
         ResultStream << '[' << TRN_BEGIN_HIGHLIGHT_SEARCH_MATCH << (m_Index+1) << TRN_END_HIGHLIGHT_SEARCH_MATCH << ']';
      } else{
         ResultStream << '[' << (m_Index+1) << ']';
      }
   }else{
      if (!RootDisplayText.is_null()) ResultStream << TRN_LEVEL_DIVIDER;
      if (IsIndexMatch){
         ResultStream << TRN_BEGIN_HIGHLIGHT_SEARCH_MATCH << (m_Index+1) << TRN_END_HIGHLIGHT_SEARCH_MATCH << ' ';
      } else if (!m_pParent->favorIndexByName()){
         ResultStream << (m_Index+1) << ' ';
      }
      COLstring HtmlName;
      if (ChildName.size() > TRN_INTELLISENSE_MAX_LENGTH){
         SearchRegex.regexReplace(HtmlName, ChildName.substr(0, TRN_INTELLISENSE_MAX_LENGTH), TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
         HtmlName.append(TRN_INTELLISENSE_TRUNCATED);
      } else {
         SearchRegex.regexReplace(HtmlName, ChildName, TRN_HIGHLIGHT_SEARCH_MATCH, TRNhtmlEscapeString);
      }
      ResultStream << HtmlName;
   }
   return Result;
}

COLstring TRNtraverserNode::optionFullText(const COLstring& RootVar) const{
   COL_METHOD(TRNtraverserNode::optionFullText);
   COLPRECONDITION(m_pChild);
   const COLstring& ChildName = m_pChild->name();
   COL_VAR(ChildName);
   if (indexByName()){
      if (m_RepeatIndex > 0){
         COL_TRC("it's a colon");
         return RootVar + ":child(\"" + ChildName + "\", " + COLint32ToString(m_RepeatIndex + 1) + ")";
      } else {
         COLstring Result = RootVar;
         COLostream ResultStream(Result);
         COL_VAR(TRNisValidLuaName(ChildName));
         TRNwriteLuaStringKey(ChildName, ResultStream);
         COL_VAR(Result);
         return Result;
      }
   } else {
      COLstring Result = RootVar;
      COLostream ResultStream(Result);
      ResultStream << '[' << (m_Index+1) << ']';
      COL_VAR(Result);
      return Result;
   }
}

//Used in the generation of a single text option when more than 1 text nodes are
//found in an XML node. In TRNintellisenseOptions.cpp
//Ticket #27741 for more information
COLstring TRNtraverserNode::childValue() const {
   return m_pChild->value();
}

COLstring TRNtraverserNode::optionFullText(const COLstring& RootVar, lua_State* L) const{
   COL_METHOD(TRNtraverserNode::optionFullText);
   const COLstring& ChildName = m_pChild->name();
   // TODO - lol - 'interesting' design.  If we don't have the LND library installed we get a crash.
   lua_getglobal(L, "node");
   lua_pushnil(L);
   while (lua_next(L, -2)){
      if (lua_tostring(L, -2) == ChildName){
         COL_VAR(lua_tostring(L, -2));
         COL_TRC("We have a name collision.");
         lua_pop(L, 3); //Return the stack to original form.
         return RootVar + ":child(\"" + ChildName + "\", " + COLint32ToString(m_RepeatIndex + 1) + ")";
      }
      lua_pop(L, 1); //Pop off function before getting next item
   }
   lua_pop(L, 1); //Pop off remaining function name left on the stack.
   return optionFullText(RootVar);
}

void TRNtraverserNode::writeTreeValue(COLstring& Sink, COLstring* pFuncName, const REXmatcher* pSearchRegex) const {
   // Warning - if later we decide to set pFuncName, you must check that it is a valid pointer first
   const NODplace* pNode = m_pChild ? m_pChild : m_pParent;

   Sink.write(TRN_BEGIN_TREE_VALUE.c_str(), TRN_BEGIN_TREE_VALUE.size());
   if (!pSearchRegex || !pNode->isSimple()){
      COLstring TreeValue;
      COLostream TreeValueStream(TreeValue);
      COL_TRC("Calling NTV");
      NTVtreeValue(TreeValueStream, *pNode, true);
      SFIhtmlCrop(TreeValue.c_str(), TreeValue.size(), TRN_INTELLISENSE_TRUNCATED, TRN_INTELLISENSE_MAX_LENGTH, Sink);
   } else {
      if (!pNode->isNull()){
         COLstring EscapedAndHighlightedStr = pNode->value();
         pSearchRegex->regexReplace(EscapedAndHighlightedStr, TRN_HIGHLIGHT_SEARCH_MATCH, TRNsearchMatchEscaper);
         COLostream Stream(Sink);
         THTMbeginSpan(Stream, NOD_SPAN_STRING);
         SFIhtmlCrop(EscapedAndHighlightedStr.c_str(), EscapedAndHighlightedStr.size(), TRN_INTELLISENSE_TRUNCATED, TRN_INTELLISENSE_MAX_LENGTH, Sink);
         THTMendSpan(Stream);
      }
   }
   Sink.write(TRN_END_TREE_VALUE.c_str(), TRN_END_TREE_VALUE.size());
}

bool TRNtraverserNode::indexByName() const {
   COLPRECONDITION(m_pChild);
   return m_pChild->isNamed() && m_pParent->isComplex() && m_pParent->favorIndexByName();
}

void TRNtraverserNode::getTreeviewText(COLstring& ShortLabel, COLstring& LongLabel) const {
   ShortLabel  = m_pFormatter->expandedView();
   LongLabel = m_pFormatter->collapsedView();
}

TRNtraverser* TRNcreateTraverser(const NODplace& RootPlace){
   return new TRNtraverserNode(RootPlace);
}

TRNtraverser* TRNcreateTraverser(const NODaddress& RootAddress){
   return new TRNtraverserNode(RootAddress);
}
