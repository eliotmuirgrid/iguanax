//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODgrammarType
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, May 12th, 2010 @ 02:06:16 PM
//
//---------------------------------------------------------------------------
#include "NODprecomp.h"
#pragma hdrstop

#include "NODgrammarType.h"

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring NODgrammarAsString(NODgrammarType GrammarType)
{
   switch(GrammarType)
   {
   case NOD_SIMPLE:  return "simple";
   case NOD_COMPLEX: return "complex";
   case NOD_VECTOR:  return "vector";
   default: COLASSERT(false); return COLstring();
   }
}
