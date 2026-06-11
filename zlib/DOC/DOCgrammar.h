#ifndef __DOC_GRAMMAR_H__
#define __DOC_GRAMMAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DOCgrammar
//
// Description:
//
// Very simple grammar
//
// Author: Eliot Muir 
// Date:   Tuesday 26 August 2025 - 12:54PM
// ---------------------------------------------------------------------------

#include <COL/COLstring.h>
#include <COL/COLarray.h>

class DOCgrammar{
public:
  enum NodeType{
      String=0,
      Integer,
      Bool,
      Array,
      Dictionary
   };

   DOCgrammar(NodeType iType, const COLstring& Name="", bool Required=false);
 
   COLstring Name;
   bool      Required;
   NodeType  Type;

   COLarray<DOCgrammar> Children;
};

bool DOCgrammarCheck(const COLvar& Data, const DOCgrammar& Grammar, COLstring* pError);

#endif // end of defensive include
