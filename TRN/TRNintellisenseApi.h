#ifndef __TRN_INTELLISENSE_API_H__
#define __TRN_INTELLISENSE_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisenseApi
//
// Description:
//
// Core of intellisense API
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

#include <COL/COLweb.h>
class TRNannotationJob;

void TRNintellisenseApi(const COLvar& Input, TRNannotationJob* pTranslator, COLvar* pOut);

#endif // end of defensive include
