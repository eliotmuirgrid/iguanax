#ifndef __TRN_ANNOTATION_BLOCK_H__
#define __TRN_ANNOTATION_BLOCK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotationBlock
//
// Description:
//
// Code which handles updating annotation block data
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

#include <COL/COLweb.h>

class TRNannotationJob;

void TRNannotationBlock(const COLvar& Params, TRNannotationJob* pTranslator, COLvar* pResult);

#endif // end of defensive include
