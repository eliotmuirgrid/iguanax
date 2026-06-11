#ifndef __TRN_INTELLISENSE_OPTIONS_H__
#define __TRN_INTELLISENSE_OPTIONS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseOptions
//
// Description:
//
// TRNintellisenseOptions class.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 02:09:31 PM
//
//---------------------------------------------------------------------------

#include <COL/COLmap.h>

class NODplace;
class TRNtraverser;

class TRNintellisenseOption{
public:
   enum ePriority{
      NORMAL = 0,
      HIGH = 1,
      MAX =2
   };

   TRNintellisenseOption(int iProtocolType, int iNodeType, int iLevel = 0, ePriority iPriority = NORMAL);
   TRNintellisenseOption(const TRNtraverser& Traverser, int iLevel = 0, ePriority iPriority = NORMAL);

   int ProtocolType;
   int NodeType;
   bool IsSimple;
   COLstring LuaKey;
   COLstring FullText; // eg, In.PID[5][1]
   COLstring DisplayText; // Name-based, with HTML styling
   COLstring FunctionName;

   // Sorting info
   ePriority Priority;
   int Level;
   int Index; // refers to the order in which it was found
};

class TRNintellisenseOptionCompareClass{
public:
   static int compare(const TRNintellisenseOption& Left, const TRNintellisenseOption& Right);   
};

class TRNsortedOptionsList{
public:
   TRNsortedOptionsList() : m_OptionsTruncated(false), m_NextIndex(0) {}

   void add(TRNintellisenseOption& Option);

   bool optionsTruncated() const { return m_OptionsTruncated; }

   // Simple wrappers
   COLindex size() const { return m_SortedOptions.size(); }
   COLavlTreePlace first() const { return m_SortedOptions.first(); }
   COLavlTreePlace next(COLavlTreePlace Place) const { return m_SortedOptions.next(Place); }
   const TRNintellisenseOption& option(COLavlTreePlace Place) const { return m_SortedOptions.key(Place); }

private:
   COLmap<TRNintellisenseOption, void*, TRNintellisenseOptionCompareClass> m_SortedOptions;
   bool m_OptionsTruncated;
   int m_NextIndex;
};

#endif // end of defensive include
