#ifndef __SDB_CRC_H__
#define __SDB_CRC_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBcrc
//
// Description:
//
// Faster version of SFIcrc to deal with large message CRC calculations
//
// Author: John Qi
// Date:   Tue Jun 17 13:11:56 PDT 2025
// ---------------------------------------------------------------------------

#include <COL/COLsink.h>

class SDBcrc : public COLsink
{
public:
   explicit SDBcrc(COLuint32 InitialRemainder) : Remainder(InitialRemainder){}
   SDBcrc();
   virtual ~SDBcrc();

   // Free function to determine the CRC32 of a buffer in one shot.
   static COLuint32 calculate(const void* Bytes, int CountOfBytes);

   // Updates the CRC value
   virtual COLuint32 write(const void* cpBuffer, COLuint32 SizeOfBuffer);

   // The resulting CRC digest after one or more writes
   COLuint32 digest();

   // Resets the CRC object
   void reset();

private:
   SDBcrc(const SDBcrc&);
   SDBcrc operator=(const SDBcrc&);

   COLuint32 Remainder;
};

#endif // end of defensive include
