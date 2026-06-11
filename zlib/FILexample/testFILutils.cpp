//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFILutils
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tue 18 Nov 2014 10:23:15 EST
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>
#include <vector>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testFULremoveNative() {
   COL_FUNCTION(testFULremoveNative);
   const COLstring RemovedDir = FILpathAppend("testFILremoveDir", "");
   if(!FILfileExists(RemovedDir)) {
      FILcreateDirectoryTreeToFile(FILpathAppend(RemovedDir, "subdir/test.txt"), FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
   }
   COLstring Err;
   FILremoveFullDirNewSafe(RemovedDir, &Err);
   COL_VAR2(RemovedDir, Err);
   UNIT_ASSERT(Err.is_null());
}

static void testFILexecutableDir(){
   COL_FUNCTION(testFILexecutableDir);
   COLstring ExeDir = FILexecutableDir();
   COL_VAR(ExeDir);
   UNIT_ASSERT(FILfileExists(ExeDir + "testFILutils.cpp"));
}

static void testFILexecutableFileName(){
   COL_FUNCTION(testFILexecutableFileName);
   COL_VAR(FILexecutableFileName(false));
#ifndef _WIN32
   UNIT_ASSERT_EQUALS(FILexecutableFileName(true), "FILexample"); // No .exe extension ever
   UNIT_ASSERT_EQUALS(FILexecutableFileName(), "FILexample"); // No .exe extension ever
#endif
   UNIT_ASSERT_EQUALS(FILexecutableFileName(false), "FILexample");
#ifdef _WIN32
   UNIT_ASSERT_EQUALS(FILexecutableFileName(true), "FILexample.exe");
#endif
}

static void testFILreadWrite(){
   COL_FUNCTION(testFILreadWrite);
   COLstring Input = "sdfasdf;jkdhpaiqw ehfpwuhefpuwhefpuhefuhphueesadfas";
   COLstring Output;
   FILwriteFile("writeexample.txt", Input);
   FILreadFile("writeexample.txt", &Output);
   UNIT_ASSERT_EQUALS(Input, Output);
}

static void testFILrenameDirectory(){
   COL_FUNCTION(testFILrenameDirectory);
   COLstring Folder = "testFILrenameDirectory";
   COLstring NewFolder = "New_testFILrenameDirectory";
   int Count;

   Count = FILrenameDirectory(Folder, NewFolder, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, false);
   UNIT_ASSERT(FILfileExists(NewFolder));
   UNIT_ASSERT(FILisDirectory(NewFolder));
   UNIT_ASSERT_EQUALS(4, Count);

   Count = FILrenameDirectory(NewFolder, Folder, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX, false);
   UNIT_ASSERT(FILfileExists(Folder));
   UNIT_ASSERT(FILisDirectory(Folder));
   UNIT_ASSERT_EQUALS(4, Count);
}

static void testFILnameIsInvalid(){
   COL_FUNCTION(testFILnameIsInvalid);
   std::vector<COLstring> ReservedNames { "CON","PRN","AUX","NUL","COM1","COM2","COM3","COM4","COM5","COM6","COM7","COM8","COM9","LPT1","LPT2","LPT3","LPT4","LPT5","LPT6","LPT7","LPT8","LPT9" };
   for (auto Name : ReservedNames) {
      COLcout << Name <<newline;
      UNIT_ASSERT(FILnameIsInvalid(Name));
   }
   UNIT_ASSERT(FILnameIsInvalid("FOO."));
   UNIT_ASSERT(FILnameIsInvalid("foo<"));
   UNIT_ASSERT(FILnameIsInvalid("foo>"));
   UNIT_ASSERT(FILnameIsInvalid("foo:"));
   UNIT_ASSERT(FILnameIsInvalid("foo\""));
   UNIT_ASSERT(FILnameIsInvalid("foo/"));
   UNIT_ASSERT(FILnameIsInvalid("foo\\"));
   UNIT_ASSERT(FILnameIsInvalid("foo|"));
   UNIT_ASSERT(FILnameIsInvalid("foo?"));
   UNIT_ASSERT(FILnameIsInvalid("foo*"));
   UNIT_ASSERT(FILnameIsInvalid("foo\"bar"));
   UNIT_ASSERT(FILnameIsInvalid("foo/bar"));
   UNIT_ASSERT(FILnameIsInvalid("foo\\bar"));
}

static void testFILpathIsInvalid(){
   COL_FUNCTION(testFILpathIsInvalid);
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "LPT3" FIL_DIR_SEPARATOR "bar"));
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "&*()" FIL_DIR_SEPARATOR "bar"));
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "QDF." FIL_DIR_SEPARATOR "bar"));
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "bar" FIL_DIR_SEPARATOR "COM3"));
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "bar" FIL_DIR_SEPARATOR "~!>?"));
   UNIT_ASSERT( FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "bar" FIL_DIR_SEPARATOR "gew."));
   UNIT_ASSERT(!FILpathIsInvalid("foo"));
   UNIT_ASSERT(!FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "bar"));
   UNIT_ASSERT(!FILpathIsInvalid("foo" FIL_DIR_SEPARATOR "bar" FIL_DIR_SEPARATOR "log.txt"));
   UNIT_ASSERT(!FILpathIsInvalid(".foo" FIL_DIR_SEPARATOR ".bar" FIL_DIR_SEPARATOR ".asd.txt"));
}

void testFILutils(UNITapp* pApp){
   pApp->add("utils/FILexecutableDir", &testFILexecutableDir);
//   pApp->add("utils/FILexecutableFileName", &testFILexecutableFileName);
   pApp->add("utils/FILread/write", &testFILreadWrite);
   pApp->add("utils/FILrenameDirectory", &testFILrenameDirectory);
   pApp->add("utils/FILnameIsInvalid", &testFILnameIsInvalid);
   pApp->add("utils/FILpathIsInvalid", &testFILpathIsInvalid);
   pApp->add("utils/FILremoveNative", &testFULremoveNative);
}
