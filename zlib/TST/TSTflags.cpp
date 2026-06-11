//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTflags
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, August 16th, 2006 @ 10:30:05 AM
//
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTflags.h"
#include <COL/COLhashmap.h>

class TSTflagsPrivate
{
public:
   COLhashmap<COLstring, COLstring> FlagTable;
};

TSTflags::TSTflags()
{
   pMember = new TSTflagsPrivate;
}

TSTflags::~TSTflags()
{
   delete pMember;
}

void TSTflags::setFlag(const COLstring& Name, const COLstring& Value)
{
   pMember->FlagTable.add(Name, Value);
}

bool TSTflags::flagExists(const COLstring& Name) const
{
   return NULL != pMember->FlagTable.find(Name);
}

const COLstring& TSTflags::flagValue(const COLstring& Name) const
{
   if (!flagExists(Name))
   {
      COL_ERROR_STREAM_PLAIN("Flag " << Name << " does not exist in the TSTflag table.", COLerror::PreCondition);
   }
   else
   {
      return pMember->FlagTable[Name];
   }
}
