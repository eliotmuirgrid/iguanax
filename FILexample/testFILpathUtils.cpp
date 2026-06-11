//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFILutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tue 12 Aug 2025
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testFILpathSimplify(){
   COL_FUNCTION(testFILpathSimplify);
   UNIT_ASSERT_EQUALS("..", FILpathSimplifyUnsafe(".."));
   UNIT_ASSERT_EQUALS("foo.txt", FILpathSimplifyUnsafe("./foo.txt"));
   UNIT_ASSERT_EQUALS("foo.txt", FILpathSimplifyUnsafe(".///foo.txt"));
   UNIT_ASSERT_EQUALS("/Users/testUser/dir/foo.txt", FILpathSimplifyUnsafe("/Users/testUser/dir/foo.txt"));
   UNIT_ASSERT_EQUALS("/Users/testUser/dir/foo.txt", FILpathSimplifyUnsafe("/Users/testUser/dir//foo.txt"));
   UNIT_ASSERT_EQUALS("/Users/testUser/dir/foo.txt", FILpathSimplifyUnsafe("/Users/testUser/dir/subdir/../foo.txt"));
}

static void testFILpathSimplifyCrash(){
   COL_FUNCTION(testFILpathSimplifyCrash);
   // Have a path with more than 256 directory separators
   COLstring BadPath = R"(/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/
                           abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/abc/foo")";
   COLstring Out;
   try {
      Out = FILpathSimplifyUnsafe(BadPath);
   } catch(const COLerror& Err){
      COL_TRC("FILpathSimplifyUnsafe throws as expected.");
      UNIT_ASSERT_EQUALS("Error: Invalid path (too many nested directories or excessive parent directory references).", Err.description());
   }
   COLstring ErrorString;
   bool Result = FILpathSimplify(BadPath, &Out, &ErrorString);
   UNIT_ASSERT(!Result);
   UNIT_ASSERT_EQUALS("Error: Invalid path (too many nested directories or excessive parent directory references).", ErrorString);
}

static void resultCompare(const COLstring& Expected, const COLstring& Actual){
   COL_FUNCTION(resultCompare);
   COLstring Input1(Expected);
   COLstring Input2(Actual);
   FILcorrectPathSeparators(Input1);
   FILcorrectPathSeparators(Input2);
   UNIT_ASSERT_EQUALS(Input1, Input2);
}

static void testFILpathAppend(){
   COL_FUNCTION(testFILpathAppend);
   // Test out empty input cases
   resultCompare("",   FILpathAppend("", ""));
   resultCompare("b",  FILpathAppend("", "b"));
   resultCompare("a/", FILpathAppend("a", ""));
   resultCompare("/",  FILpathAppend("/", ""));
   // Test extra separators
   resultCompare("a/b/c",    FILpathAppend("a//b///", "///c"));
   resultCompare("/a/b/c/d", FILpathAppend("//a///b", "c//d"));
   // Test path with extra / and .
   resultCompare("a/b/c/d/", FILpathAppend("a///b//", "//./c//./d//"));
   // Test paths with extra ..
   resultCompare("..",   FILpathAppend("a/b", "../../.."));   // one more than available
   resultCompare("../c", FILpathAppend("a/b", "../../../c"));
   // Test basic joins
   resultCompare("/",            FILpathAppend("/", "/"));
   resultCompare("a/b",          FILpathAppend("a", "b"));
   resultCompare("a/b/c",        FILpathAppend("a/b", "c"));
   resultCompare("/usr/bin",     FILpathAppend("/usr", "bin"));
   resultCompare("/usr/bin/",    FILpathAppend("/usr/", "bin/"));
   resultCompare("dir/file.txt", FILpathAppend("dir", "file.txt"));
   // Test paths with .
   resultCompare("a/b/",                  FILpathAppend("a/b", "."));
   resultCompare("c",                     FILpathAppend(".", "c"));
   resultCompare("a/b/c",                 FILpathAppend("a/./b", "./c"));
   resultCompare("/usr/local/bin",        FILpathAppend("/usr/./local", "./bin"));
   resultCompare("project/include/foo.h", FILpathAppend("project/src", "./../include/./foo.h"));
   // Test paths with ..
   resultCompare("a/",               FILpathAppend("a/b", ".."));
   resultCompare("a/c",              FILpathAppend("a/b", "../c"));
   resultCompare("a/d",              FILpathAppend("a/b/c", "../../d"));
   resultCompare("/usr/bin",         FILpathAppend("/usr/local", "../bin"));
   resultCompare("/",                FILpathAppend("/usr/local/", "../../"));
   resultCompare("../b",             FILpathAppend("a", "../../b")); // go above a
   resultCompare("../../b",          FILpathAppend("", "../../b") );
   resultCompare("/var/tmp/app.log", FILpathAppend("/var/log", "../tmp/app.log"));
   resultCompare("a/b/z",            FILpathAppend("a/b/c/d/e", "../../../x/y/../../z"));
   // Test paths with '~'. The '~' wont be expanded since FILpathAppend doesn't call FILexpand.
   resultCompare("/home/user/~",      FILpathAppend("/home/user", "~"));
   resultCompare("/home/user/~/docs", FILpathAppend("/home/user", "~/docs"));
}

static void testFILpathExtLast(){
   COL_FUNCTION(testFILpathExtLast);
   // Test basic cases 
   UNIT_ASSERT_EQUALS(".cpp", FILpathExtLast("file.cpp", true));
   UNIT_ASSERT_EQUALS("cpp",  FILpathExtLast("file.cpp", false));
   UNIT_ASSERT_EQUALS(".gz",  FILpathExtLast("archive.tar.gz", true));
   UNIT_ASSERT_EQUALS("gz",   FILpathExtLast("archive.tar.gz", false));
   UNIT_ASSERT_EQUALS("",     FILpathExtLast("noext", true));
   UNIT_ASSERT_EQUALS("",     FILpathExtLast("noext", false));
   // Test . before file 
   UNIT_ASSERT_EQUALS("",     FILpathExtLast("/dir.with.dots/name", true));
   UNIT_ASSERT_EQUALS("",     FILpathExtLast("/a.b/c.d/file", true));
   UNIT_ASSERT_EQUALS(".ext", FILpathExtLast("/a.b/c.d/file.ext", true));
   // Test hidden file
   UNIT_ASSERT_EQUALS(".bashrc", FILpathExtLast(".bashrc", true));
   UNIT_ASSERT_EQUALS("bashrc",  FILpathExtLast(".bashrc", false));
   // Test trailing dot
   UNIT_ASSERT_EQUALS(".", FILpathExtLast("a.", true));
   UNIT_ASSERT_EQUALS("",  FILpathExtLast("a.", false));
   // Test multiple dots
   UNIT_ASSERT_EQUALS(".b",    FILpathExtLast("a..b", true));
   UNIT_ASSERT_EQUALS("b",     FILpathExtLast("a..b", false));
   UNIT_ASSERT_EQUALS(".dots", FILpathExtLast("multi....dots", true));
   UNIT_ASSERT_EQUALS("dots",  FILpathExtLast("multi....dots", false));
   // Other edge cases
   UNIT_ASSERT_EQUALS("", FILpathExtLast("/path/to/file", true));
   UNIT_ASSERT_EQUALS("", FILpathExtLast("", true));
}

static void testFILpathNameWithoutLastExt(){
   COL_FUNCTION(testFILpathNameWithoutLastExt);
   // Test basic cases
   UNIT_ASSERT_EQUALS("",            FILpathNameWithoutLastExt(""));
   UNIT_ASSERT_EQUALS("noext",       FILpathNameWithoutLastExt("noext"));
   UNIT_ASSERT_EQUALS("name",        FILpathNameWithoutLastExt("/dir/name"));
   UNIT_ASSERT_EQUALS("file",        FILpathNameWithoutLastExt("/path/to/file.cpp"));
   UNIT_ASSERT_EQUALS("archive.tar", FILpathNameWithoutLastExt("archive.tar.gz"));
   // Test hidden file
   UNIT_ASSERT_EQUALS("",            FILpathNameWithoutLastExt(".bashrc"));
   // Test dots
   UNIT_ASSERT_EQUALS("",            FILpathNameWithoutLastExt("."));
   UNIT_ASSERT_EQUALS("trailingdot", FILpathNameWithoutLastExt("trailingdot."));
   UNIT_ASSERT_EQUALS("c.d.e",       FILpathNameWithoutLastExt("c.d.e."));
   UNIT_ASSERT_EQUALS("a.",          FILpathNameWithoutLastExt("a..b"));
}

static void testFILpathName(){
   COL_FUNCTION(testFILpathName);
   // Test empty file names
   UNIT_ASSERT_EQUALS("", FILpathName(""));
   UNIT_ASSERT_EQUALS("", FILpathName("/a/b/"));
   UNIT_ASSERT_EQUALS("", FILpathName("/"));
   UNIT_ASSERT_EQUALS("", FILpathName("////"));
   // Test basic file names
   UNIT_ASSERT_EQUALS("c",        FILpathName("a/b/c"));
   UNIT_ASSERT_EQUALS("c.txt",    FILpathName("/a/b/c.txt"));
   UNIT_ASSERT_EQUALS("relative", FILpathName("relative"));
   // Test dots
   UNIT_ASSERT_EQUALS(".",  FILpathName("."));
   UNIT_ASSERT_EQUALS("..", FILpathName(".."));
   UNIT_ASSERT_EQUALS(".",  FILpathName("a/b/."));
   UNIT_ASSERT_EQUALS("..", FILpathName("a/b/.."));
   // Test hidden files
   UNIT_ASSERT_EQUALS(".hidden",  FILpathName(".hidden"));
   UNIT_ASSERT_EQUALS(".c",       FILpathName("/a.b/.c"));
   // Test strange file paths
   UNIT_ASSERT_EQUALS("file.ext", FILpathName("/dir.name/file.ext"));
   UNIT_ASSERT_EQUALS("c",        FILpathName("///a//b///c"));
}

static void testFILpathResolve(){
   COL_FUNCTION(testFILpathResolve);
   COLstring WorkingDir = FILworkingDir();
   // Test basic cases
   UNIT_ASSERT_EQUALS(WorkingDir,                           FILpathResolve(""));
   UNIT_ASSERT_EQUALS(WorkingDir,                           FILpathResolve("", WorkingDir));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "myfile"),  FILpathResolve("myfile"));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "myfile"),  FILpathResolve("myfile", WorkingDir));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "foo"),     FILpathResolve("./foo"));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "foo"),     FILpathResolve("./foo", WorkingDir));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "foo/bar"), FILpathResolve("foo/bar"));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, "foo/bar"), FILpathResolve("foo/bar", WorkingDir));
   // Try custom working directory
   UNIT_ASSERT_EQUALS(FILpathAppend("/Users/testUser", "foo/bar"), FILpathResolve("foo/bar", "/Users/testUser"));
   UNIT_ASSERT_EQUALS(FILpathAppend("/Users/testUser", "foo/bar"), FILpathResolve("foo/bar", "/Users/testUser/"));
   UNIT_ASSERT_EQUALS(FILpathAppend("/Users/", "foo/bar"),         FILpathResolve("foo/bar", "/Users/testUser/../"));
   // Test hidden file
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, ".bashrc"), FILpathResolve(".bashrc"));
   UNIT_ASSERT_EQUALS(FILpathAppend(WorkingDir, ".bashrc"), FILpathResolve(".bashrc", WorkingDir));
}

static void testFILpathDir(){
   COL_FUNCTION(testFILpathDir);
   // Test empty input
   resultCompare("", FILpathDir("", true));
   resultCompare("", FILpathDir("", false));   
   // Test file and hidden file
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/gcc", true));
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/gcc", false));
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/.boo", true));
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/.boo", false));
   // Test directory with and without trailing slash
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/", true));
   resultCompare("/usr/local/bin/", FILpathDir("/usr/local/bin/", false));
   resultCompare("/usr/local/",     FILpathDir("/usr/local/bin", true));
   resultCompare("/usr/local/",     FILpathDir("/usr/local/bin", false));
   // Test /
   resultCompare("/", FILpathDir("/file", true));
   resultCompare("/", FILpathDir("/file", false));
   resultCompare("/", FILpathDir("/", true));
   resultCompare("/", FILpathDir("/", false));
   // Test relative
   resultCompare("a/b/", FILpathDir("a/b/", true));
   resultCompare("a/b/", FILpathDir("a/b/", false));
   resultCompare("",     FILpathDir("file.txt", true));
   resultCompare("",     FILpathDir("file.txt", false));
   // Test path with . and ..
   resultCompare("",          FILpathDir(".", true));
   resultCompare("",          FILpathDir(".", false));
   resultCompare("",          FILpathDir("./file.txt", true));
   resultCompare("./",        FILpathDir("./file.txt", false));
   resultCompare("",          FILpathDir("..", true));
   resultCompare("",          FILpathDir("..", false));
   resultCompare("../",       FILpathDir("../", true));
   resultCompare("../",       FILpathDir("../", false));
   resultCompare("a/c/",      FILpathDir("a/b/../c/file", true));
   resultCompare("a/b/../c/", FILpathDir("a/b/../c/file", false));
   resultCompare("../a/",     FILpathDir("../a/b", true));
   resultCompare("../a/",     FILpathDir("../a/b", false));
   // Test path with extra separators
   resultCompare("/a/b/c/",      FILpathDir("/a/./b//c///file", true));
   resultCompare("/a/./b//c///", FILpathDir("/a/./b//c///file", false));
}

void testFILpathUtils(UNITapp* pApp){
   pApp->add("pathUtils/FILpathSimplifyUnsafe",     &testFILpathSimplify);
   pApp->add("pathUtils/FILpathSimplifyCrash",      &testFILpathSimplifyCrash); // IX-4181
   pApp->add("pathUtils/FILpathAppend",             &testFILpathAppend);
   pApp->add("pathUtils/FILpathExtLast",            &testFILpathExtLast);
   pApp->add("pathUtils/FILpathNameWithoutLastExt", &testFILpathNameWithoutLastExt);
   pApp->add("pathUtils/FILpathName",               &testFILpathName);
   pApp->add("pathUtils/FILpathResolve",            &testFILpathResolve);
   pApp->add("pathUtils/FILpathDir",                &testFILpathDir);
}
