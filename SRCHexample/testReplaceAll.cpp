//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: testReplaceAll.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   31/01/25 10:11 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <SRCH/SRCHcache.h>
#include <SRCH/SRCHreplaceAll.h>
#include <SRCH/SRCHsearch.h>
#include <SRCHexample/testReplaceAll.h>
#include <UNIT/UNITapp.h>
COL_LOG_MODULE;

static void testEmptyReplacement() {
   const COLstring Replacement = "";
   const COLstring Query       = "local";
   const COLstring Path        = FILpathAppend("repos", "Comp1/");
   COLvar          Results;
   SRCHsearch(Path, Query, true, true, &Results);
   UNIT_ASSERT(Results["matches"].size());
   COLmap<COLstring, COLstring> FileContents;
   SRCHreplaceAllImpl(Results["matches"], Query, Replacement, &FileContents);
   const COLstring Foo = R"(T = {}

function foofunction()
    X = "bar"
    Y = "zoo"
end

return T)";

   const COLstring Main = R"(require 'foo'

function main()
    A = "hi"
    B = "world"

    message = A .. B
end)";

   const COLstring skl      = " U = \"top\"";
   const COLstring FooPath  = FILpathAppend("repos", "Comp1/foo.lua");
   const COLstring MainPath = FILpathAppend("repos", "Comp1/main.lua");
   const COLstring SklPath  = FILpathAppend("repos", "Comp1/skl.lua");

   for(const auto& it : FileContents) {
      COL_VAR(it.first);
      if(it.first == FooPath)  { UNIT_ASSERT_EQUALS(Foo, it.second); }
      if(it.first == MainPath) { UNIT_ASSERT_EQUALS(Main, it.second); }
      if(it.first == SklPath)  { UNIT_ASSERT_EQUALS(skl, it.second); }
   }
}

static void testSingleReplacement() {
   const COLstring Replacement = "foo";
   const COLstring Query       = "local";
   const COLstring Path        = FILpathAppend("repos", "Comp1/");
   COLvar          Results;
   SRCHsearch(Path, Query, true, true, &Results);
   COLmap<COLstring, COLstring> FileContents;
   SRCHreplaceAllImpl(Results["matches"], Query, Replacement, &FileContents);
   const COLstring Foo = R"(T = {}

function foofunction()
   foo X = "bar"
   foo Y = "zoo"
end

return T)";

   const COLstring Main = R"(require 'foo'

function main()
   foo A = "hi"
   foo B = "world"

   foo message = A .. B
end)";

   const COLstring skl      = "foo U = \"top\"";
   const COLstring FooPath  = FILpathAppend("repos", "Comp1/foo.lua");
   const COLstring MainPath = FILpathAppend("repos", "Comp1/main.lua");
   const COLstring SklPath  = FILpathAppend("repos", "Comp1/skl.lua");

   for(const auto& it : FileContents) {
      if(it.first == FooPath)  { UNIT_ASSERT_EQUALS(Foo, it.second); }
      if(it.first == MainPath) { UNIT_ASSERT_EQUALS(Main, it.second); }
      if(it.first == SklPath)  { UNIT_ASSERT_EQUALS(skl, it.second); }
   }
}

static void testSpecialCharacterReplacement() {
   const COLstring Replacement = R"(\"foo\")";
   const COLstring Query       = "local";
   const COLstring Path        = FILpathAppend("repos", "Comp1/");
   COLvar          Results;
   SRCHsearch(Path, Query, true, true, &Results);
   UNIT_ASSERT(Results["matches"].size());
   COLmap<COLstring, COLstring> FileContents;
   SRCHreplaceAllImpl(Results["matches"], Query, Replacement, &FileContents);

   const COLstring Foo = R"(T = {}

function foofunction()
   \"foo\" X = "bar"
   \"foo\" Y = "zoo"
end

return T)";

   const COLstring Main = R"(require 'foo'

function main()
   \"foo\" A = "hi"
   \"foo\" B = "world"

   \"foo\" message = A .. B
end)";

   const COLstring skl      = R"(\"foo\" U = "top")";
   const COLstring FooPath  = FILpathAppend("repos", "Comp1/foo.lua");
   const COLstring MainPath = FILpathAppend("repos", "Comp1/main.lua");
   const COLstring SklPath  = FILpathAppend("repos", "Comp1/skl.lua");

   for(const auto& it : FileContents) {
      if(it.first == FooPath)  { UNIT_ASSERT_EQUALS(Foo, it.second); }
      if(it.first == MainPath) { UNIT_ASSERT_EQUALS(Main, it.second); }
      if(it.first == SklPath)  { UNIT_ASSERT_EQUALS(skl, it.second); }
   }
}

static void testUnicodeReplacement() {
   const COLstring Replacement = "ünicode";
   const COLstring Query       = "local";
   const COLstring Path        = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   SRCHsearch(Path, Query, true, true, &Results);
   UNIT_ASSERT(Results["matches"].size());
   COLmap<COLstring, COLstring> FileContents;
   SRCHreplaceAllImpl(Results["matches"], Query, Replacement, &FileContents);

   const COLstring Main = R"(require 'foo'

function main()
   ünicode A = "hi"
   ünicode B = "world"
   ünicode message = A .. B
   ünicode unicode = "café"
   ünicode wholeword = " hi abc.git.git"
end)";

   const COLstring Foo = R"(T = {}

-- *test*
-- *test*

function foofunction() 
   ünicode X = "bar"
   ünicode Y = "zoo"
   ünicode A = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
end

return T)";

   const COLstring skl      = "ünicode U = \"top\"";
   const COLstring MainPath = FILpathAppend("repos", "Comp2/main.lua");
   const COLstring FooPath  = FILpathAppend("repos", "Comp2/foo.lua");
   const COLstring SklPath  = FILpathAppend("repos", "Comp2/skl.lua");

   for(const auto& it : FileContents) {
      if(it.first == MainPath) { UNIT_ASSERT_EQUALS(Main, it.second); }
      if(it.first == FooPath)  { UNIT_ASSERT_EQUALS(Foo, it.second); }
      if(it.first == SklPath)  { UNIT_ASSERT_EQUALS(skl, it.second); }
   }
}

static void testLongLineReplacement() {
   const COLstring Replacement = "LONG";
   const COLstring Query       = "local";
   const COLstring Path        = FILpathAppend("repos", "Comp2/");
   COLvar          Results;
   SRCHsearch(Path, Query, true, true, &Results);
   UNIT_ASSERT(Results["matches"].size());
   COLmap<COLstring, COLstring> FileContents;
   SRCHreplaceAllImpl(Results["matches"], Query, Replacement, &FileContents);

   const COLstring LongLine = "LONG A = \"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\"";
   const COLstring Foo = R"(T = {}

-- *test*
-- *test*

function foofunction() 
   LONG X = "bar"
   LONG Y = "zoo"
   )" + LongLine + R"(
end

return T)";

   const COLstring skl      = "LONG U = \"top\"";
   const COLstring FooPath  = FILpathAppend("repos", "Comp2/foo.lua");
   const COLstring SklPath  = FILpathAppend("repos", "Comp2/skl.lua");

   for(const auto& it : FileContents) {
      if(it.first == FooPath) { UNIT_ASSERT_EQUALS(Foo, it.second); }
      if(it.first == SklPath) { UNIT_ASSERT_EQUALS(skl, it.second); }
   }
}

void testReplaceAll(UNITapp* pApp) {
   pApp->add("replace/single", testSingleReplacement);
   pApp->add("replace/empty", testEmptyReplacement);
   pApp->add("replace/special_chars", testSpecialCharacterReplacement);
   pApp->add("replace/unicode", testUnicodeReplacement);
   pApp->add("replace/long_line", testLongLineReplacement);
}