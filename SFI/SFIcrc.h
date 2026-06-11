#ifndef __SFI_CRC_H__
#define __SFI_CRC_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrc
//
// Description:
//
// Calculate a 32 Bit CRC given a set of bytes.
//
// Copied from CRYcrc.
//
// Author: Andrew Vajoczki
// Date:   Monday, December 22nd, 2008 @ 12:01:27 PM
//
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

class SFIcrc : public COLsink
{
public: 
   explicit SFIcrc(COLuint32 InitialRemainder) : Remainder(InitialRemainder){}
   SFIcrc();
   virtual ~SFIcrc();

   // Free function to determine the CRC32 of a buffer in one shot.
   static COLuint32 calculate(const void* Bytes, int CountOfBytes);

   // Updates the CRC value
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);

   // The resulting CRC digest after one or more writes
   COLuint32 digest();

   // Resets the CRC object
   void reset();

private:
   SFIcrc(const SFIcrc&);
   SFIcrc operator=(const SFIcrc&);

   COLuint32 Remainder;
};

#endif // end of defensive include
