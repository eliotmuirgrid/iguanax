#ifndef __SFI_CRC16_H__
#define __SFI_CRC16_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIcrc16
//
// Description:
//
// Calculate a 32 Bit CRC given a set of bytes.
//
// Copied from CRYcrc16.
//
// Author: Andrew Vajoczki
// Date:   Jan 13, 2010
//
//---------------------------------------------------------------------------

#include <COL/COLsink.h>

class SFIcrc16 : public COLsink
{
public: 
   explicit SFIcrc16(COLuint32 InitialRemainder) : Remainder(InitialRemainder){}
   SFIcrc16();
   virtual ~SFIcrc16();

   // Free function to determine the CRC32 of a buffer in one shot.
   static COLuint32 calculate(const void* Bytes, int CountOfBytes);

   // Updates the CRC value
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);

   // The resulting CRC digest after one or more writes
   COLuint32 digest();

   // Resets the CRC object
   void reset();

private:
   SFIcrc16(const SFIcrc16&);
   SFIcrc16 operator=(const SFIcrc16&);

   COLuint32 Remainder;
};

#endif // end of defensive include
