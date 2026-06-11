//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testMZIP.cpp
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, 13th March 2014
//---------------------------------------------------------------------------

#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <MZIP/MZIPzip.h>
#include <MZIP/MZIPunzip.h>
#include <MZIP/MZIPvar.h>

#include <COL/COLvar.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testSimple(){
   COL_FUNCTION(testSimple);
   COLstring RemoveError;
   if (FILfileExists("blah")){ FILremoveFullDirNewSafe("blah", &RemoveError); }
   if (FILfileExists("blah2")){ FILremoveFullDirNewSafe("blah2", &RemoveError); }
   if (FILfileExists("blah.zip")){ FILremoveWithThrow("blah.zip"); }
   FILmakeDir("blah", FIL_USER_RWX);
   FILmakeDir("blah/deblah", FIL_USER_RWX);
   COLarray<COLstring> FileList;
   for (int i=1;i <5; i++){
      FileList.push_back("blah/"+COLintToString(i)+".txt");
      FILwriteFile(FileList.back(), COLintToString(i));
      FileList.push_back("blah/deblah/"+COLintToString(i)+".txt");
      FILwriteFile(FileList.back(), COLintToString(i));
   }
   COLstring WorkingDir = FILworkingDir();
   COL_VAR(WorkingDir);

   // test a bad zip file path
   COLstring Err = "this string should be cleared by next function";
   int rc = MZIPzipFiles(":://///../../this/is/a/bad/path/for/a/zipfile.zip", FileList, WorkingDir, &Err);
   COL_VAR(Err);
   UNIT_ASSERT_EQUALS("Cannot open or create :://///../../this/is/a/bad/path/for/a/zipfile.zip", Err);
   UNIT_ASSERT_EQUALS(MZIP_FAILURE, rc);
   // test a bad zip input file
   FileList.push_back("some/non/existent/file.baz"); // add a bad file to FileList
   rc = MZIPzipFiles("blah_test_bad_input_file.zip", FileList, WorkingDir, &Err);
   COL_VAR(Err);
   UNIT_ASSERT_EQUALS("Cannot open file " + WorkingDir + "some/non/existent/file.baz for reading.", Err);
   UNIT_ASSERT_EQUALS(MZIP_FAILURE, rc);
   FileList.pop_back(); // remove the bad file from FileList
   // test making a good zip file
   rc = MZIPzipFiles("blah.zip", FileList, WorkingDir, &Err);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);

   // test unzipping.
   COLarray<COLstring> FileList2;
   FILmakeDir("blah2", FIL_USER_RWX);
   COLstring Dump;
   COLostream Stream(Dump);
   MZIPunzipUnpackZipFile("blah.zip", "blah2", true, NULL, &Stream, &FileList2);
   COL_VAR(Dump);
}

/*
void testOld(){
   COL_FUNCTION(testOld);
   if (FILfileExists("blah")){
      FILremoveFullDirNew("blah");
   }
   if (FILfileExists("blah2")){
      FILremoveFullDirNew("blah2");
   }
   if (FILfileExists("blah.zip")){
      FILremoveWithThrow("blah.zip");
   }

   COL_TRC("Create blah directory and some stuff in it.");
   FILmakeDir("blah", FIL_USER_RWX);
   FILmakeDir("blah/deblah", FIL_USER_RWX);
   COLarray<COLstring> FileList;
   for (int i=1;i <5; i++){
      FileList.push_back("blah/"+COLintToString(i)+"汉字汉字.txt");
      FILwriteFile(FileList.back(), COLintToString(i));
      FileList.push_back("blah/deblah/"+COLintToString(i)+"汉字汉字.txt");
      FILwriteFile(FileList.back(), COLintToString(i));
   }
   COLstring WorkingDir = FILworkingDir();
   COL_VAR(WorkingDir);

   // test a bad zip file path
   COLstring Err = "this string should be cleared by next function";
   int rc = MZIPzipFiles(":://///../../this/is/a/bad/path/for/a/zipfile.zip", FileList, WorkingDir, &Err);
   UNIT_ASSERT_EQUALS("Cannot open or create :://///../../this/is/a/bad/path/for/a/zipfile.zip", Err);
   UNIT_ASSERT_EQUALS(MZIP_FAILURE, rc);

   // test a bad zip input file
   FileList.push_back("some/non/existent/file.baz"); // add a bad file to FileList
   rc = MZIPzipFiles("blah_test_bad_input_file.zip", FileList, WorkingDir, &Err);
   COL_VAR(Err);
   UNIT_ASSERT_EQUALS("Cannot open file " + WorkingDir + "some/non/existent/file.baz for reading.", Err);
   UNIT_ASSERT_EQUALS(MZIP_FAILURE, rc);
   FileList.pop_back(); // remove the bad file from FileList

   // test making a good zip file
   rc = MZIPzipFiles("blah.zip", FileList, WorkingDir, &Err);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);

   // test making a good zip file with multibyte characters in its name and contents
   rc = MZIPzipFiles("汉字.zip", FileList, WorkingDir, &Err);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);

   COLarray<COLstring> FileList2;
   FILmakeDir("blah2", FIL_USER_RWX);
   MZIPunzipUnpackZipFile("blah.zip", "blah2", true, NULL, &COLcout, &FileList2);

   //COL_TRC("Clean directory");
   //FILremoveFullDir("blah");
}

static void testZipFilesToMemory()
{
   // set up
   COLarray<COLstring> FileList;
   for (int i=1;i <5; i++){
      FileList.push_back("blah/"+COLintToString(i)+"汉字汉字.txt");
      FILwriteFile(FileList.back(), COLintToString(i));
      FileList.push_back("blah/deblah/"+COLintToString(i)+"汉字汉字.txt");
      FILwriteFile(FileList.back(), COLintToString(i));
   }
   const COLstring WorkingDir = FILworkingDir();

   //
   // Test passing a bad zip input file for an in-memory zip
   //

   FileList.push_back("some/non/existent/file.baz"); // add a bad file to FileList

   COLstring MemZip = "some junk to be replaced";
   COLstring Err = "this string should be cleared by next function";
   int rc = MZIPzipFilesToMemory(FileList, WorkingDir, &MemZip, &Err);
   UNIT_ASSERT_EQUALS("Cannot open file " + WorkingDir + "some/non/existent/file.baz for reading.", Err);
   UNIT_ASSERT_EQUALS(MZIP_FAILURE, rc);
   UNIT_ASSERT_EQUALS(0, MemZip.size());
   FileList.pop_back(); // remove the bad file from FileList

   //
   // Test making a good in-memory zip
   //

   rc = MZIPzipFilesToMemory(FileList, WorkingDir, &MemZip, &Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);

   COLstring ContentVerify;
   FILreadFile("blah.zip", &ContentVerify);

   if (FILfileExists("blah.zip")){
      FILremoveWithThrow("blah.zip");
   }

   rc = MZIPzipFiles("blah.zip", FileList, WorkingDir, &Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(1062, MemZip.size());
   UNIT_ASSERT_EQUALS(1062, ContentVerify.size());
   UNIT_ASSERT_EQUALS(ContentVerify.size(), MemZip.size());

   // We cannot compare zip images because something about zipping is not deterministic.
   // It might be dates, might be unused fields.
   COL_DUMP("testZipFilesToMemory:MemZip", MemZip.c_str(), MemZip.size());
   COL_DUMP("testZipFilesToMemory:ContentVerify", ContentVerify.c_str(), ContentVerify.size());
   FILwriteFile("blah_testZipFilesToMemory.zip", MemZip);
   //UNIT_ASSERT_EQUALS(0, ::memcmp(MemZip.c_str(), ContentVerify.c_str(), ContentVerify.size()));

   COLvar Var1, Var2;
   rc = MZIPunzipToVar(MemZip.c_str(),        MemZip.size(),        NULL, false, &Var1, &Err);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   rc = MZIPunzipToVar(ContentVerify.c_str(), ContentVerify.size(), NULL, false, &Var2, &Err);
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT_EQUALS(Var2.json(), Var1.json());
}

void testMemoryOpen(){
   COL_FUNCTION(testMemoryOpen);
   COLstring Content;
   FILreadFile("blah.zip", &Content);
   unzFile File = MZIPunzipOpenMem(Content.c_str(), Content.size());
   UNIT_ASSERT(File != NULL);
   unzClose(File);
}

void testBadMemoryOpen(){
   COL_FUNCTION(testBadMemoryOpen);
   {
      // read in a good zip file into memory
      COLstring Content;
      FILreadFile("blah.zip", &Content);

      // verify that it is valid
      unzFile File = MZIPunzipOpenMem(Content.c_str(), Content.size());
      UNIT_ASSERT(File != NULL);
      unzClose(File);

      // verify that partially written zip files should fail
      //const int N = Content.size();
      const int N = 850;
      COLASSERT(Content.size() >= N);
      for (int n = 0; n <= N; ++n) {
         //COL_DUMP("testMemoryOpen bad contents", Content.c_str(), n);
         File = MZIPunzipOpenMem(Content.c_str(), n);
         if (File) {
            COL_TRC(n << " byte partial zip succeeds");
         } else {
            COL_TRC(n << " byte partial zip fails");
         }
         UNIT_ASSERT_EQUALS(0, File);
         unzClose(File);
      }
   }
   {
      COLstring BadZipContents("Junk Memory - not a zip file.");
      for (int n = 0; n <= BadZipContents.size(); ++n) {
         unzFile File = MZIPunzipOpenMem(BadZipContents.c_str(), n);
         UNIT_ASSERT_EQUALS(0, File);
         unzClose(File);
      }
   }
}*/

static void testBadMemZip(){
   COL_FUNCTION(testBadMemZip);
   COLstring BinaryZip;
   COLstring Err;

   COLvar Var;
   int rc = MZIPzipVar(Var, &BinaryZip, &Err);
   COL_DUMP("empty BinaryZip", BinaryZip.c_str(), BinaryZip.size());
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT(BinaryZip.size() > 0);

   Var.setArrayType();
   Var.push_back("abc");
   rc = MZIPzipVar(Var, &BinaryZip, &Err);
   COL_DUMP("test BinaryZip with array", BinaryZip.c_str(), BinaryZip.size());
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT(BinaryZip.size() > 0);

   Var = "Foo Bar";
   rc = MZIPzipVar(Var, &BinaryZip, &Err);
   COL_DUMP("test BinaryZip with a string", BinaryZip.c_str(), BinaryZip.size());
   UNIT_ASSERT_EQUALS("", Err);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT(BinaryZip.size() > 0);
}

void testVarZipAndUnzip2(){
   COL_FUNCTION(testVarZipAndUnzip2);
   COLstring Content;
   FILreadFile("example.zip", &Content);
   COLvar Result;
   COLstring Error;
   // TODO - is password in the right spot?
   const char* Password = 0;
   const bool WantMetaData = true;
   int ResultCode = MZIPunzipToVar(Content.c_str(), Content.size(), Password, WantMetaData, &Result, &Error);
   COL_VAR2(ResultCode, Result);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, ResultCode);
   UNIT_ASSERT_EQUALS("{\"aaa\":\"This file is called \\\"aaa\\\".\\n\",\"aaa::date\":\"2014-03-17 13:20:18\",\"aaa::mode\":\"100644\",\"bbb.txt\":\"this file\\nis 'bbb.txt'.\\n\",\"bbb.txt::date\":\"2014-03-17 13:21:26\",\"bbb.txt::mode\":\"100400\",\"empty.txt\":\"\",\"empty.txt::date\":\"2014-03-17 13:38:08\",\"empty.txt::mode\":\"100644\",\"empty_dir\":{},\"empty_dir::date\":\"2014-03-17 13:19:52\",\"empty_dir::mode\":\"40755\",\"f400\":\"\",\"f400::date\":\"2014-03-17 17:04:12\",\"f400::mode\":\"100400\",\"f440\":\"\",\"f440::date\":\"2014-03-17 17:04:06\",\"f440::mode\":\"100440\",\"f444\":\"\",\"f444::date\":\"2014-03-17 17:18:04\",\"f444::mode\":\"100444\",\"f450\":\"\",\"f450::date\":\"2014-03-17 17:04:02\",\"f450::mode\":\"100450\",\"f460\":\"\",\"f460::date\":\"2014-03-17 17:04:00\",\"f460::mode\":\"100460\",\"f470\":\"\",\"f470::date\":\"2014-03-17 17:03:58\",\"f470::mode\":\"100470\",\"f500\":\"\",\"f500::date\":\"2014-03-17 16:59:36\",\"f500::mode\":\"100500\",\"f555\":\"\",\"f555::date\":\"2014-03-17 17:18:04\",\"f555::mode\":\"100555\",\"f600\":\"\",\"f600::date\":\"2014-03-17 16:59:52\",\"f600::mode\":\"100600\",\"f666\":\"\",\"f666::date\":\"2014-03-17 17:18:04\",\"f666::mode\":\"100666\",\"f700\":\"\",\"f700::date\":\"2014-03-17 16:59:30\",\"f700::mode\":\"100700\",\"f777\":\"\",\"f777::date\":\"2014-03-17 17:18:04\",\"f777::mode\":\"100777\",\"test_dir\":{\"aaa\":\"content of aaa\\n\",\"aaa::date\":\"2014-03-17 13:24:46\",\"aaa::mode\":\"100644\",\"bbb.txt\":\"Contents of 'bbb.txt'.\\n\",\"bbb.txt::date\":\"2014-03-17 13:25:36\",\"bbb.txt::mode\":\"100644\",\"empty_directory1\":{},\"empty_directory1::date\":\"2014-03-17 12:20:28\",\"empty_directory1::mode\":\"40755\",\"empty_directory2\":{},\"empty_directory2::date\":\"2014-03-17 12:20:30\",\"empty_directory2::mode\":\"40700\",\"empty_directory3\":{},\"empty_directory3::date\":\"2014-03-17 12:20:30\",\"empty_directory3::mode\":\"40755\",\"empty_file.txt\":\"\",\"empty_file.txt::date\":\"2014-03-17 12:30:16\",\"empty_file.txt::mode\":\"100644\",\"foo\":{\"bar\":{\"baz.txt\":\"some test data or another.\\n\",\"baz.txt::date\":\"2014-03-17 13:37:14\",\"baz.txt::mode\":\"100644\"},\"bar::date\":\"2014-03-17 13:36:44\",\"bar::mode\":\"40755\",\"zzz\":\"Zzzzzzzzzz...\\n\",\"zzz::date\":\"2014-03-17 13:34:00\",\"zzz::mode\":\"100644\"},\"foo::date\":\"2014-03-17 13:36:32\",\"foo::mode\":\"40755\",\"mydiffs\":\"#!/bin/bash\\n\\nARGS=\\\".\\\"\\nif [ x\\\"$*\\\" != x\\\"\\\" ]; then\\n\\tARGS=$*\\nfi\\n\\nhg diff $ARGS | less\\n\\n\",\"mydiffs::date\":\"2014-03-17 12:20:42\",\"mydiffs::mode\":\"100755\"},\"test_dir::date\":\"2014-03-17 13:33:30\",\"test_dir::mode\":\"40755\"}", Result.json());

   // test in-memory conversion of a COLvar to a binary zip image in a COLstring
   COLstring BinaryZip;
   int rc = MZIPzipVar(Result, &BinaryZip, &Error);
   COL_VAR2(rc, Error);
   COL_DBG("BinaryZip = " << COLvar(BinaryZip));
   UNIT_ASSERT_EQUALS("", Error);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   UNIT_ASSERT(BinaryZip.size() > 0);
   FILwriteFile("example_testVarZipAndUnzip2.zip", BinaryZip);

   // test converting an in-memory binary zip image back to a COLvar without meta-data
   COLvar NuVar;
   rc = MZIPunzipToVar(BinaryZip.c_str(), BinaryZip.size(), Password, false, &NuVar, &Error);
   COL_VAR2(rc, Error);
   UNIT_ASSERT_EQUALS("", Error);
   UNIT_ASSERT_EQUALS(MZIP_SUCCESS, rc);
   COL_VAR(NuVar.json(1));
   //COL_VAR(COLvar(NuVar.json()));
   UNIT_ASSERT_EQUALS("{\"aaa\":\"This file is called \\\"aaa\\\".\\n\",\"bbb.txt\":\"this file\\nis 'bbb.txt'.\\n\",\"empty.txt\":\"\",\"empty_dir\":{},\"f400\":\"\",\"f440\":\"\",\"f444\":\"\",\"f450\":\"\",\"f460\":\"\",\"f470\":\"\",\"f500\":\"\",\"f555\":\"\",\"f600\":\"\",\"f666\":\"\",\"f700\":\"\",\"f777\":\"\",\"test_dir\":{\"aaa\":\"content of aaa\\n\",\"bbb.txt\":\"Contents of 'bbb.txt'.\\n\",\"empty_directory1\":{},\"empty_directory2\":{},\"empty_directory3\":{},\"empty_file.txt\":\"\",\"foo\":{\"bar\":{\"baz.txt\":\"some test data or another.\\n\"},\"zzz\":\"Zzzzzzzzzz...\\n\"},\"mydiffs\":\"#!/bin/bash\\n\\nARGS=\\\".\\\"\\nif [ x\\\"$*\\\" != x\\\"\\\" ]; then\\n\\tARGS=$*\\nfi\\n\\nhg diff $ARGS | less\\n\\n\"}}", NuVar.json());
}

// TODO - could port over more unit tests if there was value in it?
void testMZIP(UNITapp* pApp){
    pApp->add("file/simple", &testSimple);
    pApp->add("var/testBadMemZip", &testBadMemZip);
    pApp->add("var/zipAndUnzip2", &testVarZipAndUnzip2);
}
