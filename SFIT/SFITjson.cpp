//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIjson
//
// Description:
//
// Implementation
//
// Author: Andrew Vajoczki
// Date:   2009-02-11
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITjson.h"

#include <SFI/SFIjsonEscape.h>

#include <memory.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

static const char TestOriginalText[] = "\"An \"'object'\" is an unordered set of name/value pairs. An `object' begins with { (left brace) and ends with } (right brace). Each name is followed by : (colon) and the name/value pairs are separated by , (comma). An array is an ordered collection of values. An array be gins with [ (left bracket) and ends with ] (right bracket). Values are separated by , (comma). A value can be a string in double quotes, or a number, or true or false or null, or an object or an array. These structures can be nested. A string is a collection of zero or more Unicode characters, wrapped in dou ble quotes, using backslash escapes. A character is represented as a single character string. A string is very much like a C or Java string. A number is very much like a C or Java number, except that the oc tal and hexadecimal formats are not used. Whitespace can be inserted between any pair of tokens. Except ing a few encoding details, that completely describes the language./\\\b\f\n\r\t\"";

static int TestOriginalTextLength = sizeof(TestOriginalText) - 1; // subtract 1 for nil

static const char TestJsonText[] = "\\\"An \\\"'object'\\\" is an unordered set of name/value pairs. An `object' begins with { (left brace) and ends with } (right brace). Each name is followed by : (colon) and the name/value pairs are separated by , (comma). An array is an ordered collection of values. An array be gins with [ (left bracket) and ends with ] (right bracket). Values are separated by , (comma). A value can be a string in double quotes, or a number, or true or false or null, or an object or an array. These structures can be nested. A string is a collection of zero or more Unicode characters, wrapped in dou ble quotes, using backslash escapes. A character is represented as a single character string. A string is very much like a C or Java string. A number is very much like a C or Java number, except that the oc tal and hexadecimal formats are not used. Whitespace can be inserted between any pair of tokens. Except ing a few encoding details, that completely describes the language./\\\\\\b\\f\\n\\r\\t\\\"";

static int TestJsonTextLength = sizeof(TestJsonText) - 1; // subtract 1 for nil

static void testEncode()
{
   COLstring Data;

   Data.clear();
   const COLstring BoringText = "Some Boring Text Wihout interesting characters";
   SFIjsonEscape(BoringText.c_str(), BoringText.size(), Data);
   TST_ASSERT_EQUALS(BoringText, Data);
   TST_ASSERT_EQUALS(Data.size(), BoringText.size());

   Data.clear();
   const COLstring LessBoringText = "\"Some/Less\nBoring\rText\"!";
   const COLstring LessBoringJson = "\\\"Some/Less\\nBoring\\rText\\\"!";
   SFIjsonEscape(LessBoringText.c_str(), LessBoringText.size(), Data);
   TST_ASSERT_EQUALS(LessBoringJson, Data);
   TST_ASSERT_EQUALS(Data.size(), LessBoringJson.size());

   Data.clear();
   SFIjsonEscape(TestOriginalText, TestOriginalTextLength, Data);
   TST_ASSERT_EQUALS(TestJsonText, Data);
   TST_ASSERT_EQUALS(Data.size(), TestJsonTextLength);

   Data.clear();
   const COLstring TwiceTestJsonText = COLstring(TestJsonText) + TestJsonText;
   SFIjsonEscape(TestOriginalText, TestOriginalTextLength, Data);
   SFIjsonEscape(TestOriginalText, TestOriginalTextLength, Data);
   TST_ASSERT_EQUALS(TwiceTestJsonText, Data);
   TST_ASSERT_EQUALS(Data.size(), TwiceTestJsonText.size());
}

static void testEncodeSpeed()
{
   const int Iterations = 1000;
   for (int Trials = 0; Trials < Iterations; ++Trials)
   {
      COLstring Data;
      for (int i = 0; i < Iterations; ++i)
      {
         SFIjsonEscape(TestOriginalText, TestOriginalTextLength, Data);
      }
      TST_ASSERT(Data.size() == TestJsonTextLength*Iterations);
   }
}

TSTtestable* SFITjson()
{
   TST_START_COLLECTION("SFITjson")
      TST_ADD_TEST_FUNCTION(testEncode);
      //TST_ADD_TEST_FUNCTION(testEncodeSpeed);
   TST_END_FIXTURE
};
