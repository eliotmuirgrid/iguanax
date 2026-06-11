#ifndef __SFI_JSON_ESCAPE_H__
#define __SFI_JSON_ESCAPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIjsonEscape
//
// Description:
//
// SFIjsonEscape function - someone else can make this into a COLfilter interface
// later if they want to.  For my purposes this interface is pretty fast.
//
// Author: Eliot Muir
// Date:   Monday, February 9th, 2009 @ 09:49:57 AM
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// WARNING - this code needs some love #24133

void SFIjsonEscape(const char* Data, int SizeOfData, COLsink& Sink);
void SFIjsonEscape(const char* Data, int SizeOfData, COLstring& String);  // overloaded operator

// SFIisJsonSafe() returns true if the given string is valid inside
// a JSON string delimited by Delimiter when set; if Delimiter is
// unset both ' and " must be escaped in the given string.  Some
// valid strings will fail this test, but no invalid ones will pass.
// Javascript ignores invalid escapes, and so do we.
//
bool SFIisJsonSafe(const char*, size_t, char Delimiter = 0);

#endif // end of defensive include
