//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLmath.cpp
//
// Description:
//
// This is a test of COLmath
//
// Author: Craig Burrell
// Date:   Mon 02/21/2005
//
//---------------------------------------------------------------------------

#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLmath.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testCOLmathRandom() {
#ifdef _WIN32
   TST_ASSERT_NO_EXCEPTION(COLmath::randomNumber(32767));   // make sure we are not using ::rand() on windows
   TST_ASSERT_NO_EXCEPTION(COLmath::randomNumber(UINT_MAX));// we support anything <= UINTMAX
#else
   TST_ASSERT_NO_EXCEPTION(COLmath::randomNumber(RAND_MAX)); // make sure RAND_MAX itself is supported
#endif
   TST_ASSERT_NO_EXCEPTION(COLmath::randomNumber(888888));
}

void testCOLmathPower()
{
   {
      COLint64 Result = COLmath::power(25, 5);
      TST_ASSERT(Result == 9765625);
   }

   {
      COLint64 Result = COLmath::power(-25, 5);
      TST_ASSERT(Result == -9765625);
   }

   {
      COLint64 Result = COLmath::power(25, 1);
      TST_ASSERT(Result == 25);
   }

   {
      COLint64 Result = COLmath::power(25, 0);
      TST_ASSERT(Result == 1);
   }

   {
      COLint64 Result = COLmath::power(0, 25);
      TST_ASSERT(Result == 0);
   }

   {
      COLint64 Result = COLmath::power(0, 0);
      TST_ASSERT(Result == 1);
   }

  /* try
   {
      COL_TRC("Negative exponent test...");
      COLint64 Result = COLmath::power(25, -1);
   }
   catch(COLerror&)
   {
      // we expected to get this error
      COL_TRC("caught the problem ok." << newline);
   }*/


}


// This is a test of our custom implementation of atoi64 on
// HPUX and AIX platforms.  It can't hurt to run it on all
// platforms, though.
void testCOLmathAtoi64()
{

   {
      COLstring TestString("9223372036854775807");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == COL_INT_64_MAX );
   }

   // negative-values tests
   {
      COLstring TestString("-1");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == -1 );
   }

   {
      COLstring TestString("-9223372036854775807");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == -COL_INT_64_MAX );
   }

   // whitespace tests
   {
      COLstring TestString("    -1");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == -1 );
   }

   {
      COLstring TestString("   9223372036854775807");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == COL_INT_64_MAX );
   }

   {
      COLstring TestString("-  1");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == 0 );
   }

   // irregular-form tests
   {
      COLstring TestString("-100-2");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == -100 );
   }

   {
      COLstring TestString("543-1");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == 543 );
   }

   {
      COLstring TestString("-");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == 0 );
   }

   {
      COLstring TestString("   922337 2036854775807");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == 922337 );
   }

   {
      COLstring TestString("   922337@2036854775807");
      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
      TST_ASSERT( TestResult == 922337 );
   }

   // out-of-range test
   // TODO: the behaviour here seems to be non-standard across platforms:
   // _WIN32 returns -1, but Linux returns COL_INT_64_MAX... hard to test
//   {
//      COLstring TestString("18446744073709551615");
//      COLint64 TestResult = COLmath::atoi64(TestString.c_str());
//      TST_ASSERT( TestResult == -1);
//   }

}


void testCOLmathParseUint64()
{

   {
      COLstring TestString("0");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(0, TestResult);
   }

   {
      COLstring TestString("123456789");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(123456789, TestResult);
   }

   {
      COLstring TestString("001234567890");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(1234567890, TestResult);
   }

   // Maximum 64-bit integer (0xFFFFFFFFFFFFFFFF)
   {
      COLstring TestString("18446744073709551615");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX, TestResult);
   }

   // Almost the maximum
   {
      COLstring TestString("18446744073709551614");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX-1, TestResult);
   }

   // Too large
   {
      COLstring TestString("18446744073709551616");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX, TestResult);
   }

   // Too large
   {
      COLstring TestString("28446744073709551615");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX, TestResult);
   }

   // Way too large
   {
      COLstring TestString("1879568098960298778934789547980543780943278045");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX, TestResult);
   }

   // Way too large
   {
      COLstring TestString("0879568098960298778934789547980543780943278045");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(COL_UINT_64_MAX, TestResult);
   }

   // Negative numbers not supported
   {
      COLstring TestString("-1");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(0, TestResult);
   }

   // Spaces not supported
   {
      COLstring TestString("    356");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(0, TestResult);
   }

   // Contains invalid character
   {
      COLstring TestString("75747:374574");
      COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
      TST_ASSERT_EQUALS(0, TestResult);
   }

   // Parse integers 0 through 1000
   {
      COLstring TestString;
      for (COLuint64 IntegerIndex = 0; IntegerIndex <= 1000; ++IntegerIndex)
      {
         TestString.clear();
         COLostream (TestString) << IntegerIndex;
         COLuint64 TestResult = COLmath::parseUint64(TestString.c_str(), TestString.size());
         TST_ASSERT_EQUALS(IntegerIndex, TestResult);
      }
   }

}

void testCOLmathAtousParse0to0() {
   COLstring TestString("0");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

void testCOLmathAtousParse1to1() {
   COLstring TestString("1");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(1, TestResult);
}

void testCOLmathAtousParse00890to89() {
   COLstring TestString("00890");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(890, TestResult);
}

// Maximum 16-bit integer (0xFFFF)
void testCOLmathAtousParseMaximum16bitInt() {
   COLstring TestString("65535");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(COL_UINT_16_MAX, TestResult);
}

// Almost the maximum
void testCOLmathAtousParseAlmostMaximum16bitInt() {
   COLstring TestString("65534");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(COL_UINT_16_MAX-1, TestResult);
}

// Too large
void testCOLmathAtousRejectOverMaximum16bitIntBy1() {
   COLstring TestString("65536");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

// Too large
void testCOLmathAtousRejectOverMaximum16bitIntByMore() {
   COLstring TestString("28446744073709551615");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

// Way too large
void testCOLmathAtousRejectOverMaximum16bitIntByWayMore() {
   COLstring TestString("1879568098960298778934789547980543780943278045");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

// Negative numbers not supported
void testCOLmathAtousRejectNegative1() {
   COLstring TestString("-1");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

// Negative numbers not supported
void testCOLmathAtousRejectNegative100() {
   COLstring TestString("-100");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(0, TestResult);
}

// Contains invalid character
void testCOLmathAtousStopParsingAfterInvalidCharacter() {
   COLstring TestString("7d37");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(7, TestResult);
}

// Contains invalid character
void testCOLmathAtousStopParsingAfterInvalidSpecialCharacter() {
   COLstring TestString("7:37");
   COLuint16 TestResult = COLmath::atous(TestString.c_str());
   TST_ASSERT_EQUALS(7, TestResult);
}

// Parse integers 0 through 1000
void testCOLmathAtousSParse1to1000() {
   COLstring TestString;
   for (COLuint16 IntegerIndex = 0; IntegerIndex <= 1000; ++IntegerIndex)
   {
      TestString.clear();
      COLostream (TestString) << IntegerIndex;
      COLuint16 TestResult = COLmath::atous(TestString.c_str());
      TST_ASSERT_EQUALS(IntegerIndex, TestResult);
   }
}

TSTtestable* COLmathTests()
{
   TST_START_COLLECTION("COLmathTests")
      TST_ADD_TEST_FUNCTION(testCOLmathRandom);
      TST_ADD_TEST_FUNCTION(testCOLmathPower);
      TST_ADD_TEST_FUNCTION(testCOLmathAtoi64);
      TST_ADD_TEST_FUNCTION(testCOLmathParseUint64);

      TST_ADD_COLLECTION("COLmathAtousTests");
         TST_ADD_TEST_FUNCTION(testCOLmathAtousParse0to0);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousParse1to1);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousParse00890to89);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousParseMaximum16bitInt);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousParseAlmostMaximum16bitInt);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousRejectOverMaximum16bitIntBy1);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousRejectOverMaximum16bitIntByMore);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousRejectOverMaximum16bitIntByWayMore);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousRejectNegative1);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousRejectNegative100);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousStopParsingAfterInvalidCharacter);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousStopParsingAfterInvalidSpecialCharacter);
         TST_ADD_TEST_FUNCTION(testCOLmathAtousSParse1to1000);
      TST_END_ADD_COLLECTION
   TST_END_COLLECTION
}