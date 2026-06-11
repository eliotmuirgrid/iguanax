// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFind
//
// Description:
//
// Implementation.
//
// Author: Vismay Shah
// Date:   Tue 28 Nov 2023 13:51:48 EST
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SRCH/SRCHcache.h>
#include <SRCH/SRCHsearch.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testRandom() {
   COL_FUNCTION(testRandom);
   const COLstring Path = FILpathAppend("repos", "file1.txt");
   COLvar Results;
   SRCHsearch(Path, ".git2", true, true, &Results);
   COL_VAR(Results["matches"].size());
   UNIT_ASSERT(Results["matches"].size() == 1);
}

static void testWholeWord() {
   COL_FUNCTION(testWholeWord);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar Results;
   SRCHsearch(Path, "git", true, true, &Results);
   COL_VAR(Results["matches"].size());
   UNIT_ASSERT(Results["matches"].size() == 2);
}

static void testLongSearchString() {
   COL_FUNCTION(testLongSearchString);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   const COLstring longString(100, 'a');
   SRCHsearch(Path, longString, true, false, &Results);
   UNIT_ASSERT(Results["matches"].size() == 1);  // File with long string
}

static void testUnicodeCharacters() {
   COL_FUNCTION(testUnicodeCharacters);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   SRCHsearch(Path, "café", true, false, &Results);
   UNIT_ASSERT(Results["matches"].size() == 1);  // Files with accented characters
}

static void testSpecialChars() {
   COL_FUNCTION(testSpecialChars);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   SRCHsearch(Path, "*test*", true, true, &Results);
   COL_VAR(Results["matches"]);
   UNIT_ASSERT(Results["matches"].size() == 2);  // Expect files with *test* pattern
}

static void testEmptyString() {
   COL_FUNCTION(testEmptyString);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   SRCHsearch(Path, "", true, true, &Results);
   UNIT_ASSERT(Results["matches"].size() == 0);  // Should match nothing
}

static void testWordLen() {
   COL_FUNCTION(testWordLen);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar Results;
   SRCHsearch(Path, "loca", true, true, &Results);
   COL_VAR(Results["matches"]);
   UNIT_ASSERT(Results["matches"].size() == 0);
   Results.reset();
   SRCHsearch(Path, "loca", true, false, &Results);
   COL_VAR(Results["matches"]);
   COL_VAR(Results["matches"].size());
   UNIT_ASSERT(Results["matches"].size() == 9);
}

static void testMatchCase() {
   COL_FUNCTION(testMatchCase);
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLvar Results;
   SRCHsearch(Path, "Local", true, true, &Results);
   COL_VAR(Results["matches"]);
   UNIT_ASSERT(Results["matches"].size() == 0);
   Results.reset();
   SRCHsearch(Path, "Local", false, true, &Results);
   COL_VAR(Results["matches"]);
   COL_VAR(Results["matches"].size());
   UNIT_ASSERT(Results["matches"].size() == 9);
}

static void testCache() {
   COL_FUNCTION(testCache);
   COLvar Result;
   const COLstring Path = FILpathAppend("repos", "Comp2/");
   COLmap<COLstring, COLstring> Cache;
   SRCHcache(&Cache, Path, &Result);
   UNIT_ASSERT(Cache.size() == 3);
   for(const auto& it : Cache) { COL_VAR2(it.first, it.second); }
}

void testFind(UNITapp* pApp){
   COL_FUNCTION(testFind);
   pApp->add("search/cache", testCache);
   pApp->add("search/case_sensitive", testMatchCase);
   pApp->add("search/word_len", testWordLen);
   pApp->add("search/empty_string", testEmptyString);
   pApp->add("search/special_chars", testSpecialChars);
   pApp->add("search/unicode", testUnicodeCharacters);
   pApp->add("search/long_string", testLongSearchString);
   pApp->add("search/whole_word", testWholeWord);
   pApp->add("search/random", testRandom);
}