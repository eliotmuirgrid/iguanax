#ifndef __TRN_INTELLISENSE_H__
#define __TRN_INTELLISENSE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNintellisense
//
// Description:
//
// TRNintellisense - routine for formatting JSON.
//
// Author: Eliot Muir
// Date:   Monday, December 20th, 2010 @ 01:18:11 PM
//---------------------------------------------------------------------------

class TRNintellisenseContext;
class COLvar;
class COLstring;

void TRNintellisenseJson(COLvar* pOut, const TRNintellisenseContext& Context, int LineNumber, int CursorPosition);

#endif // end of defensive include
