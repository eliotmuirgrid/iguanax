#ifndef __TXT_UTILS_H__
#define __TXT_UTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTutils
//
// Description:
//
// Small, distinctly texty things.
//
// Author: Eric Mulvaney
// Date:   Friday, May 29th, 2009 @ 11:20:13 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class COLstring;

// This can be used to apply a natural ordering to strings.  It is
// case insensitive, and recognizes embedded natural numbers and
// sorts them like numbers, not like ASCII strings.
// 
// Note: If this function returns 0 then this means that the strings
// are equal according to the natural ordering, but they may still
// have differences when case is concerned. The COLstring compare
// method can be called on the strings afterwards to determine if
// the strings are equal to each other with regards to case as well.
//
int TXTnaturalCompareStrings(const COLstring& Left, const COLstring& Right);


// COLmap needs a class-wrapped static function.
//
class TXTnaturalCompareStringsClass
{
public:
   static int compare(const COLstring& Left, const COLstring& Right)
   {
      return TXTnaturalCompareStrings(Left, Right);
   }
};


// Expand in-place all HL7-escaped 8-bit characters (extended ASCII).
// Non-standard HL7 messages are not modified (see note below).
//
// Bug:  By "standard HL7", we expect each message to begin with an MSH
// header, and the standard compliment of delimiters, e.g., "MSH|^~\&|".
// The delimiters may vary, but their positions and number cannot, nor
// can there be duplicate delimiters.  All of these deviations can be
// detected except for when delimiters are rearranged.  In that case, we
// will end up using the wrong delimiter to detect escape sequences.
// However, sequences DXNND (with 0xNN >= 0x80) should be rather rare
// with any non-escape, non-field delimiter D.
//
// The only way to fix this, and to make this algorithm applicable to
// non-standard HL7 messages, would be to make escaping configurable
// in the UI, since we cannot rely on an appropriate VMD in all cases.
// 
void TXTunescapeExtendedAscii(COLstring&);

// The opposite of TXTunescapeExtendedAscii().  HL7 messages are detected
// as above, and all 8-bit characters are escaped.
//
void TXTescapeExtendedAscii(COLstring&);

// As above, except that HL7 messages are not detected, the escape
// character must be specified.
//
void TXTescapeExtendedAscii(COLstring&, char);

void TXTescapeNewLines(COLstring* pString);
void TXTunescapeNewLines(COLstring* pString);
bool TXThasBadAscii(const COLstring& Data);

#endif // end of defensive include
