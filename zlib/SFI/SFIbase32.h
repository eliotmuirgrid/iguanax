#ifndef __SFI_BASE32_H__
#define __SFI_BASE32_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIbase32
//
// Description:
//
// SFIbase32 class.
//
// Author: Steven Dodd
// Date:   Thursday, August 19th, 2010 @ 02:55:40 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SFIbase32 
{
public:
   static const char* BASE32_CHARS;

   static void encode(COLsink& Destination, const COLuint8* pSource, COLuint32 SourceSize);
   
   static void encodeTuple(char Destination[8], const COLuint8 Source[5]);

private:
   SFIbase32(); // not allowed
};


void writeBase32Map();

#endif // end of defensive include
