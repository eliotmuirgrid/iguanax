#ifndef __SFI_RANDOM_H__
#define __SFI_RANDOM_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIrandom
//
// Description:
//
// A cryptographically strong random number generator.
// This class wraps OS functions that generate good random numbers.
//
// On windows this is implemented using <wincrypt.h> and on unix
// /dev/urandom is used (except for HPUX, where regular rand() is used)
//
// More or less plaigarized from crypto.
//
// Copied from CRYrandom.
//
// Author: Nasron Cheong
// Date:   Monday, February 12th, 2007 @ 05:07:13 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

class SFIrandomBase;

class SFIrandom {
public:
   // by default, the seed is generated from a region 16 bytes
   // returned by malloc upon construction
   SFIrandom();
   ~SFIrandom();
   
   // seeding is only valid on Windows, on Unix, we assume that the
   // /dev/urandom underlying implementation is always well seeded.
   void seed(const void* pData, COLuint32 SeedSize);

   // generate a random number of bytes to the provided 
   // output. e.g., to generate a 32 bit random number:
   // 
   // SFIrandom Rng;
   // COLuint32 RandomNumber = 0;
   // Rng.generate(&RandomNumber,sizeof(RandomNumber));
   void generate(void* pOutput, COLuint32 BufferSize);

   // if false, indicates that the normal rand() function is being used
   // and resulting data is not cryptographically strong
   bool isStrongRng() const;

private:
   SFIrandomBase* pImpl;
   SFIrandom(const SFIrandom& Orig); // not allowed
   SFIrandom& operator=(const SFIrandom& Orig); // not allowed
};

COLstring SFIrandomHexString(int LengthOfString);

#endif // end of defensive include
