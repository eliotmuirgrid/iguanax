#ifndef __TRN_TRAVERSER_NODE_H__
#define __TRN_TRAVERSER_NODE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtraverserNode
//
// Description:
//
// Node tree traverser.  This was factored out of TRNtraverser.  It's
// used for intellisense and for full argument APIS.
//
// Author: Eliot Muir 
// Date:   Thursday 04 May 2023 - 08:52AM
// ---------------------------------------------------------------------------

#include <COL/COLhashmap.h>
#include <COL/COLref.h>
#include <COL/COLauto.h>

#include <NOD/NODplace.h>
#include <NOD/NODaddress.h>

#include <TRN/TRNtraverser.h>

class TRNtraverserNode : public TRNtraverser{
public:
   TRNtraverserNode(const NODplace& Place);
   TRNtraverserNode(const NODaddress& RootAddress);
   TRNtraverserNode(const NODplace& Place, TRNtreeFormatter* pFormatter);
   ~TRNtraverserNode();

   bool stepNext();
   TRNtraverser* traverser() const;
   bool isLeaf() const;
   bool isNull() const;
   const COLstring traverserType() const { return TRN_NODE_TRAVERSER_TYPE; }

   void getTypeInfo(int& ProtocolType, int& NodeType) const;
   void getUniqueKey(COLstring& Key) const;
   void getLuaKey(COLstring& LuaKey) const {} // do nothing
   bool isComplexLuaItem() const { return false; }

   bool isLowPriority() const;
   TRNtraverser::eSearchResult findMatch(const REXmatcher& SearchRegex, int SearchIndex) const;
   COLstring childValue() const;
   COLstring optionDisplayText(COLstring* pFuncName=NULL) const;
   COLstring deepOptionDisplayText(const REXmatcher& SearchRegex, const COLstring& RootDisplayText, bool IsIndexMatch) const;
   COLstring optionFullText(const COLstring& RootVar) const;
   COLstring optionFullText(const COLstring& RootVar, lua_State* L) const;

   void writeTreeValue(COLstring& String, COLstring* pFuncName=NULL, const REXmatcher* pSearchRegex = NULL) const;

   void getTreeviewText(COLstring& ShortLabel, COLstring& LongLabel) const;

   void getTreeviewKey(COLstring& Key) const {} // do nothing
   void getTreeviewValue(COLstring& Value) const {} // do nothing
   const NODplace* nodPlace() const { return NULL; } // do nothing

private:
   const NODplace* m_pParent;
   const NODplace* m_pChild;

   // The following indices refer to the child's position inside the parent.
   int m_Index;
   int m_RepeatIndex;

   COLhashmap<COLstring, int> m_RepeatsByName;

   // Just to make sure any dummy items don't go out of scope.
   COLref<NODplace> m_pDummyItem;

   mutable COLauto<TRNtreeFormatter> m_pFormatter;

   bool indexByName() const;
};
#endif // end of defensive include