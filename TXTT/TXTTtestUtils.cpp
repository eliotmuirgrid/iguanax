//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTTtestUtils
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Wednesday, June 23rd, 2010 @ 04:27:36 PM
//
//---------------------------------------------------------------------------
#include "TXTTprecomp.h"
#pragma hdrstop

#include "TXTTtestUtils.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TXT/TXTutils.h>

#include <stdlib.h>


void testNaturalCompareStrings()
{
   static const struct {
      const char *pLeft, *pRight;
      int Compare;
   } Tests[] = {
      { "Alpha 123", "Alpha 123", 0 },
      { "Beta", "BETA", 0 },
      { "Gamma 100", "Gamma 9", 1 },
      { "Delta 0010", "DELTA 10", 0 },
      { "Epsilon -9", "Epsilon -10", -1 },  // It failed math class.
      { 0 }
   };

   for(int i=0; Tests[i].pLeft; i++)
   {
      int Compare = TXTnaturalCompareStrings(Tests[i].pLeft, Tests[i].pRight);

      if(Compare)
         Compare = Compare / abs(Compare);

      TST_ASSERT_EQUALS_MESSAGE(Tests[i].Compare, Compare,
         Tests[i].pLeft << " <=> " << Tests[i].pRight);
   }
}


static const struct {
   const char *pBefore, *pAfter;
} s_Tests[] = {

   //
   // Will Convert
   //

   // Regular delimiters
   { "MSH|^~\\&|TEST-A1|\\x80\\|\\X80\\",
     "MSH|^~\\&|TEST-A1|\x80|\x80" },

   // Leading whitespace
   { " \r\n\v\f MSH|^~\\&|TEST-A2|\\x80\\|\\X80\\",
     " \r\n\v\f MSH|^~\\&|TEST-A2|\x80|\x80" },

   // Alternate delimiters
   { "MSH/%$*-/TEST-A3/*x80*/*X80*",
     "MSH/%$*-/TEST-A3/\x80/\x80" },

   // Mixed with incomplete escapes
   { "MSH|^~\\&|TEST-A4|\\\\x80\\|\\x\\X80\\|\\x8\\x80\\",
     "MSH|^~\\&|TEST-A4|\\\x80|\\x\x80|\\x8\x80" },

   //
   // Won't Convert
   //

   // 7-bit ASCII escapes
   { "MSH|^~\\&|TEST-B1|\\x20\\|\\X20\\",
     "MSH|^~\\&|TEST-B1|\\x20\\|\\X20\\" },

   // Incomplete escapes
   { "MSH|^~\\&|TEST-B2|\\x80|\\X80|\\x8\\|\\x8|\\",
     "MSH|^~\\&|TEST-B2|\\x80|\\X80|\\x8\\|\\x8|\\" },

   // Invalid hexadecimal values
   { "MSH|^~\\&|TEST-B3|\\x8G\\|\\X8G\\",
     "MSH|^~\\&|TEST-B3|\\x8G\\|\\X8G\\" },

   // Missing delimiter
   { "MSH|^~\\|TEST-B4|\\x80\\|\\X80\\",
     "MSH|^~\\|TEST-B4|\\x80\\|\\X80\\" },

   // Duplicate delimiters
   { "MSH|^~\\^|TEST-B5|\\x80\\|\\X80\\",
     "MSH|^~\\^|TEST-B5|\\x80\\|\\X80\\" },

   // Extra delimiter
   { "MSH|^~\\&$|TEST-B6|\\x80\\|\\X80\\",
     "MSH|^~\\&$|TEST-B6|\\x80\\|\\X80\\" },

   // Invalid delimiters
   { "MSH|BORK|TEST-B7|Rx80R|RX80R",
     "MSH|BORK|TEST-B7|Rx80R|RX80R" },

   { 0 }
};

void testUnescapeExtendedAscii()
{
   for(int i=0; s_Tests[i].pBefore; ++i)
   {
      COLstring String = s_Tests[i].pBefore;
      TXTunescapeExtendedAscii(String);
      TST_ASSERT_EQUALS(s_Tests[i].pAfter, String);
   }
}

void testEscapeExtendedAscii()
{
   // Test without escape character detection.
   static const char Before[] = "\x86 *X42* \x7F \xA2\xB3 *X00* \xED";
   static const char After[]  = "*X86* *X42* \x7F *XA2**XB3* *X00* *XED*";

   COLstring String = Before;
   TXTescapeExtendedAscii(String, '*');
   TST_ASSERT_EQUALS(After, String);

   // Test with detection.
   for(int i=0; s_Tests[i].pBefore; ++i)
   {
      COLstring Expected(s_Tests[i].pBefore);
      String = s_Tests[i].pAfter;
      TXTescapeExtendedAscii(String);
      
      // HPUX's toupper() modifies some extended characters--I'm not
      // sure what locale it's in.  To avoid locale issues, we need 
      // to make these uppercase after escaping them.
      //
      Expected.toUpperCase();
      String.toUpperCase();

      TST_ASSERT_EQUALS(Expected, String);
   }
}

TSTtestable* TXTTtestUtils()
{
   TST_START_COLLECTION("TXTTtestUtils")
      TST_ADD_TEST_FUNCTION(testNaturalCompareStrings);
      TST_ADD_TEST_FUNCTION(testUnescapeExtendedAscii);
      TST_ADD_TEST_FUNCTION(  testEscapeExtendedAscii);
   TST_END_FIXTURE
};

