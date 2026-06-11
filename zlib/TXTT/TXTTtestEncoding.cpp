//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTTtestEncoding
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Thursday, June 11th, 2009 @ 03:27:08 PM
//
//---------------------------------------------------------------------------
#include "TXTTprecomp.h"
#pragma hdrstop

#include "TXTTtestEncoding.h"

#include <COL/COLhashmap.h>
#include <COL/COLarray.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <TXT/TXTencoding.h>


static void basicTest() {
   COL_FUNCTION(basicTest);

   TXTencoding Default;
   TST_ASSERT(Default.isDefault());
   TST_ASSERT(Default.isSupported());

#ifdef _WIN32
   TST_ASSERT_EQUALS("Western (Windows-1252)", Default.description());
#else
   TST_ASSERT_EQUALS("Western (ISO-8859-1)", Default.description());
#endif
}

static void utf8Test() {
   COL_FUNCTION(utf8Test);

   TXTencoding Latin1;
   TXTencoding Utf8 = TXTencoding::utf8();
   TST_ASSERT(Utf8.isSupported());
   TST_ASSERT_EQUALS("Unicode (UTF-8)", Utf8.description());
   const char InLatin1[] = "Fran\xE7ois";  // Francois.
   const char InUtf8[]   = "Fran\xC3\xA7ois";

   COLstring Result;

   // UTF-8 to Latin-1

   Latin1.convertFromUtf8(Result, InUtf8, sizeof InUtf8 - 1);
   TST_ASSERT_EQUALS(InLatin1, Result);

   Latin1.convertFrom(Result, InUtf8, sizeof InUtf8 - 1, Utf8);
   TST_ASSERT_EQUALS(InLatin1, Result);

   Utf8.convertTo(Result, InUtf8, sizeof InUtf8 - 1, Latin1);
   TST_ASSERT_EQUALS(InLatin1, Result);

   // Latin-1 to UTF-8

   Latin1.convertToUtf8(Result, InLatin1, sizeof InLatin1 - 1);
   TST_ASSERT_EQUALS(InUtf8, Result);

   Latin1.convertTo(Result, InLatin1, sizeof InLatin1 - 1, Utf8);
   TST_ASSERT_EQUALS(InUtf8, Result);

   Utf8.convertFrom(Result, InLatin1, sizeof InLatin1 - 1, Latin1);
   TST_ASSERT_EQUALS(InUtf8, Result);
   
}

static void utf16Test() {
   COL_FUNCTION(utf16Test);
   TXTencoding YouSixteen("UTF-16");
   COLstring Letter16 = "\xFE\xFF\x0A\x0a"; // Gurmukhi letter UU (ਊ) in UTF-16
   COLstring Letter8  = "\xE0\xA8\x8A";     // Same thing in UTF-8
   COLstring ReWritten;
   YouSixteen.convertToUtf8(ReWritten, Letter16.c_str(), Letter16.size());
   TST_ASSERT_EQUALS(ReWritten, "ਊ");  // Depending on your editor you may not see the letter here.
   TST_ASSERT_EQUALS(ReWritten, Letter8);

   COLstring ReReWritten;
   YouSixteen.convertFromUtf8(ReReWritten, ReWritten.c_str(), ReWritten.size());
   TST_ASSERT_EQUALS(ReReWritten, Letter16);
}

static void unknownEncodingTest()
{
   TXTencoding Klingon("Klingon");
   TST_ASSERT(!Klingon.isSupported());  // :-(
}


static void knownEncodingsTest()
{
   COLarray<TXTencoding> AllEncodings;
   TXTencoding::getBuiltInEncodings(AllEncodings);

   bool FoundDefault = false;
   COLhashmap<COLstring,bool> SeenName;

   for(TXTencoding* pEncoding=AllEncodings.begin(), *pEnd=AllEncodings.end(); pEncoding != pEnd; ++pEncoding)
   {
      const COLstring EncodingName = pEncoding->name();

      TST_ASSERT( !SeenName[EncodingName] );
      SeenName[EncodingName] = true;

      TST_ASSERT(pEncoding->isSupported());

      if( pEncoding->isDefault() )
      {
         TST_ASSERT(!FoundDefault);
         FoundDefault = true;
      }
   }

   TST_ASSERT(FoundDefault);
}


#define UTF8_REPLACEMENT "\xEF\xBF\xBD"  // U+FFFD
static void badCharacters()
{
   TXTencoding Latin1;
   TXTencoding ShiftJIS("sjis");

   COLstring Result;

   Latin1.convertFromUtf8(Result, "\xFF", 1);
   TST_ASSERT_EQUALS("?", Result);

   if(ShiftJIS.isSupported())
   {
      ShiftJIS.convertToUtf8(Result, "\xFF", 1);
      TST_ASSERT_EQUALS(UTF8_REPLACEMENT, Result);
   }

#ifdef _WIN32
   TXTencoding Cp1252("Windows-1252");
   TST_ASSERT(Cp1252.isSupported());

   Cp1252.convertToUtf8(Result, "\x8F", 1);
   TST_ASSERT_EQUALS(UTF8_REPLACEMENT, Result);
#endif
}


TSTtestable* TXTTtestEncoding()
{
   TST_START_COLLECTION("TXTTtestEncoding")
      TST_ADD_TEST_FUNCTION(basicTest);
      TST_ADD_TEST_FUNCTION(utf8Test);
      TST_ADD_TEST_FUNCTION(unknownEncodingTest);
      TST_ADD_TEST_FUNCTION(knownEncodingsTest);
      TST_ADD_TEST_FUNCTION(badCharacters);
   TST_END_FIXTURE
};

