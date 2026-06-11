//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
// 
// Author: Ram Siva
// Date:   Tue 08/24/2013 
//
//---------------------------------------------------------------------------

#include <REXexample/REXexamplePrecomp.h>
#pragma hdrstop

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <REX/REXfind.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

static void testREXfind() {
   COL_FUNCTION(testREXfind);

   REXfind Find;
   Find.init("ADT|BAR");
   const char* String1 = "123ADTqrsBARxyz";           

   // Checking the matching functionality of REXfind.
   Find.matchBegin(String1, ::strlen(String1));
   TST_ASSERT_MESSAGE(Find.matchNext() && Find.position()==6,
                      "REXfind::matchNext false negative match");
   TST_ASSERT_MESSAGE(Find.matchNext() && Find.position()==12,
                      "REXfind::matchNext false negative match");
   TST_ASSERT_MESSAGE(Find.matchNext()==false,
                      "REXfind::matchNext false positive match");
}

static void testREXfindPcreValgrindError() {
   COL_FUNCTION(testREXfindPcreValgrindError);
   COLstring str("[[[");
   try { REXfind().init(str.c_str()); } catch(...) { }
}

static void testREXfindEmptyPattern() {
   COL_FUNCTION(testREXfindEmptyPattern);

   const char* String1 = "ABChttp://qrs 342";
   const char* Pattern = "(https?://\\S*)|()";
   REXfind Find1, Find2;
   Find1.init(Pattern, REXfind::REX_DEFAULT);
   Find2.init(Pattern, REXfind::REX_NOTEMPTY);

   Find1.matchBegin(String1, ::strlen(String1));
   TST_ASSERT_MESSAGE(Find1.matchNext() && Find1.position()==0,
                      "REXfind::matchNext false negative match for default option");
   TST_ASSERT_MESSAGE(Find1.matchNext() && Find1.position()==0,
                      "REXfind::matchNext false negative match for default option");
   TST_ASSERT_MESSAGE(Find1.matchNext() && Find1.position()==0,
                      "REXfind::matchNext false negative match for default option");

   Find2.matchBegin(String1, ::strlen(String1));
   TST_ASSERT_MESSAGE(Find2.matchNext() && Find2.position()==13,
                      "REXfind::matchNext false negative match for REX_NOTEMPTY option");
   TST_ASSERT_MESSAGE(Find2.matchNext()==false && Find2.position() < 0,
                      "REXfind::matchNext false positive match for REX_NOTEMPTY option");
}

static void testREXfindReuseFinder() {
   COL_FUNCTION(testREXfindReuseFinder);

   const COLstring String1 = "1 2 3 4 5";
   const COLstring String2 = "6 7 8 9 0";
   const char* Pattern = "\\S+";

   REXfind Finder;
   Finder.init(Pattern);

   COL_TRC("Starting finder on string 1");
   Finder.matchBegin(String1.data(), String1.size());
   int NumMatches = 0;
   while (Finder.matchNext()) {
      REXresult Match = Finder.match(0);
      COL_DBG("String 1 match: " << Match);
      NumMatches++;
   }
   COL_VAR(NumMatches);
   TST_ASSERT_MESSAGE(NumMatches == 5, "Expected 5 matches, got: " << NumMatches);
   // It should not crash or error when attempting to match again - it should just return false
   TST_ASSERT_NO_EXCEPTION(Finder.matchNext());

   COL_TRC("Reusing finder on string 2");
   Finder.matchBegin(String2.data(), String2.size());

   while (Finder.matchNext()) {
      REXresult Match = Finder.match(0);
      COL_DBG("String 2 match: " << Match);
      NumMatches++;
   }
   COL_VAR(NumMatches);
   TST_ASSERT_MESSAGE(NumMatches == 10, "Expected 10 matches, got: " << NumMatches);
   TST_ASSERT_NO_EXCEPTION(Finder.matchNext());
   COL_TRC("No crashes! All good");
}

TSTtestable* getREXfindTests() {
   TST_START_COLLECTION("testREXfind")
      TST_ADD_TEST_FUNCTION(testREXfind)
      TST_ADD_TEST_FUNCTION(testREXfindPcreValgrindError)
      TST_ADD_TEST_FUNCTION(testREXfindEmptyPattern)
      TST_ADD_TEST_FUNCTION(testREXfindReuseFinder)
   TST_END_COLLECTION
}
