//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testSearch.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   23/04/24 12:36 PM
//  ---------------------------------------------------------------------------
#include "testSearch.h"

#include <COL/COLlog.h>
#include <MATCH/MATCHsearch.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testBizarreQueries() {
   COL_FUNCTION(testBizarreQueries);
   MATCHpositions Positions;
   const COLstring Terms = "#dev #prod #test #critical_systems #debug #error #warning";

   UNIT_ASSERT(MATCHsearch("  \t\n\r  #dev  \t\n\r  AND  \t\n\r  #prod  \t\n\r  ", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("      \t\t\t      ", Terms, false, &Positions)); // Only whitespace

   // Deeply nested NOT operations
   UNIT_ASSERT(MATCHsearch("NOT NOT NOT NOT NOT #unknown", Terms, false, &Positions)); // Odd number of NOTs
   UNIT_ASSERT(!MATCHsearch("NOT NOT NOT NOT #unknown", Terms, false, &Positions)); // Even number of NOTs
   UNIT_ASSERT(!MATCHsearch("NOT NOT NOT NOT NOT NOT NOT #dev", Terms, false, &Positions)); // 7 NOTs (odd)
   
   UNIT_ASSERT(!MATCHsearch("AND AND AND", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("OR OR OR", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT AND OR", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("AND OR NOT AND OR", Terms, false, &Positions));

   // Mixed valid and invalid operator sequences
   UNIT_ASSERT(!MATCHsearch("#dev AND AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev OR OR #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev AND OR NOT", Terms, false, &Positions));

   // Operators mixed with empty terms or just whitespace
   UNIT_ASSERT(!MATCHsearch("AND  \t  OR  \n  NOT", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev AND  \t\n  OR #prod", Terms, false, &Positions));

   // Very long chains of valid operations
   COLstring LongQuery = "#dev AND #prod AND #test AND #debug AND #error AND #warning";
   UNIT_ASSERT(MATCHsearch(LongQuery, Terms, false, &Positions));

   COLstring LongOrQuery = "#unknown OR #missing OR #absent OR #dev OR #nonexistent";
   UNIT_ASSERT(MATCHsearch(LongOrQuery, Terms, false, &Positions)); // Should match #dev

   // Alternating NOT patterns
   UNIT_ASSERT(MATCHsearch("NOT #unknown AND NOT #missing AND #dev", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT #dev AND NOT #prod AND NOT #test", Terms, false, &Positions));

   // Edge cases with wildcards and operators
   UNIT_ASSERT(!MATCHsearch("* AND AND *", Terms, false, &Positions)); // Invalid syntax
   UNIT_ASSERT(MATCHsearch("NOT #unkn* AND #de*", Terms, false, &Positions)); // Valid mixed wildcards

   // Regex-like patterns that should be treated as glob (not actual regex)
   UNIT_ASSERT(MATCHsearch("#d[ev]", "#d[ev]", false, &Positions)); // Literal bracket match
   UNIT_ASSERT(!MATCHsearch("#d[ev]", "#dev", false, &Positions)); // Should not regex expand
   UNIT_ASSERT(MATCHsearch("#d\\ev", "#d\\ev backslash", false, &Positions)); // Literal backslash

   UNIT_ASSERT(MATCHsearch("and AND or", "text and AND or here", false, &Positions));
   UNIT_ASSERT(MATCHsearch("And OR and", "mixed And OR and case", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("not NOT Not", "various not NOT Not forms", false, &Positions));

   UNIT_ASSERT(MATCHsearch("term\x00with\x00nulls", "term\x00with\x00nulls", false, &Positions)); // Null bytes
   UNIT_ASSERT(MATCHsearch("unicode™test", "unicode™test content", false, &Positions)); // Unicode symbols
   UNIT_ASSERT(MATCHsearch("emoji😀test", "emoji😀test data", false, &Positions)); // Emoji characters

   // Very long individual terms
   COLstring VeryLongTerm(1000, 'x'); // 1000 character term
   COLstring VeryLongText = "start " + VeryLongTerm + " end";
   UNIT_ASSERT(MATCHsearch(VeryLongTerm, VeryLongText, false, &Positions));

   UNIT_ASSERT(MATCHsearch("#dev\t\t\tAND\n\n\n#prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("\t\n\r \t\n\r AND \t\n\r OR \t\n\r", Terms, false, &Positions));

   // Mixed operator precedence
   UNIT_ASSERT(MATCHsearch("NOT #unknown OR #dev AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev OR NOT #unknown AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT #dev AND NOT #prod OR NOT #test", Terms, false, &Positions)); // All negated

   // Boundary conditions for term matching
   UNIT_ASSERT(MATCHsearch("", "", false, &Positions)); // Both empty
   UNIT_ASSERT(Positions.size() == 0);

   UNIT_ASSERT(MATCHsearch("a", "a", false, &Positions));
   UNIT_ASSERT(MATCHsearch("A", "a", false, &Positions));

   // Numbers and special chars as search terms
   UNIT_ASSERT(MATCHsearch("123 AND 456", "text 123 and 456 here", false, &Positions));
   UNIT_ASSERT(MATCHsearch("@#$% OR &*()", "symbols @#$% present", false, &Positions));

   COLstring ManyTerms = "a AND b AND c AND d AND e AND f AND g AND h AND i AND j";
   COLstring ManyText = "a b c d e f g h i j";
   UNIT_ASSERT(MATCHsearch(ManyTerms, ManyText, false, &Positions));

   // Operators at word boundaries within larger terms
   UNIT_ASSERT(MATCHsearch("ANDROID", "testing ANDROID app", false, &Positions));
   UNIT_ASSERT(MATCHsearch("FORMULA", "math FORMULA calculation", false, &Positions));
   UNIT_ASSERT(MATCHsearch("NOTATION", "music NOTATION system", false, &Positions));

   // Repeated identical terms with operators
   UNIT_ASSERT(MATCHsearch("#dev OR #dev OR #dev", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev AND #dev", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("NOT #unknown AND NOT #unknown", Terms, false, &Positions));

   // Complex malformed expressions
   UNIT_ASSERT(!MATCHsearch("AND #dev OR AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev AND OR NOT AND #prod", Terms, false, &Positions));

   // Special characters in search terms (should be escaped properly)
   UNIT_ASSERT(MATCHsearch("func()", "calling func() method", false, &Positions));
   UNIT_ASSERT(MATCHsearch("[debug]", "log [debug] message", false, &Positions));
   UNIT_ASSERT(MATCHsearch("$var", "shell $var expansion", false, &Positions));
   UNIT_ASSERT(MATCHsearch("a.b", "module a.b loaded", false, &Positions));
   UNIT_ASSERT(MATCHsearch("10^2", "math 10^2 equals 100", false, &Positions));

   // Unicode and non-ASCII
   UNIT_ASSERT(MATCHsearch("café", "visiting café downtown", false, &Positions));

   // Very long terms
   COLstring longTerm = "very_long_term_that_goes_on_and_on_and_on_and_on_and_on_and_on_and_on";
   COLstring longText = "text with " + longTerm + " in it";
   UNIT_ASSERT(MATCHsearch(longTerm, longText, false, &Positions));
}

static void testMalformedQueries() {
   COL_FUNCTION(testMalformedQueries);
   MATCHpositions Positions;
   const COLstring Terms = "#dev #prod #test #critical_systems";

   // Dangling operators - should be treated as literal search terms
   UNIT_ASSERT(!MATCHsearch("AND", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("OR", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT", Terms, false, &Positions));

   // Test with text that contains the operator keywords
   UNIT_ASSERT(!MATCHsearch("AND", "searching for AND keyword", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("OR", "searching for OR keyword", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT", "do NOT enter", false, &Positions));

   // Leading operators (invalid syntax)
   UNIT_ASSERT(!MATCHsearch("AND #dev", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("OR #dev", Terms, false, &Positions));

   // Trailing operators (invalid syntax)
   UNIT_ASSERT(!MATCHsearch("#dev AND", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev OR", Terms, false, &Positions));

   // Multiple consecutive operators
   UNIT_ASSERT(!MATCHsearch("#dev AND OR #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev OR AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("AND OR NOT", Terms, false, &Positions));

   // NOT without operand
   UNIT_ASSERT(!MATCHsearch("NOT", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev NOT", Terms, false, &Positions));

   // Empty terms with operators
   UNIT_ASSERT(!MATCHsearch("   AND   ", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("  #dev  AND  ", Terms, false, &Positions));

   // Mixed case operators (should be treated as literals)
   UNIT_ASSERT(!MATCHsearch("#dev And #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev and #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("Not #dev", Terms, false, &Positions));

   // Operators as part of larger terms (should work as literals)
   UNIT_ASSERT(MATCHsearch("ANDROID", "testing ANDROID app", false, &Positions));
   UNIT_ASSERT(MATCHsearch("OR-gate", "logic OR-gate circuit", false, &Positions));
   UNIT_ASSERT(MATCHsearch("CANNOT", "user CANNOT access", false, &Positions));

   // Multiple spaces (should be handled gracefully)
   UNIT_ASSERT(MATCHsearch("#dev     #prod", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev   AND   #prod", Terms, false, &Positions));

   // Only operators
   UNIT_ASSERT(!MATCHsearch("AND OR NOT", Terms, false, &Positions));

   // Nested NOT operations (should work)
   UNIT_ASSERT(MATCHsearch("NOT NOT #dev", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT NOT #unknown", Terms, false, &Positions));

   // Complex malformed expressions
   UNIT_ASSERT(!MATCHsearch("AND #dev OR AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev AND OR NOT AND #prod", Terms, false, &Positions));

   // Special characters in search terms (should be escaped properly)
   UNIT_ASSERT(MATCHsearch("func()", "calling func() method", false, &Positions));
   UNIT_ASSERT(MATCHsearch("[debug]", "log [debug] message", false, &Positions));
   UNIT_ASSERT(MATCHsearch("$var", "shell $var expansion", false, &Positions));
   UNIT_ASSERT(MATCHsearch("a.b", "module a.b loaded", false, &Positions));
   UNIT_ASSERT(MATCHsearch("10^2", "math 10^2 equals 100", false, &Positions));

   // Unicode and non-ASCII
   UNIT_ASSERT(MATCHsearch("café", "visiting café downtown", false, &Positions));

   // Very long terms
   COLstring longTerm = "very_long_term_that_goes_on_and_on_and_on_and_on_and_on_and_on_and_on";
   COLstring longText = "text with " + longTerm + " in it";
   UNIT_ASSERT(MATCHsearch(longTerm, longText, false, &Positions));
}

static void testGlobEdgeCases() {
   COL_FUNCTION(testGlobEdgeCases);
   MATCHpositions Positions;
   const COLstring Terms = "#dev #prod #support #critical_systems";

   // Empty/whitespace filter -> match (no positions)
   UNIT_ASSERT(MATCHsearch("", Terms, false, &Positions));
   UNIT_ASSERT(Positions.size() == 0);
   UNIT_ASSERT(MATCHsearch("   ", Terms, false, &Positions));
   UNIT_ASSERT(Positions.size() == 0);

   // Empty string with non-empty filter -> no match
   UNIT_ASSERT(!MATCHsearch("#dev", "", false, &Positions));

   // Case-insensitive matching
   UNIT_ASSERT(MATCHsearch("#DEV", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev", Terms, false, &Positions));

   // Sequence matching
   UNIT_ASSERT(MATCHsearch("#dev #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #unknown", Terms, false, &Positions));

   // Wildcards
   UNIT_ASSERT(MATCHsearch("#cr*al_sys*", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#supp*", Terms, false, &Positions));

   // Exact match boundaries
   UNIT_ASSERT(MATCHsearch("#dev", Terms, true, &Positions));
   UNIT_ASSERT(!MATCHsearch("#de", Terms, true, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev", "#devops", true, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev*", "#devops", true, &Positions));

   // Exact match token boundary should cross punctuation
   UNIT_ASSERT(MATCHsearch("#dev", "#dev,", true, &Positions));

   // Glob escaping for regex meta: '.' should be literal, '*' wildcard
   UNIT_ASSERT(MATCHsearch("file.name*", "file.name123", false, &Positions));
}

static void testPositionsAndMax() {
   COL_FUNCTION(testPositionsAndMax);
   MATCHpositions Positions;
   // Limit matches to 1
   UNIT_ASSERT(MATCHsearch("*", "alpha", false, &Positions, 1));
   UNIT_ASSERT(Positions.size() == 1);

   // NOT should return no positions
   UNIT_ASSERT(!MATCHsearch("NOT #dev", "#dev", false, &Positions));
   UNIT_ASSERT(Positions.size() == 0);

   // AND should collect positions from both terms
   UNIT_ASSERT(MATCHsearch("#dev AND #prod", "#dev #prod #test", false, &Positions));
   UNIT_ASSERT(Positions.size() == 2);

   // OR should collect positions from matching terms
   UNIT_ASSERT(MATCHsearch("#dev OR #unknown", "#dev #prod", false, &Positions));
   UNIT_ASSERT(Positions.size() == 1);

   // Multiple matches of same term
   UNIT_ASSERT(MATCHsearch("test", "test this test that", false, &Positions));
   UNIT_ASSERT(Positions.size() == 2);
}

static void testRegexEdgeCases() {
   COL_FUNCTION(testRegexEdgeCases);
   MATCHpositions Positions;
   // Empty string never matches in regex path (by design)
   UNIT_ASSERT(!MATCHsearch("/.*/", "", false, &Positions));
   // Invalid regex => treated as glob; should not match here
   UNIT_ASSERT(!MATCHsearch("/[abc/", "#prod", false, &Positions));
   // Multiple regex matches accumulate positions
   UNIT_ASSERT(MATCHsearch("/a./", "aa ab ac", false, &Positions));
   UNIT_ASSERT(Positions.size() >= 3);
}

static void testGlob() {
   COL_FUNCTION(testGlob);
   MATCHpositions Positions;
   const COLstring Terms = "#dev #prod #test #critical_systems";

   // Explicit OR
   UNIT_ASSERT(MATCHsearch("#dev OR #unknown", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#unknown OR #dev", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#unknown OR #missing", Terms, false, &Positions));

   // Explicit AND
   UNIT_ASSERT(MATCHsearch("#dev AND #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev AND #unknown", Terms, false, &Positions));

   // NOT operator (uppercase keywords)
   UNIT_ASSERT(MATCHsearch("NOT #unknown", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT #dev", Terms, false, &Positions));

   // Operator precedence: NOT > AND > OR
   UNIT_ASSERT(MATCHsearch("#dev AND #prod OR #test", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("NOT #unknown AND #dev", Terms, false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev OR #test AND #prod", Terms, false, &Positions));

   // Sequence matching
   UNIT_ASSERT(MATCHsearch("#dev #prod", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#prod #dev", Terms, false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #prod #unknown", Terms, false, &Positions));

   // Wildcards with operators
   UNIT_ASSERT(MATCHsearch("#cr* OR #de*", Terms, false, &Positions));
}

static void testRegex() {
   COL_FUNCTION(testRegex);
   MATCHpositions Positions;
   UNIT_ASSERT(MATCHsearch("/this|that/", "this that test", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/#test*/", "#testing", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/#prod|#test/", "#prod", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/#prod|#test/", "#test", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b/", "user@example.com", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/AND|OR/", "testing AND keywords", false, &Positions));
   UNIT_ASSERT(MATCHsearch("/\\(|\\)/", "test (parentheses)", false, &Positions));
}

static void testSequenceMatching() {
   COL_FUNCTION(testSequenceMatching);
   MATCHpositions Positions;
   // Basic sequence matching
   UNIT_ASSERT(MATCHsearch("#dev #test", "#dev #test #prod", false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev #test", "#dev #test", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #test", "#test #dev", false, &Positions)); // Order matters
   
   // Sequence with AND
   UNIT_ASSERT(MATCHsearch("#dev #test AND #supp", "#dev #test #supp", false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev #test AND #supp", "#supp #dev #test", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #test AND #supp", "#dev #supp #test", false, &Positions)); // Sequence broken
   
   // Sequence with OR
   UNIT_ASSERT(MATCHsearch("#dev #test OR #prod", "#dev #test", false, &Positions));
   UNIT_ASSERT(MATCHsearch("#dev #test OR #prod", "#prod", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #test OR #prod", "#test #dev", false, &Positions));
   
   // Sequence with NOT
   UNIT_ASSERT(MATCHsearch("NOT #dev #test", "#prod #supp", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("NOT #dev #test", "#dev #test", false, &Positions));
   
   // Combinations
   UNIT_ASSERT(MATCHsearch("#dev #test AND NOT #debug #error", "#dev #test #info", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#dev #test AND NOT #debug #error", "#dev #test #debug #error", false, &Positions));
   
   // Wildcards in sequences
   UNIT_ASSERT(MATCHsearch("#dev* #test*", "#development #testing", false, &Positions));
   
   // Long sequences
   UNIT_ASSERT(MATCHsearch("#a #b #c #d", "#a #b #c #d #e", false, &Positions));
   UNIT_ASSERT(!MATCHsearch("#a #b #c #d", "#a #b #d #c", false, &Positions));
}

void testSearch(UNITapp* pApp) {
   pApp->add("search/regex",         &testRegex);
   pApp->add("search/glob",          &testGlob);
   pApp->add("search/sequence",      &testSequenceMatching);
   pApp->add("search/edge/glob",     &testGlobEdgeCases);
   pApp->add("search/positions",     &testPositionsAndMax);
   pApp->add("search/edge/regex",    &testRegexEdgeCases);
   pApp->add("search/malformed",     &testMalformedQueries);
   pApp->add("search/bizarre",       &testBizarreQueries);
}