//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// SFIrandom unittests 
//
// Author: Nasron Cheong
// Date:   Feb 2007
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include <COL/COLarray.h>
#include <COL/COLhashmap.h>
#include <COL/COLdateTime.h>
#include <COL/COLmath.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <SFI/SFIrandom.h>

#include <TST/TSTtestCollection.h>
#include <TST/TSThelpers.h>
#include <TST/TSTtestFunction.h>

static const int MAX_TRIALS = 40000;

//
// Basically generate a large set of random numbers
// keeping track of repetitions, as a gauge of how 'good'
// the c rand() is
//
COLuint32 testRandRepeats(const COLint64 MaxTrials)
{
   COLdateTime Start(COLdateTime::currentTime());

   SFIrandom Rng;
   COLhashmap<COLuint32,COLuint32> GenList;
   COLuint32 RandomNum = 0;
   COLint64 IterCount = MaxTrials;
   COLuint32 MaxRepeats = 0;
   while(IterCount--)
   {
      RandomNum = COLmath::randomNumber( RAND_MAX-1 );
      if (GenList.find(RandomNum))
      {
         MaxRepeats++;
      }
      else
      {
         GenList.add(RandomNum,RandomNum);
      }
   }
   COLdateTime End(COLdateTime::currentTime());

   COL_TRC("Repeats found for bits == " << sizeof(RandomNum)*8 
           << " : " << MaxRepeats 
           << " in " << MaxTrials 
           << " in " << (End-Start).seconds() << " seconds (" 
           << (100*(double)MaxRepeats/MaxTrials) << "%)");
   COL_VAR(MaxRepeats);
   return MaxRepeats;
}

bool testSFIisStrongRngAvailable()
{
   return SFIrandom().isStrongRng();
}

//
// Basically generate a large set of random numbers
// keeping track of repetitions, as a gauge of how 'good'
// the SFIrandom
//

static COLstring BufferAsHex(const void* buf, int len)
{
   static char Hex[] = "0123456789abcdef";
   const unsigned char* p = (const unsigned char*)buf;
   COLstring Result;
   for (int i = 0; i < len; ++i) {
      const unsigned char b = p[i];
      Result += Hex[(b & 0xf0) >> 4];
      Result += Hex[(b & 0x0f)];
   }
   return Result;
}

COLuint32 testRandomRepeats(const COLint64 MaxTrials, const COLuint32 ByteSize)
{
   COL_FUNCTION(testRandomRepeats);
   const int Bits = ByteSize * 8;
   COL_VAR3(MaxTrials, ByteSize, Bits);
   COLhashmap<COLstring,char> GenMap;

   SFIrandom Rng;
   COLint64 IterCount = MaxTrials;
   COLuint32 RepeatsFound = 0;
   COLstring RandomValue(ByteSize, 0);
   while (IterCount--) {
      Rng.generate((void*)RandomValue.data(), RandomValue.size());
      const COLstring RandomHexString = BufferAsHex(RandomValue.data(), RandomValue.size());
      COL_TRC(RandomHexString);
      if (GenMap.find(RandomHexString)) {
         COL_TRC("repeat found");
         RepeatsFound++;
      } else {
         GenMap.add(RandomHexString,0);
      }
   }

   COL_TRC(RepeatsFound << " repeats found for " << ByteSize*8 << " bits in " << MaxTrials << " trials");
   return RepeatsFound;
}

/*
// Looks like this test is only here for comparison; to show
// how much more random SFIrandom is.
void testLibcRand()
{
   COLmath::resetRandomGenerator();

   //COL_TRC(" Using libc rand() : ");
   testRandRepeats(MAX_TRIALS);
}

void testRandomRepeatsWith32Bits()
{
   
   COLmath::resetRandomGenerator();

   //COL_TRC(" Using SFIrandom() : ");
   COLuint32 CryRepeats = testRandomRepeats(MAX_TRIALS,sizeof(COLuint32));

   if (testSFIisStrongRngAvailable())
   {
      TST_ASSERT(CryRepeats < 40);
   }
}
*/

void testRandomRepeatsWith1Byte()
{
   COL_FUNCTION(testRandomRepeatsWith1Byte);
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();
   COLuint32 Repeats8 = testRandomRepeats(256,1);
   COL_VAR(Repeats8);
   TST_ASSERT(Repeats8 < 150); 
}

/*
void testRandomRepeatsWith2Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats16 = testRandomRepeats(65536,2);
   if (testSFIisStrongRngAvailable())
   {
      TST_ASSERT(Repeats16 < 27000); 
   }
}

void testRandomRepeatsWith3Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats24 = testRandomRepeats(MAX_TRIALS,3);
   if (testSFIisStrongRngAvailable())
   {
      TST_ASSERT(Repeats24 < 10000 );
   }
}

void testRandomRepeatsWith4Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats32 = testRandomRepeats(MAX_TRIALS,4);

   if (testSFIisStrongRngAvailable())
   {
      TST_ASSERT(Repeats32 < 40 );
   }
}

void testRandomRepeatsWith5Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats40 = testRandomRepeats(MAX_TRIALS,5);

   if (testSFIisStrongRngAvailable())
   {
      TST_ASSERT(Repeats40 < 10);
   }
}

void testRandomRepeatsWith6Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats = testRandomRepeats(MAX_TRIALS,6);

   if (testSFIisStrongRngAvailable())
   {
      //so large a range that it should definitely be zero.
      TST_ASSERT_EQUALS(Repeats, 0);
   }
}

void testRandomRepeatsWith7Bytes()
{
   COLdateTime Start(COLdateTime::currentTime());
   COLmath::resetRandomGenerator();

   COLuint32 Repeats = testRandomRepeats(MAX_TRIALS,7);
   if (testSFIisStrongRngAvailable())
   {
      //so large a range that it should definitely be zero.
      TST_ASSERT_EQUALS(Repeats, 0);
   }
}
*/

void testRandomRepeatsWith8Bytes()
{
   COL_FUNCTION(testRandomRepeatsWith8Bytes);
   COLmath::resetRandomGenerator();

   COLuint32 Repeats = testRandomRepeats(MAX_TRIALS,8);

   //so large a range that it should probably be zero.
   TST_ASSERT_EQUALS(Repeats, 0);
}

void testRandomRepeatsWith16Bytes()
{
   COL_FUNCTION(testRandomRepeatsWith16Bytes);
   COLmath::resetRandomGenerator();

   COLuint32 Repeats = testRandomRepeats(MAX_TRIALS,16);

   //so large a range that it should "definitely" be zero.
   TST_ASSERT_EQUALS(Repeats, 0);
}

void testRandomIsStrongRng()
{
   SFIrandom Rng;
   TST_ASSERT_EQUALS(true,Rng.isStrongRng())
}

TSTtestable* SFITrandom()
{
   TST_START_COLLECTION("testRandom")
      //TST_ADD_TEST_FUNCTION(testLibcRand)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith32Bits)
      TST_ADD_TEST_FUNCTION(testRandomRepeatsWith1Byte)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith2Bytes)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith3Bytes)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith4Bytes)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith5Bytes)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith6Bytes)
      //TST_ADD_TEST_FUNCTION(testRandomRepeatsWith7Bytes)
      TST_ADD_TEST_FUNCTION(testRandomRepeatsWith8Bytes)
      TST_ADD_TEST_FUNCTION(testRandomRepeatsWith16Bytes)
      TST_ADD_TEST_FUNCTION(testRandomIsStrongRng)
   TST_END_COLLECTION
}

