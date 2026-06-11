#ifndef __TRN_FORMAT_UNTRUNCATED_STRING_H__
#define __TRN_FORMAT_UNTRUNCATED_STRING_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNformatUntruncatedString
//
// Description:
//
// Format string in annotations with no truncation.
//
// Author: Eliot Muir
// Date:   Saturday, November 20th, 2010 @ 02:38:37 PM
//---------------------------------------------------------------------------

#include <LUA/LUAheader.h>
#include <COL/COLstring.h>

void TRNformatUntruncatedString(const char* pData, int Size, COLostream& Stream);

#endif // end of defensive include