//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIhex
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Friday, August 4th, 2006 @ 09:36:35 AM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFIThex.h"
#include "SFITstream.h"

#include <SFI/SFIhexEncodeFilter.h>
#include <SFI/SFIhexDecodeFilter.h>
#include <SFI/SFIhexValidFilter.h>

#include <memory.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLsinkString.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testEncode()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexEncodeFilter TestFilter(&Sink);

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This is a test.";
   }
   TST_ASSERT(Data == "54686973206973206120746573742E");
   
   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This " << "is ";
      FilterStream.stream() << "also a test" << '!';
   }
   TST_ASSERT(Data == "5468697320697320616C736F2061207465737421");
}

static void testEncodeSpeed()
{
   const int Iterations = 1000;
   for (int Trials = 0; Trials < Iterations; ++Trials)
   {
      COLstring Data;
      COLsinkString Sink(Data);
      SFIhexEncodeFilter TestFilter(&Sink);
      Data.clear();
      TestFilter.resetFilter();
      {
         SFITstream FilterStream(TestFilter);
         for (int i = 0; i < Iterations; ++i)
         {
            FilterStream.stream() << "Therojhero eroje orjodfij9eugj0ij3t0ji3t0jsfgj sdfkljs odfjoisdjf 0sdfj 0sdfj0234j50ujsgjdpsgmsdf)IJ)*RJ_0j0j I)j0j0gj034tj0-3mtopmsfgopm0234j02m30234m0234m0234m023402i3jm40im0is0ifi0difj0i3245j0ijI~0jdf0gjdfgklmmmJ)J#jhfd9hf";
            FilterStream.stream() << "also a test" << '!';
         }
      }
      TST_ASSERT(Data.size() == 237*2*Iterations);
   }
}

static void testEncodeHigherThan127()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexEncodeFilter TestFilter(&Sink);

   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << (char)127 << (char)255;
   }
   TST_ASSERT_EQUALS("7FFF", Data);
}

static void testDecode()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexDecodeFilter TestFilter(&Sink);

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "54686973206973206120746573742e";
   }
   TST_ASSERT(Data == "This is a test.");

   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << '5' << "46" << '8' << "69" << '7' << '3';
      FilterStream.stream() << "2069732" << "0616c73";
      FilterStream.stream() << "6F20612074657374" << 21;
   }
   TST_ASSERT(Data == "This is also a test!");

   // Test it by resetting it midstream
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "54686973206973206";
      TestFilter.resetFilter();
      Data.clear();
      FilterStream.stream() << "546869732073747566662073686F756c646e27742062652069676E6F7265642E";
   }
   //COLcout << Data << newline;
   TST_ASSERT(Data == "This stuff shouldn't be ignored.");
}

static void testDecodeInvalid()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexDecodeFilter TestDecodeFilter(&Sink);

   // test bad hex character odd place
   Data.clear();
   TestDecodeFilter.resetFilter();
   {
      SFITstream FilterStream(TestDecodeFilter);
      TST_ASSERT_EXCEPTION(FilterStream.stream() << "5468697320697320G12074657742e",
         COLerror("Invalid character in hex decoding: 'G'",COLerror::PreCondition));
   }

   // test even bad hex character place
   Data.clear();
   TestDecodeFilter.resetFilter();
   {
      SFITstream FilterStream(TestDecodeFilter);
      TST_ASSERT_EXCEPTION(FilterStream.stream() << "468697320697320H12074657742e",
         COLerror("Invalid character in hex decoding: 'H'",COLerror::PreCondition));
   }

   // no leftover hex byte test
   Data.clear();
   TestDecodeFilter.resetFilter();
   TST_ASSERT(0 == TestDecodeFilter.unprocessedCharacterCount());
   {
      SFITstream FilterStream(TestDecodeFilter);
      FilterStream.stream() << "468697";
   }
   TST_ASSERT(0 == TestDecodeFilter.unprocessedCharacterCount());

   // leftover hex byte test
   Data.clear();
   TestDecodeFilter.resetFilter();
   TST_ASSERT(0 == TestDecodeFilter.unprocessedCharacterCount());
   {
      SFITstream FilterStream(TestDecodeFilter);
      FilterStream.stream() << "4686973";
   }
   TST_ASSERT(1 == TestDecodeFilter.unprocessedCharacterCount());
}

static void testDecodeBinaryData()
{
   const char HexEncodedString[] = "00f0e1d2c3b4a5968778695a4b3c2d1e0f";
   static const unsigned char CorrectBytes[] =
   {
      0x00, 0xf0, 0xe1, 0xd2, 0xc3, 0xb4, 0xa5, 0x96, 0x87, 0x78,
      0x69, 0x5a, 0x4b, 0x3c, 0x2d, 0x1e, 0x0f
   };

   {
      COLstring Data;
      COLsinkString Sink(Data);
      SFIhexDecodeFilter TestDecodeFilter(&Sink);
      SFITstream FilterStream(TestDecodeFilter);
      FilterStream.stream() << HexEncodedString;
      TST_ASSERT(0 == TestDecodeFilter.unprocessedCharacterCount());
      TST_ASSERT(sizeof(CorrectBytes)/sizeof(CorrectBytes[0]) == Data.size());
      TST_ASSERT(0 == memcmp(Data.c_str(),CorrectBytes,sizeof(CorrectBytes)/sizeof(CorrectBytes[0])));
   }
}

static void testChain()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexDecodeFilter DecodeFilter(&Sink);
   SFIhexEncodeFilter EncodeFilter(&DecodeFilter);

   // Test it with a single write
   Data.clear();
   DecodeFilter.resetFilter();
   EncodeFilter.resetFilter();
   {
      SFITstream FilterStream(EncodeFilter);
      FilterStream.stream() << "This is a test.";
   }
   TST_ASSERT(Data == "This is a test.");

   // Test with multiple writes
   Data.clear();
   DecodeFilter.resetFilter();
   EncodeFilter.resetFilter();
   {
      SFITstream FilterStream(EncodeFilter);
      FilterStream.stream() << "This " << "is ";
      FilterStream.stream() << "also a test" << '!';
   }
   TST_ASSERT(Data == "This is also a test!");

   // Lets reverse directions!
   DecodeFilter.setNext(&EncodeFilter);
   COLsinkString Sink2(Data);
   EncodeFilter.setNext(&Sink2);   

   // Test it with a single write
   Data.clear();
   DecodeFilter.resetFilter();
   EncodeFilter.resetFilter();
   {
      SFITstream FilterStream(DecodeFilter);
      FilterStream.stream() << "54686973206973206120746573742e";
   }
   TST_ASSERT(Data == "54686973206973206120746573742E");

   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   DecodeFilter.resetFilter();
   EncodeFilter.resetFilter();
   {
      SFITstream FilterStream(DecodeFilter);
      FilterStream.stream() << "546869732069732" << "0616c73";
      FilterStream.stream() << "6F20612074657374" << 21;
   }   
   TST_ASSERT(Data == "5468697320697320616C736F2061207465737421");
}

static void testValid()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIhexValidFilter TestFilter(&Sink);

   // Just one test in here
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "54this won't show up in it :) \t \n686973206973206120746573742E";
   }
   TST_ASSERT(Data == "54686973206973206120746573742E");
}

TSTtestable* SFIThex()
{
   TST_START_COLLECTION("SFIThex")
      TST_ADD_TEST_FUNCTION(testEncode);
      // TST_ADD_TEST_FUNCTION(testEncodeSpeed);
      TST_ADD_TEST_FUNCTION(testEncodeHigherThan127);
      TST_ADD_TEST_FUNCTION(testDecode);
      TST_ADD_TEST_FUNCTION(testDecodeInvalid);
      TST_ADD_TEST_FUNCTION(testDecodeBinaryData);
      TST_ADD_TEST_FUNCTION(testChain);
      TST_ADD_TEST_FUNCTION(testValid);
   TST_END_FIXTURE
};

