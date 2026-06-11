//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTTtestEncodingFilter
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Fri 12 Dec 2014 17:41:04 EST
//---------------------------------------------------------------------------
#include "TXTTprecomp.h"
#pragma hdrstop

#include "TXTTtestEncodingFilter.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TXT/TXTencodingFilter.h>

// n.b. Euro sign works really well for testing since it requires 3 bytes in UTF-8 and 1 byte in CP1252/ISO 8859-15

static void testTrivialConversion() {
   COL_FUNCTION(testTrivialConversion);
   // The data is provided as ASCII, so it should be trivial to convert to UTF-8
   COLstring Data = "Hello world";
   COLstring Result;

   TST_ASSERT_NO_EXCEPTION(Result = TXTencode(Data, "UTF-8", "UTF-8"));
   TST_ASSERT_EQUALS_MESSAGE(Data, Result, "Encoding from UTF-8 to UTF-8 should be trivial, and return the same data.");

   TST_ASSERT_NO_EXCEPTION(Result = TXTencode(Data, "ASCII", "UTF-8"));
   TST_ASSERT_EQUALS_MESSAGE(Data, Result, "Encoding from ASCII to UTF-8 should be trivial, and return the same data.");
}

static void testExpandingConversion() {
   COL_FUNCTION(testExpandingConversion);
   // This case involves converting an encoded which uses less bytes to an encoding that uses more bytes for the given data sample
   // In ISO 8859-15, the Euro symbol is only one byte, but in UTF-8, it is 3 bytes
   static const int InitialSize = 1 << 22;
   COLstring Data = COLstring(InitialSize, '\xA4');
   COLstring Expected = COLstring(InitialSize * 3, '\0');
   for (int i = 0; i < InitialSize; i++) {
      Expected[3*i]   = '\xE2';
      Expected[3*i+1] = '\x82';
      Expected[3*i+2] = '\xAC';
   }
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "ISO-8859-15", "UTF-8");

   TST_ASSERT_NO_EXCEPTION(Filter.write(Data.c_str(), InitialSize));
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   TST_ASSERT_EQUALS(Expected, Result);
}

static void testContractingConversion() {
   COL_FUNCTION(testContractingConversion);
   // Similar to Expanding, but going from UTF-8 -> ISO 8859-15
   static const int InitialSize = 3 << 22;
   COLstring Data = COLstring(InitialSize, '\0');
   for (int i = 0; i < InitialSize; i += 3) {
      Data[i]   = '\xE2';
      Data[i+1] = '\x82';
      Data[i+2] = '\xAC';
   }
   COLstring Expected = COLstring(InitialSize / 3, '\xA4');
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "ISO-8859-15");

   TST_ASSERT_NO_EXCEPTION(Filter.write(Data.c_str(), InitialSize));
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   TST_ASSERT_EQUALS(Expected, Result);
}

static void testIncompleteData() {
   COL_FUNCTION(testIncompleteData);
   // This data is the beginning of a multi-byte sequence in UTF-8... but it does not contain the remainder of the bytes
   // So trying to convert it from UTF-8 into anything else should result in an error
   COLstring Data = "\xE0\x80";  // should have 3 bytes to decode an Ex sequence!
   COLstring Result;

   TST_ASSERT_EXCEPTION(Result = TXTencode(Data, "UTF-8", "UTF-8"),
                        COLerror("Encoding conversion is expecting more characters.", COLerror::SystemError));
}

static void testInvalidMultiByte() {
   COL_FUNCTION(testInvalidMultiByte);
   // This data is starts a UTF-8 multibyte, but doesn't proceed with the expected bit
   // So trying to convert it from UTF-8 into anything else should result in an error
   COLstring Data = COL_T("\xE0\x00\x00");  //careful when embedding nulls!
   COLstring Result;

   TST_ASSERT_EXCEPTION(Result = TXTencode(Data, "UTF-8", "UTF-8"),
                        COLerror("Illegal byte sequence encountered at position 0. Showing 3 bytes: E0 0 0", COLerror::SystemError));
}

static void testChunkedMultiByte() {
   COL_FUNCTION(testChunkedMultiByte);
   // Write a valid multi-byte into the filter, one byte at a time
   // The filter should be capable of buffering the bytes until more bytes are available to convert
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "UTF-8");
   COLostream Stream(Filter); // just for the convenience
   // Euro sign = E2 82 AC
   TST_ASSERT_NO_EXCEPTION(Stream << '\xE2');
   TST_ASSERT_NO_EXCEPTION(Stream << '\x82');
   TST_ASSERT_NO_EXCEPTION(Stream << '\xAC');
   TST_ASSERT_NO_EXCEPTION_MESSAGE(Filter.onEndStream(), "A full multi-byte sequence was fed in, so no errors should have occurred");
   TST_ASSERT_EQUALS_MESSAGE("\xE2\x82\xAC", Result, "Bytes should not have been lost during a trivial conversion from UTF-8 to UTF-8");
}

static void testBigChunk() {
   COL_FUNCTION(testBigChunk);
   // Throwing a lot of data in at once should not pose a problem to an encoding filter
   static const int StringSize = 1 << 22;
   COL_TRC("Writing chunk of size " << StringSize);
   COLstring Data = COLstring(StringSize, 'a');
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "UTF-8");

   TST_ASSERT_NO_EXCEPTION(Filter.write(Data.c_str(), StringSize));
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   TST_ASSERT_EQUALS(Data, Result);
}

static void testSmallChunks() {
   COL_FUNCTION(testSmallChunks);
   // Throwing in little bits of data at a time (incrementally) should not pose a problem to an encoding filter
   static const int NumChunks = 1 << 16;
   static const int ChunkSize = 1 << 6;
   COLstring Data = COLstring(ChunkSize * NumChunks, 'a');
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "UTF-8");

   for (int i = 0; i < NumChunks; i++) {
      if ((i * 4) % NumChunks == 0) {  // i % (1 << 13)
         // Don't print too much...
         COL_TRC("Progress: writing chunk " << i << " of " << NumChunks << "...");
      }
      // Careful here - Do the arithmetic before converting to void*
      TST_ASSERT_NO_EXCEPTION(Filter.write((Data.c_str() + i * ChunkSize), ChunkSize));
   }
   COL_TRC("Done writing all chunks");
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   TST_ASSERT_EQUALS(Data, Result);
}

static void testResetFilter() {
   COL_FUNCTION(testResetFilter);
   // Start throwing in a multi-byte, but then cancel (reset) and start another multi-byte
   // That should be supported by the filter - Should not trigger incomplete/illegal exceptions
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "UTF-8");
   COLostream Stream(Filter); // just for the convenience

   TST_ASSERT_NO_EXCEPTION(Stream << '\xE0');      // start a 3 byte sequence
   TST_ASSERT_NO_EXCEPTION(Filter.resetFilter());  // whoops, nevermind!
   TST_ASSERT_NO_EXCEPTION(Stream << '\xE2');      // start another 3-byte sequence: would count as an illegal sequence if we started a 3-byte sequence twice
   TST_ASSERT_NO_EXCEPTION(Stream << "\x82\xAC");
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   // The initial 0xE0 should not be in the result, since it never formed a complete multi-byte
   TST_ASSERT_EQUALS("\xE2\x82\xAC", Result);
}

static void testUnprocessedCharacterCount() {
   COL_FUNCTION(testUnprocessedCharacterCount);
   COLstring Result;
   // Do some setup to write the data into the filter
   COLsinkString Sink(Result);
   TXTencodingFilter Filter(&Sink, "UTF-8", "UTF-8");
   COLostream Stream(Filter); // just for the convenience

   // Write a 4-byte multi-byte sequence, and check the unprocessed character count at each step
   COL_TRC("First byte going in");
   TST_ASSERT_NO_EXCEPTION(Stream << '\xF0');
   TST_ASSERT_EQUALS(1, Filter.unprocessedCharacterCount());
   COL_TRC("Second byte going in");
   TST_ASSERT_NO_EXCEPTION(Stream << '\x9D');
   TST_ASSERT_EQUALS(2, Filter.unprocessedCharacterCount());
   COL_TRC("Third byte going in");
   TST_ASSERT_NO_EXCEPTION(Stream << '\x91');
   TST_ASSERT_EQUALS(3, Filter.unprocessedCharacterCount());
   COL_TRC("Last byte going in");
   TST_ASSERT_NO_EXCEPTION(Stream << '\x8E');
   TST_ASSERT_EQUALS(0, Filter.unprocessedCharacterCount());
   TST_ASSERT_NO_EXCEPTION(Filter.onEndStream());
   TST_ASSERT_EQUALS("\xF0\x9D\x91\x8E", Result);
}

static void testUtf16Conversion() {
   // This one is especially interesting becaues Windows filenames are saved in UTF-16
   COL_FUNCTION(testUtf16);
   COLstring Letter = "\xFE\xFF\x0A\x0a"; // Gurmukhi letter UU (ਊ)
   COLstring ReWritten = TXTencode(Letter, "UTF-16", "UTF-8");
   TST_ASSERT_EQUALS("ਊ", ReWritten);
   COLstring ReReWritten = TXTencode(ReWritten, "UTF-8", "UTF-16");
   TST_ASSERT_EQUALS(Letter, ReReWritten);
}

TSTtestable* TXTTtestEncodingFilter() {
   TST_START_COLLECTION("TXTTtestEncodingFilter")
      TST_ADD_TEST_FUNCTION(testTrivialConversion);
      TST_ADD_TEST_FUNCTION(testExpandingConversion);
      TST_ADD_TEST_FUNCTION(testContractingConversion);
      TST_ADD_TEST_FUNCTION(testIncompleteData);
      TST_ADD_TEST_FUNCTION(testInvalidMultiByte);
      TST_ADD_TEST_FUNCTION(testChunkedMultiByte);
      TST_ADD_TEST_FUNCTION(testBigChunk);
      TST_ADD_TEST_FUNCTION(testSmallChunks);
      TST_ADD_TEST_FUNCTION(testResetFilter);
      TST_ADD_TEST_FUNCTION(testUnprocessedCharacterCount);
      TST_ADD_TEST_FUNCTION(testUtf16Conversion);
   TST_END_FIXTURE
}
