//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIbase64
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

#include "SFITbase64.h"
#include "SFITstream.h"

#include <SFI/SFIbase64EncodeFilter.h>
#include <SFI/SFIbase64DecodeFilter.h>

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
   SFIbase64EncodeFilter TestFilter(&Sink);

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This is a test.";
   }
   TST_ASSERT_EQUALS("VGhpcyBpcyBhIHRlc3Qu", Data);

   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This " << "is ";
      FilterStream.stream() << "also a test" << '!';
   }
   TST_ASSERT_EQUALS("VGhpcyBpcyBhbHNvIGEgdGVzdCE=", Data);

   // Test it by resetting it midstream
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This stuff should be ignored.";
      TestFilter.resetFilter();
      Data.clear();
      FilterStream.stream() << "This stuff shouldn't be ignored.";
   }
   TST_ASSERT_EQUALS("VGhpcyBzdHVmZiBzaG91bGRuJ3QgYmUgaWdub3JlZC4=", Data);

   // Test it with multiple lines and a non-default line-length and line ending
   Data.clear();
   TestFilter.resetFilter();
   {
      COLsinkString Sink(Data);
      SFIbase64EncodeFilter TestFilterWithLineLength(&Sink, 64, "\n");
      SFITstream FilterStream(TestFilterWithLineLength);
      FilterStream.stream() << "0123456789012345678901234567890123456789012345678901234567890123456789";
   }

   TST_ASSERT_EQUALS("MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3\nODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OQ==", Data);


   // Ensure complete last line does not get newline
   Data.clear();
   TestFilter.resetFilter();
   {
      COLsinkString Sink(Data);
      SFIbase64EncodeFilter TestFilterWithLineLength(&Sink, 64, "\n");
      SFITstream FilterStream(TestFilterWithLineLength);
      FilterStream.stream() << "012345678901234567890123456789012345678901234567";
   }

   TST_ASSERT_EQUALS("MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3", Data);

   // Ensure complete last line does not get newline
   Data.clear();
   TestFilter.resetFilter();
   {
      COLsinkString Sink(Data);
      SFIbase64EncodeFilter TestFilterWithLineLength(&Sink, 64, "\n");
      SFITstream FilterStream(TestFilterWithLineLength);
      FilterStream.stream() << "012345678901234567890123456789012345678901234567012345678901234567890123456789012345678901234567";
   }

   TST_ASSERT_EQUALS("MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3\nMDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3", Data);

   // Ensure trailing single char causes newline termination of preceding line.
   Data.clear();
   TestFilter.resetFilter();
   {
      COLsinkString Sink(Data);
      SFIbase64EncodeFilter TestFilterWithLineLength(&Sink, 64, "\n");
      SFITstream FilterStream(TestFilterWithLineLength);
      FilterStream.stream() << "0123456789012345678901234567890123456789012345678";
   }

   TST_ASSERT_EQUALS("MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3\nOA==", Data);


   // Ensure trailing pair of char causes newline termination of preceding line.
   Data.clear();
   TestFilter.resetFilter();
   {
      COLsinkString Sink(Data);
      SFIbase64EncodeFilter TestFilterWithLineLength(&Sink, 64, "\n");
      SFITstream FilterStream(TestFilterWithLineLength);
      FilterStream.stream() << "01234567890123456789012345678901234567890123456789";
   }

   TST_ASSERT_EQUALS("MDEyMzQ1Njc4OTAxMjM0NTY3ODkwMTIzNDU2Nzg5MDEyMzQ1Njc4OTAxMjM0NTY3\nODk=", Data);
}

static void testDecode()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIbase64DecodeFilter TestFilter(&Sink);

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "VGhpcyBpcyBhIHRlc3Qu";
   }
   TST_ASSERT(Data == "This is a test.");

   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "VGhpcyBpcy" << "Bhb";
      FilterStream.stream() << "HNvIGEgdGVzdCE" << '=';
   }
   TST_ASSERT(Data == "This is also a test!");

   // Test it by resetting it midstream
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "VGhpcyBpcyBhbHNv";
      TestFilter.resetFilter();
      Data.clear();
      FilterStream.stream() << "VGhpcyBzdHVmZiBzaG91bGRuJ3QgYmUgaWdub3JlZC4=";
   }
   TST_ASSERT(Data == "This stuff shouldn't be ignored.");
}

static void testChain()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIbase64DecodeFilter DecodeFilter(&Sink);
   SFIbase64EncodeFilter EncodeFilter(&DecodeFilter);

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
      FilterStream.stream() << "VGhpcyBpcyBhIHRlc3Qu";
   }
   TST_ASSERT(Data == "VGhpcyBpcyBhIHRlc3Qu");

   // Test it with multiple writes to ensure buffering is working properly
   Data.clear();
   DecodeFilter.resetFilter();
   EncodeFilter.resetFilter();
   {
      SFITstream FilterStream(DecodeFilter);
      FilterStream.stream() << "VGhpcyBpcy" << "Bhb";
      FilterStream.stream() << "HNvIGEgdGVzdCE" << '=';
   }   
   TST_ASSERT(Data == "VGhpcyBpcyBhbHNvIGEgdGVzdCE=");
}

static void testAllEquals()
{
   
   //this used to cause an allocation failure, see
   //#6166
   COLstring Data;
   COLsinkString Sink(Data);
   SFIbase64DecodeFilter TestFilter(&Sink);

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "====";
   }
   TST_ASSERT(Data == "");

   // Test it with a single write
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      TST_ASSERT_EXCEPTION(FilterStream.stream() << "some bad data =-==================%^&*#$%@#$to decrypt",
         COLerror("Invalid '=' in decoding Base64 stream",COLerror::PreCondition) );
   }
}


TSTtestable* SFITbase64()
{
   TST_START_COLLECTION("SFITbase64")
      TST_ADD_TEST_FUNCTION(testEncode);
      TST_ADD_TEST_FUNCTION(testDecode);
      TST_ADD_TEST_FUNCTION(testChain);
      TST_ADD_TEST_FUNCTION(testAllEquals)
   TST_END_FIXTURE
};
