#ifndef __TRN_WRITE_STRING_VAR_H__
#define __TRN_WRITE_STRING_VAR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNwriteStringVar
//
// Description:
//
// Helper
//
// Author: Eliot Muir
// Date:   Wednesday, November 24th, 2010 @ 01:19:34 PM
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>

class TRNarg;
class CHNKrenderedChunk;

void TRNwriteStringVar(const COLarray<CHNKrenderedChunk>& Chunks, const TRNarg& Arg, COLvar* pResult);

#endif // end of defensive include
