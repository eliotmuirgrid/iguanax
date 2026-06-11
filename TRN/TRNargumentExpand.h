#ifndef __TRN_ARGUMENT_EXPAND_H__
#define __TRN_ARGUMENT_EXPAND_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNargumentExpand
//
// Description:
//
// /ide/full_arg api
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

#include <COL/COLweb.h>
class TRNannotationJob;

void TRNargumentExpand(const COLvar& Input, TRNannotationJob* pTranslator, COLvar* pResult);

#endif // end of defensive include