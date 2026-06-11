#ifndef __TRN_ANNOTATION_H__
#define __TRN_ANNOTATION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNannotation
//
// Description:
//
// This powers the /ide/annotation API.
//
// Author: Eliot Muir
// Date:   Tuesday, November 16th, 2010 @ 02:58:17 PM
//---------------------------------------------------------------------------

class COLvar;
class TRNannotationJob;

void TRNannotation(const COLvar& Params, TRNannotationJob* pTranslator, COLvar* pResult);

#endif // end of defensive include
