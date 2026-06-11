#ifndef __THTM_PRINTER_H__
#define __THTM_PRINTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: THTMprinter
//
// Description:
//
// HTML print helper functions
//
// Author: Kevin Senn
// Date:   Wednesday, January 11th, 2012 @ 09:27:05 AM
//---------------------------------------------------------------------------

#include <NOD/NODprintCallback.h>

void THTMbeginSpan(COLostream& Stream, NODprintSpanType SpanType);
void THTMprintEscaped(COLostream& Stream, const char* pData, int Length);
void THTMprintEscapedLiteral(COLostream& Stream, const char* pData, int Length);
void THTMprintEscapedLiteralWithNewlines(COLostream& Stream, const char* pData, int Length);
void THTMendSpan(COLostream& Stream);

#endif // end of defensive include