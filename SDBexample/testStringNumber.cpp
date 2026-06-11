// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testDir
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Mon 13 Feb 2023 10:01:00 EST
// ---------------------------------------------------------------------------
#include "testDir.h"
#include <map>

#include <UNIT/UNITapp.h>

#include <SDB/SDBdir.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBstring.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testString(){
   COL_FUNCTION(testString);
   COLstring S1 = "The quick brown fox ran after Puff";
   COLstring Buffer;
   Buffer.setCapacity(SDBnumberSize(S1.size()) + S1.size());
   int Amount = SDBwriteString(S1, Buffer.get_buffer());
   COLstring S2;
   SDBreadString(&S2, Buffer.get_buffer(), Amount);
   COL_VAR2(S1, S2);
   UNIT_ASSERT_EQUALS(S1,S2);
}

static void testNumber(){
   COL_FUNCTION(testNumber);
   COLint64 X1 = 124124123412342142ull;
   COLstring Out;
   int Written = SDBwriteNumber(X1, Out.get_buffer());
   Out.setSize(Written);
   COL_VAR(Out.size());
   COLuint64 X2;
   int Read = SDBreadNumber(&X2, Out.c_str(), Out.size());
   COL_VAR2(X1, X2);
   UNIT_ASSERT_EQUALS(X1, X2);
}

static void SDBTrunWriteNumberTest(COLuint64 Challenge, const COLstring& Answer) {
   static COLstring Buffer;
   int Amount = SDBwriteNumber(Challenge, Buffer.get_buffer());
   Buffer.setSize(Amount);
   UNIT_ASSERT_EQUALS(Answer, Buffer);
   UNIT_ASSERT_EQUALS(Answer.size(), SDBnumberSize(Challenge));
}

static void SDBTrunReadNumberTest(const COLstring& Challenge, COLuint64 Answer) {
   COLuint64 Value;
   int Amount = SDBreadNumber(&Value, Challenge.data(), Challenge.size());
   UNIT_ASSERT_EQUALS(Answer, Value);
   UNIT_ASSERT_EQUALS(Amount, SDBnumberSize(Answer));
}

static void SDBTrunReadNumberErrorTest(const COLstring& Challenge, int ErrorCode) {
   COLuint64 Value;
   UNIT_ASSERT_EQUALS(ErrorCode, SDBreadNumber(&Value, Challenge.data(), Challenge.size()));
}

static void testVarWidthNumber() {
   COL_FUNCTION(testVarWidthNumber);
   COLuint64 Value;
   SDBreadNumber(&Value, "\xFF\x00", 2);
   // return;
   std::map<COLuint64,COLstring> Tests = {
      {0, COL_T("\x00")},
      {1, "\x01"},
      {127, "\x7F"},
      {128, "\x80\x01"},
      {129, "\x81\x01"},
      {255, "\xFF\x01"},
      {256, "\x80\x02"},
      {65535, "\xFF\xFF\x03"},
      {65536, "\x80\x80\x04"},
      {4294967295, "\xFF\xFF\xFF\xFF\x0F"},
      {4294967296, "\x80\x80\x80\x80\x10"},
      {COL_UINT_64_MAX, "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\x01"}
   };
   for (auto Test = Tests.cbegin(); Test !=Tests.cend(); Test++) {
      SDBTrunWriteNumberTest(Test->first, Test->second);
      SDBTrunReadNumberTest(Test->second, Test->first);
   }
   // addition read error tests
   // doesn't have to be \xFF, as long as it's sequence of bytes whose MSB is 1.
   for (int i = 1; i <= 9; ++i) {
      COLstring Test(i, '\xFF');
      SDBTrunReadNumberErrorTest(Test, -1);  // "Need more data"
      Test.write("\x00", 1);
      SDBTrunReadNumberErrorTest(Test, -2);  // "Not a number. Expecting another byte"
   }
   for (int i = 10; i < 12; ++i) {  // "Not a number. Must be 10 bytes or less"
      SDBTrunReadNumberErrorTest(COLstring(i, '\xFF'), -3);
   }
}

void testStringNumber(UNITapp* pApp){
   COL_FUNCTION(testStringNumber);
   pApp->add("number/readwrite", &testNumber);
   pApp->add("string/readwrite", &testString);
   pApp->add("number/var/width", &testVarWidthNumber);
}
