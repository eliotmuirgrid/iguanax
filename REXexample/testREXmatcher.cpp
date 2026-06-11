//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Description:
//
// Implementation
// 
// Author: George Ma
// Date:   Tue 03/02/2004 
//
//---------------------------------------------------------------------------

#include <REXexample/REXexamplePrecomp.h>
#pragma hdrstop

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <REX/REXmatcher.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>

void testREXmatcher()
{
   REXmatcher Matcher;

   Matcher.init("ADT|BAR");               
                                          
   const char* String1 = "ADT";           
   const char* String2 = "BAR";
   const char* String3 = "DFSD";
   const char* String4 = "DFJDADTFKKGBARKGK";

   // Checking the matching functionality of REXmatcher.
   TST_ASSERT_MESSAGE(Matcher.doesMatch(String1),"REXmatcher::doesMatch failed");
   TST_ASSERT_MESSAGE(Matcher.doesMatch(String2),"REXmatcher::doesMatch failed");
   TST_ASSERT_MESSAGE(!Matcher.doesMatch(String3),"REXmatcher::doesMatch failed");
   TST_ASSERT_MESSAGE(Matcher.doesMatch(String4),"REXmatcher::doesMatch failed");
   

   COLindex PatternStart;
   COLindex PatternEnd;

   // We can excuse the use of Majik numbers here because it is just a unit test.
   Matcher.findMatch(String4,0,PatternStart,PatternEnd);
   TST_ASSERT_MESSAGE(PatternStart == 4,"REXmatcher::findMatch failed");
   TST_ASSERT_MESSAGE(PatternEnd == 6,"REXmatcher::findMatch failed");  

   Matcher.findMatch(String4,PatternEnd+1,PatternStart,PatternEnd);
   TST_ASSERT_MESSAGE(PatternStart == 11,"REXmatcher::findMatch failed");
   TST_ASSERT_MESSAGE(PatternEnd == 13,"REXmatcher::findMatch failed");  

   COLstring String5(String4);
   Matcher.regexReplace(String5,"boourns");
   REX_EXAMPLE_LOG(String5);
   TST_ASSERT_MESSAGE(String5 == "DFJDboournsFKKGboournsKGK","REXmatcher::regexReplace failed");  

   Matcher.init("COL(.*)");

   // This is okay for a unit test - this code doesn't have to be as maintainable as application code
   COLstring String6 = "COLbinaryDump";      
   Matcher.regexReplace(String6,"FMT$1");
   REX_EXAMPLE_LOG(String6);
   TST_ASSERT_MESSAGE(String6 == "FMTbinaryDump","REXmatcher::regexReplace failed");  
}

void bad1()
{
   TST_ASSERT_MESSAGE(false, "Bad");
}

void bad2()
{
   TST_ASSERT_MESSAGE(false, "VeryBad");
}

void testREXmatcherPcreValgrindError()
{
   // FYI - The version of PCRE for REX generates valgrind errors.
   //       Hopefully they will go away once PCRE is upgraded.
   REXmatcher Matcher("[[["); // a pattern found in a DLLexample test.

   /*
   Conditional jump or move depends on uninitialised value(s)
      at 0x806088A: pcre_compile_rex (pcre.c:2553)
      by 0x805B346: REXmatcher::init(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:163)
      by 0x805B7B4: REXmatcher::REXmatcher(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:132)
   
   Conditional jump or move depends on uninitialised value(s)
      at 0x805FAD7: pcre_compile_rex (pcre.c:2571)
      by 0x805B346: REXmatcher::init(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:163)
      by 0x805B7B4: REXmatcher::REXmatcher(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:132)
   
   Conditional jump or move depends on uninitialised value(s)
      at 0x8060783: pcre_compile_rex (pcre.c:3005)
      by 0x805B346: REXmatcher::init(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:163)
      by 0x805B7B4: REXmatcher::REXmatcher(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:132)
   
   Use of uninitialised value of size 4
      at 0x806083C: pcre_compile_rex (pcre.c:3032)
      by 0x805B346: REXmatcher::init(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:163)
      by 0x805B7B4: REXmatcher::REXmatcher(COLstring const&, REXmatcher::REXoption) (REXmatcher.cpp:132)
   */
}

void testREXmatcherInvalidByDefault()
{
   REXmatcher Matcher;
   TST_ASSERT_EQUALS(false, Matcher.isExpressionValid());
}


static COLstring replacement(const char* pMatch, size_t MatchLength, void* pContext)
{
   COL_FUNCTION(replacement);
   int& Count = *(int*)pContext;
   ++Count;
   COL_VAR(Count);
   COL_TRC("Match: " << COLstring(pMatch,MatchLength));
   return COLintToString(Count) + ":" + COLstring(pMatch, MatchLength);
}

/*
void testFunctionalReplace()
{
   COL_FUNCTION(testFunctionalReplace);

   REXmatcher Matcher("(\\w+)");

   COLstring Subject = "one two three";

   int Count = 0;
   Matcher.functionalReplace(Subject, replacement, NULL, &Count);

   COL_VAR(Count);
   COL_TRC("Got: " << Subject);

   TST_ASSERT_EQUALS(3, Count);
   TST_ASSERT_EQUALS("1:one 2:two 3:three", Subject);
}
*/

COLstring testLargeFilter(const COLstring& Input)
{
   TST_ASSERT(Input == "b" || Input.is_null());
   COLstring InputUpper = COLupperCase(Input);
   InputUpper += '!';
   return InputUpper;
}

COLstring testLargeReplace(const char* pMatch, size_t MatchLength, void* pContext)
{
   COLstring Match;
   Match.append(pMatch,MatchLength);
   TST_ASSERT(Match.is_null() || Match == "a");
   Match.toUpperCase();
   Match += "??";
   return Match;
}

/*
void testLargeFunctionalReplace()
{
   //make a large string
   COLstring TestString;
   COLstring ExpectedString = "!";
   while(TestString.size() < 1024*1024)
   {
      TestString += "ab";
      ExpectedString += "A??B!";
   }

   const COLuint32 OrigSize = TestString.size();
   REXmatcher Matcher("(a)");
   Matcher.functionalReplace(TestString, &testLargeReplace, &testLargeFilter, 0);

   TST_ASSERT_EQUALS(1+(OrigSize/2)*5, ExpectedString.size());
   TST_ASSERT_EQUALS(ExpectedString.substr(0,20), TestString.substr(0,20));
   TST_ASSERT(ExpectedString == TestString);
}
*/

COLstring testLargeReplace2(const char* pMatch, size_t MatchLength, void* pContext)
{
   COLstring Match;
   Match.write(pMatch,MatchLength);
   TST_ASSERT(Match.is_null() || Match == "abc");
   return "WabcXYZ";
}

/*
void testLargeFunctionalReplaceWithoutFilter()
{
   //make a large string
   COLstring TestString;
   COLstring ExpectedString;
   while(TestString.size() < 1024*1024)
   {
      TestString += "XYabc";
      ExpectedString += "XYWabcXYZ";
   }

   const COLuint32 OrigSize = TestString.size();
   REXmatcher Matcher("(abc)");
   Matcher.functionalReplace(TestString, &testLargeReplace2, 0, 0);

   TST_ASSERT_EQUALS((OrigSize/5)*9, TestString.size());
   TST_ASSERT_EQUALS(ExpectedString.substr(0,20), TestString.substr(0,20));
   TST_ASSERT(ExpectedString == TestString);
}
*/

void testREXmatcherEmptyPattern()
{
   const char* String1 = "ADTXYZ";
   const char* String2 = "ABChttp://qrs 342";
   const char* Pattern = "(https?://\\S*)|()";
   COLindex Pos(1), Beg(0), End(0);
   REXmatcher Matcher1(Pattern, REXmatcher::REXoptionDefault);
   REXmatcher Matcher2(Pattern, REXmatcher::NotEmpty);

   TST_ASSERT_MESSAGE(Matcher1.doesMatch(String1),
                      "REXmatcher::doesMatch false negative for default  option");
   TST_ASSERT_MESSAGE(Matcher1.doesMatch(String2),
                      "REXmatcher::doesMatch false negative for default  option");
   TST_ASSERT_MESSAGE(Matcher2.doesMatch(String1)==false,
                      "REXmatcher::doesMatch false positive for NotEmpty option");
   TST_ASSERT_MESSAGE(Matcher2.doesMatch(String2),
                      "REXmatcher::doesMatch false negative for NotEmpty option");
   TST_ASSERT_MESSAGE(Matcher1.findMatch(String1,Pos,Beg,End),
                      "REXmatcher::findMatch false negative for default option");
   TST_ASSERT_MESSAGE(Matcher2.findMatch(String1,Pos,Beg,End)==false,
                      "REXmatcher::findMatch false positive for NotEmpty option");
}

TSTtestable* getREXmatcherTests() 
{
   TST_START_COLLECTION("testREX")
      TST_ADD_TEST_FUNCTION(testREXmatcher)
      TST_ADD_TEST_FUNCTION(testREXmatcherPcreValgrindError)
      TST_ADD_TEST_FUNCTION(testREXmatcherInvalidByDefault)
      TST_ADD_TEST_FUNCTION(testREXmatcherEmptyPattern)
      //TST_ADD_TEST_FUNCTION(testFunctionalReplace);
      //TST_ADD_TEST_FUNCTION(testLargeFunctionalReplace);
      //TST_ADD_TEST_FUNCTION(testLargeFunctionalReplaceWithoutFilter);
      //TST_ADD_TEST_FUNCTION(bad1)
      //TST_ADD_TEST_FUNCTION(bad2)
   TST_END_COLLECTION
}
