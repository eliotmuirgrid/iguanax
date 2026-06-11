#ifndef __TRN_INTELLISENSE_MAIN_H__
#define __TRN_INTELLISENSE_MAIN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseMain
//
// Description:
//
// TRNintellisensMain
//
// Author: Eliot Muir
// Date:   Thursday, December 23rd, 2010 @ 10:21:13 PM
//---------------------------------------------------------------------------
#include <COL/COLstring.h>

class COLvar;
class TRNannotationJob;
class TRNintellisenseContext;

void TRNintellisenseMain(TRNintellisenseContext* pContext, COLvar* pOut, TRNannotationJob* pJob);

#endif // end of defensive include
