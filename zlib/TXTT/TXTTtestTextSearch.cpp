//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TXTTtestTextSearch
//
// Description:
//
// Implementation
//
// Author: Eric Mulvaney
// Date:   Wednesday, April 15th, 2009 @ 04:56:42 PM
//
//---------------------------------------------------------------------------
#include "TXTTprecomp.h"
#pragma hdrstop

#include "TXTTtestTextSearch.h"

#include <TXT/TXTtextSearch.h>

#include <TST/TSTtestable.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSThelpers.h>


#define QUERY(X)    TXTtextSearch Search; Search.init(X)
#define MATCH(X)    TST_ASSERT(  Search.matches(X) )
#define NO_MATCH(X) TST_ASSERT( !Search.matches(X) )


static void basicTest()
{
   QUERY("word");
   MATCH("A word among many.");
   NO_MATCH("None to be found.");
}


static void phraseTest()
{
   QUERY("\"a phrase\"");
   MATCH("A phrase?  I suppose you could call it that.");
   NO_MATCH("That phrase won't match this.");
}


static void conjunctionTest()
{
   QUERY("this AND that");  // AND is not special.
   MATCH("This, that and some others.");
   NO_MATCH("This or that?");
}


static void disjunctionTest()
{
   QUERY("this OR that");
   MATCH("This and that.");
   MATCH("Just this.");
   MATCH("Just that.");
   NO_MATCH("Neither here NOR there.");
}


static void negationTest()
{
   QUERY("-this NOT that");
   NO_MATCH("Just this.");
   NO_MATCH("Just that.");
   NO_MATCH("This or that.");
   MATCH("Not here anyway.");
}


static void regexTest()
{
   QUERY("/this|that/");
   MATCH("Just this.");
   MATCH("Just that.");
   NO_MATCH("Nothing to see here.");
}


static void badRegexTest()
{
   QUERY("/bad(regex/");
   NO_MATCH("bad(regex");
   NO_MATCH("");
}


static void groupTest()
{
   QUERY("just (one OR more) /words?/");
   MATCH("Just one word.");
   MATCH("Just more words.");
   NO_MATCH("Just words.");
}


static void escapeTest()
{
   QUERY("\"\\\"quoted phrase\\\"\" OR /paths\\/are\\/no\\/fun/");

   // VC6 has a dumb pre-processor.
   const char* pQuotedPhrase = "\"quoted phrase\"";
   MATCH(pQuotedPhrase);

   MATCH("/paths/are/no/fun");
   NO_MATCH("quoted phrase");
   NO_MATCH("paths\\/are\\/no\\/fun");
}


static void notSoBasicTest()
{
   QUERY("two words OR (\"a phrase\" /and a regex/ but NOT this)");
   MATCH("So, two words, a phrase and a regex walk into a bar... but what?");
   MATCH("Just two words in a bar?");
   MATCH("But also a phrase and a regex.");
   NO_MATCH("This is a phrase and a regex, but not really.")
}


static void skipGroupOnFailTest()
{
   // When processing a group, the closing parenthesis marks its
   // success; when we fail to match something before a group, we
   // need to skip over the entire group and not mistake its end
   // as our success.

   QUERY("A word (this OR that) OR phrase");
   MATCH("A phrase.");
   NO_MATCH("A short sentence.");
}


static void missingParenthesisTest()
{
   QUERY("just (this OR that");
   MATCH("Just this.");
   MATCH("Just that.");
   NO_MATCH("Only just.");
}


static void superfluousParenthesisTest()
{
   QUERY("this)");
   MATCH("A phrase (like this).");
   MATCH("Just this.");
}


static void unclosedRegexTest()
{
   QUERY("/this|that");
   MATCH("This and that.");
   NO_MATCH("Neither.");
}


static void unclosedPhraseTest()
{
   QUERY("he said \"who cares");
   MATCH("Who cares what he said.");
   NO_MATCH("Tell me whom, apart from you or I, cares what he said?");
}


static void matchedAreaRegexTest()
{
   TXTtextSearch Search;

   Search.init("two words (\"a citation[1]\" OR /[regex]/) but NOT keywords nor -diatribes");
   {
      REXmatcher Regex = Search.matchedAreaRegex();
      TST_ASSERT(Regex.isExpressionValid());
      TST_ASSERT_EQUALS("(two|words|a citation\\[1\\]|[regex]|but|keywords|nor|diatribes)", Regex.pattern());
   }

   // Ensure that special characters are escaped outside regular-expressions.
   //
   Search.init("\"[](){}^.$|?*+\\\\\"");
   {
      REXmatcher Regex = Search.matchedAreaRegex();
      TST_ASSERT(Regex.isExpressionValid());
      TST_ASSERT_EQUALS("(\\[\\]\\(\\)\\{\\}\\^\\.\\$\\|\\?\\*\\+\\\\)", Regex.pattern());
   }

   // Invalid regular-expressions should be dropped.
   //
   Search.init("just these /[not this/");
   {
      REXmatcher Regex = Search.matchedAreaRegex();
      TST_ASSERT(Regex.isExpressionValid());
      TST_ASSERT_EQUALS("(just|these)", Regex.pattern());
   }

   // For empty queries, the pattern should match nothing quickly.
   //
   Search.init("");
   {
      REXmatcher Regex = Search.matchedAreaRegex();
      TST_ASSERT(Regex.isExpressionValid());
      TST_ASSERT_EQUALS("^(?!.).", Regex.pattern());
   }
}


TSTtestable* TXTTtestTextSearch()
{
   TST_START_COLLECTION("TXTTtestTextSearch");
      TST_ADD_TEST_FUNCTION(basicTest);
      TST_ADD_TEST_FUNCTION(phraseTest);
      TST_ADD_TEST_FUNCTION(conjunctionTest);
      TST_ADD_TEST_FUNCTION(disjunctionTest);
      TST_ADD_TEST_FUNCTION(negationTest);
      TST_ADD_TEST_FUNCTION(regexTest);
      TST_ADD_TEST_FUNCTION(badRegexTest);
      TST_ADD_TEST_FUNCTION(groupTest);
      TST_ADD_TEST_FUNCTION(escapeTest);
      TST_ADD_TEST_FUNCTION(notSoBasicTest);
      TST_ADD_TEST_FUNCTION(skipGroupOnFailTest);
      TST_ADD_TEST_FUNCTION(missingParenthesisTest);
      TST_ADD_TEST_FUNCTION(superfluousParenthesisTest);
      TST_ADD_TEST_FUNCTION(unclosedRegexTest);
      TST_ADD_TEST_FUNCTION(unclosedPhraseTest);
      TST_ADD_TEST_FUNCTION(matchedAreaRegexTest);
   TST_END_FIXTURE
}
