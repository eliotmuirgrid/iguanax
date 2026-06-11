#ifndef __SFI_HTML_CROP_H__
#define __SFI_HTML_CROP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIhtmlCrop
//
// Description:
//
// SFIhtmlCrop function.
//
// Crops the number of characters that will be DISPLAYED in an HTML-string.
// Any closing tags that are present in the string after the "cropping-point" will
// be included.  TextToAppend will be appended to the string iff it was cropped.
// But closing tags that close tags opened AFTER the cropping will NOT be included.
//
// For example,
// DWBhtmlCrop("<b>It's&nbsp;amazing</b><p>New Paragraph</p>", 44, "...", 7)
// will return "<b>It's&nbsp;am</b>...".
//
// WARNING: the length TextToAppend is not taken into account in the length of the
// resulting string.  For example, in the example above, the actual number of characters
// a browser would display is 7 + 3 (strlen("...")).
//
// NOTE: this doesn't take into account the actual semantics of the HTML tags.  For example,
// if a part of the string is "<span style='display:none;'>this text won't be display</span>", the
// characters inside the span will still be counted.  It also does not take into account the
// collapsing of whitespace.  For example, "  " will count as two characters.
//
// Author: Kevin Senn
// Date:   Tuesday, August 30th, 2011 @ 03:06:37 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
class COLsink;
class COLstring;

void SFIhtmlCrop(
   const char* pHtmlSource,
   const COLuint32 SourceSize,
   const COLstring& TextToAppend,
   COLuint32 MaxLength,
   COLsink& Sink
);

// Overloaded string version
void SFIhtmlCrop(
   const char* pHtmlSource,
   const COLuint32 SourceSize,
   const COLstring& TextToAppend,
   COLuint32 MaxLength,
   COLstring& Sink
);


#endif // end of defensive include
