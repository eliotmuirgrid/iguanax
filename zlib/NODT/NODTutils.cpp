//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: NODTutils
//
// Description:
//
// Implementation
//
// Author: Kevin Senn
// Date:   Tuesday, November 9th, 2010 @ 03:34:02 PM
//
//---------------------------------------------------------------------------
#include "NODTprecomp.h"
#pragma hdrstop

#include "NODTutils.h"

#include <NOD/NODplace.h>
#include <NOD/NODoperation.h>

#include <NTV/NTVtreeValue.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

/*COLstring NODTtreeValue(const NODplace& Place){
   COLstring Result;
   COLostream ResultStream(Result);
   NTVtreeValue(ResultStream, Place, false);
   return Result;
}*/

NODplace& NODTgetNamedChildRepeat(NODplace& Place, const COLstring& ChildName, int RepeatIndex){
   return Place.child(NODgetNamedChildRepeat(Place, ChildName, RepeatIndex));
}
