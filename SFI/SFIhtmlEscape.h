#ifndef __SFI_HTML_ESCAPE_H__
#define __SFI_HTML_ESCAPE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhtmlEscape
//
// Description:
//
// SFIhtmlEscape works like SFIjsonEscape - it will write the escaped data
// directly to a COLsink.  The function is mostly copied from CShtmlEscape.
//
// The following conversions will be made:
//
// ';'  => "&#59;"
// '&'  => "&amp;"
// '<'  => "&lt;"
// '>'  => "&gt;"
// '\"' => "&quot;"
// '\n' => "<br>"  * This is not necessarily standard HTML escaping, but it is very useful.
//
// Author: Kevin Senn
// Date:   Wednesday, April 8th, 2009 @ 09:24:58 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class COLsink;
class COLstring;

// These interfaces are for streaming.
void SFIhtmlEscape(const char* Data, int SizeOfData, COLsink& Sink);
void SFIhtmlEscape(const char* Data, int SizeOfData, COLstring& String);  // overloaded to make taking COLsink out of COLstring easier.

void SFIhtmlEscapeNoNewline(const char* Data, int SizeOfData, COLsink& Sink);
void SFIhtmlEscapeNoNewline(const char* Data, int SizeOfData, COLstring& String);  // overloaded to make taking COLsink out of COLstring easier.

void SFIhtmlEscapeWithCRAsNewline(const char* Data, int SizeOfData, COLsink& Sink);
COLstring SFIhtmlEscapeWithCRAsNewlineString(const COLstring& String);  // Convenient for small strings.

// only turns newlines into <br>
void SFIhtmlEscapeNewlineOnly(const char* Data, int SizeOfData, COLsink& Sink);
void SFIhtmlEscapeNewlineOnly(const char* Data, int SizeOfData, COLstring& Sink);




#endif // end of defensive include
