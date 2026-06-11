//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFIrandom
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong
// Date:   Monday, February 12th, 2007 @ 05:07:13 PM
//---------------------------------------------------------------------------
#include "SFIprecomp.h"
#pragma hdrstop

#include "SFIrandom.h"
#include "SFIhexEncodeFilter.h"

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef _WIN32
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#endif

#include <time.h>

class SFIrandomBase {
public:
   SFIrandomBase() {}
   virtual ~SFIrandomBase() {}
   virtual bool isStrongRng() const = 0;
   virtual void seed(const void* pData, COLuint32 SeedSize) = 0;
   virtual void generate(void* pOutput, COLuint32 BufferSize) = 0;
public:
   SFIrandomBase(const SFIrandomBase&);
   SFIrandomBase& operator=(const SFIrandomBase&);
};

#ifdef _WIN32

class SFIrandomWindows : public SFIrandomBase {
public:
   SFIrandomWindows() : Provider(0) {
      if(!::CryptAcquireContext(&Provider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT)) {
         COL_THROW_WIN32_ERROR("CryptAcquireContext failed");
      }
      // contribute to the seed used by Windows' CryptGenRandom function
      const int SeedSize = 16;
      char Seed[SeedSize]; // largely unset bytes used on purpose
   
      // embded the time in the seed
      time_t t = ::time(0);
      ::memcpy(&Seed[1], &t, 4);
   
      // as well as some psuedo randomish stack address
      unsigned SomeStackAddress = (*(unsigned*)&t);
      ::memcpy(&Seed[7], &SomeStackAddress, 4);
   
      seed(Seed, SeedSize);
   }
   virtual ~SFIrandomWindows() {
      ::CryptReleaseContext(Provider, 0);
   }
   // Note: this call will not deterministically seed the RNG.
   // It will only contribute to Windows' RNG seed value.
   virtual void seed(const void* pData, COLuint32 SeedSize) {
      if (!SeedSize) {
         return;
      }
      // Windows only: make a copy of the data because windows will write to the buffer
      COLarray<unsigned char> Seed;
      Seed.resize(SeedSize);
      ::memcpy(Seed.getArray(),pData,SeedSize);
      if (!::CryptGenRandom(Provider,SeedSize,Seed.getArray())) {
         COL_THROW_WIN32_ERROR("SFIrandom::seed failed");
      }
   }
   virtual void generate(void* pOutput, COLuint32 BufferSize) {
      if (!BufferSize) {
         return;
      }
      if (!::CryptGenRandom(Provider,BufferSize,reinterpret_cast<unsigned char*>(pOutput))) {
         COL_THROW_WIN32_ERROR("SFIrandom::generate failed");
      }
   }
   virtual bool isStrongRng() const {return true;}
private:
   HCRYPTPROV Provider;
};

#else //!_WIN32

class SFIrandomUnix : public SFIrandomBase {
public:
   //return < 0 if error
   static int OpenUrandom() {
      return open("/dev/urandom",O_RDONLY);
   }
   SFIrandomUnix(int iRandomFd)
   : RandomFd(iRandomFd) {
   }
   virtual ~SFIrandomUnix() {
      ::close(RandomFd);
   }
   virtual void seed(const void* pData, COLuint32 SeedSize) {
      // no seeding is possible for /dev/urandom
   }
   virtual void generate(void* pOutput, COLuint32 BufferSize) {
      if (!BufferSize) {
         return;
      }
      COLuint32 AmountRead = ::read(RandomFd, pOutput, BufferSize);
      while (AmountRead < BufferSize) {
         AmountRead += ::read(RandomFd, 
                              reinterpret_cast<unsigned char*>(pOutput)+AmountRead, 
                              BufferSize-AmountRead);
      }
   }
   virtual bool isStrongRng() const {return true;}
private:
   int RandomFd; 
   SFIrandomUnix(const SFIrandomUnix&);
   SFIrandomUnix& operator=(const SFIrandomUnix&);
};

//
// The backup unix random number generator when /dev/urandom doesn't exist.
//
class SFIrandomLibC : public SFIrandomBase
{
public:
   SFIrandomLibC() {
      // seed input - clock
      struct timeval tv;
      ::gettimeofday(&tv, 0);
      // seed input - an address that new returned
      char* pDummy = new char[(unsigned)tv.tv_usec & 0xff];
      delete [] pDummy;
      // xor inputs: a heap address, a stack address, clock ms, time
      COLuint64 Seed = (((COLuint64)pDummy ^ (COLuint64)&pDummy) >> 2);
      Seed ^= (COLuint64)tv.tv_usec;
      Seed ^= (unsigned)::time(0);
      COL_DBG("SFIrandomLibC Seed = " << Seed);
      seed(&Seed, sizeof(Seed));
   }
   virtual ~SFIrandomLibC() { }
   virtual void seed(const void* pData, COLuint32 SeedSize) {
      unsigned SeedValue = 0;
      unsigned NumBytes = COL_MIN(SeedSize,sizeof(SeedValue));
      ::memcpy(&SeedValue, pData, NumBytes);
      COL_DBG("SFIrandomLibC SeedValue = " << SeedValue);
      ::srand(SeedValue);
   }
   virtual void generate(void* pOutput, COLuint32 BufferSize) {
      if (BufferSize > 0) {
         char* pOutputPtr = reinterpret_cast<char*>(pOutput);
         int RandValue = ::rand();
         const char* pRandValue = reinterpret_cast<const char*>(&RandValue);
         int CurrentPos = 0;
         while (CurrentPos < BufferSize) {
            *pOutputPtr = *pRandValue;
            pOutputPtr++;
            pRandValue++;
            CurrentPos++;
            if (CurrentPos % sizeof(RandValue) == 0) {
               RandValue = ::rand();
               pRandValue = reinterpret_cast<const char*>(&RandValue);          
            }
         }
      }
   }
   virtual bool isStrongRng() const { return false; }
private:
   SFIrandomLibC(const SFIrandomLibC&);
   SFIrandomLibC& operator=(const SFIrandomLibC&);
};

#endif //_WIN32

SFIrandom::SFIrandom() : pImpl(0) {
#ifdef _WIN32
   pImpl = new SFIrandomWindows;
#else
   int UrandFd = SFIrandomUnix::OpenUrandom();
   if (UrandFd < 0) {
      //doesn't exist, drop to lib c impl
      pImpl = new SFIrandomLibC;
   } else {
      pImpl = new SFIrandomUnix(UrandFd);
   }
#endif
}

SFIrandom::~SFIrandom() {
   delete pImpl;
}

bool SFIrandom::isStrongRng() const {
   return pImpl->isStrongRng();
}

void SFIrandom::seed(const void* pData, COLuint32 SeedSize) {
   pImpl->seed(pData,SeedSize);
}

void SFIrandom::generate(void* pOutput, COLuint32 BufferSize) {
   pImpl->generate(pOutput,BufferSize);
}

COLstring SFIrandomHexString(int LengthOfString) {
   COL_FUNCTION(SFIrandomHexString);
   COLPRECONDITION(LengthOfString % 2 == 0);

   COLstring RandomHexString;
   COLsinkString Sink(RandomHexString);
   SFIhexEncodeFilter HexFilter(&Sink);

   COLstring RandomBytes;
   RandomBytes.setCapacity(LengthOfString / 2 +1);
   RandomBytes.setSize(LengthOfString/2);
   SFIrandom().generate((void*)RandomBytes.c_str(), RandomBytes.size());
   HexFilter.write(RandomBytes.c_str(), RandomBytes.size());
   COL_VAR2(LengthOfString, RandomHexString);
   return RandomHexString;
}
