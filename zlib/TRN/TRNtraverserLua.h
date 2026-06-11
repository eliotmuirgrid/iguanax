#ifndef __TRN_TRAVERSER_LUA_H__
#define __TRN_TRAVERSER_LUA_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNtraverserLua
//
// Description:
//
// Lua traverser.  This was part of TRNtraveser.  It is used as part of intellisense
// and for the full arg APIs.
//
// Author: Eliot Muir 
// Date:   Thursday 04 May 2023 - 08:54AM
// ---------------------------------------------------------------------------

#include <TRN/TRNtraverser.h>

#include <COL/COLarray.h>
#include <COL/COLauto.h>

class TRNluaItem{
public:
   TRNluaItem();

   COLstring       m_String;
   int             m_LuaType;
   bool            m_IsInteger;
   int             m_Integer; // valid iff m_IsInteger is true
   const NODplace* m_pNode; // if non-null, this item is a NOD node
   const void*     m_pAddress; // valid iff item is not a simple type (number, boolean, or string)

   bool operator==(const TRNluaItem& That) const;
};

class TRNtraverserLua : public TRNtraverser{
public:
   TRNtraverserLua(lua_State* iL, int TableIndex, bool ForDisplay, char IndexChar);
   TRNtraverserLua(lua_State* iL, int TableIndex, bool ForDisplay, char IndexChar, COLarray<const void*>* pTableStack);

   ~TRNtraverserLua();

   bool stepNext();
   TRNtraverser* traverser() const;
   bool isLeaf() const;
   bool isNull() const { return false; } // no null lua nodes - setting nil as a table entry removes the entry.
   const COLstring traverserType() const { return TRN_LUA_TRAVERSER_TYPE; }

   void getTypeInfo(int& ProtocolType, int& NodeType) const;
   void getUniqueKey(COLstring& Key) const;
   void getLuaKey(COLstring& LuaKey) const;
   bool isComplexLuaItem() const;

   bool isLowPriority() const { return false; }
   TRNtraverser::eSearchResult findMatch(const REXmatcher& SearchRegex, int SearchIndex) const;
   COLstring optionDisplayText(COLstring* pFuncName=NULL) const;
   COLstring childValue() const;
   COLstring deepOptionDisplayText(const REXmatcher& SearchRegex, const COLstring& RootDisplayText, bool IsIndexMatch) const;
   COLstring optionFullText(const COLstring& RootVar) const;
   //This signature of the optionFullText is only declared because it is used in TRNtraverserNode
   //For more info check out ticket/branch #27741
   COLstring optionFullText(const COLstring& RootVar, lua_State* L) const;
   void writeTreeValue(COLstring& String, COLstring* pFuncName=NULL, const REXmatcher* pSearchRegex = NULL) const;

   void getTreeviewText(COLstring& ShortLabel, COLstring& LongLabel) const;

   void getTreeviewKey(COLstring& Key) const;
   void getTreeviewValue(COLstring& Value) const;
   const NODplace* nodPlace() const;

private:
   enum eIterationState{
      NOT_STARTED,
      ROOT_TABLE,
      INDEX_TABLE,
      KEYS_TABLE,
      DONE
   };

   lua_State* L;
   const int m_InitialStackSize;
   int m_TableIndex;
   int m_OriginalTableIndex; // used iff m_IterationState == KEYS_TABLE
   int m_KeyIndex;
   int m_ValueIndex;

   COLarray<TRNluaItem> m_Keys;

   TRNluaItem m_Value;
   bool m_ValueHasMetatable;
   COLstring m_ValueShortLabel;
   COLstring m_ValueLongLabel;
   int m_ValueDisplayProtocolType;
   int m_ValueDisplayNodeType;

   bool m_ForDisplay;
   char m_IndexChar;

   eIterationState m_IterationState;

   mutable COLauto< COLarray<const void*> > m_pTableStack;

   void initRoot();
   void initIteration();
   void initStackForIteration();
   bool initKeysTableIteration();
   bool initIndexTableIteration();
   void iterateTable();
   bool initKeyAndValue(); // returns false if the entry should be skipped
   const TRNluaItem& currentKey() const { return m_Keys.back(); }
   TRNluaItem& currentKey() { return m_Keys.back(); }
};

#endif // end of defensive include