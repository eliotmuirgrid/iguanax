//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testFILfile
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Sun 21 Mar 2021 22:08:12 EDT
//---------------------------------------------------------------------------
#include <UNIT/UNITapp.h>

#include <FIL/FILutils.h>
#include <FIL/FILfile.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testFILfileOpen(){
   COL_FUNCTION(testFILfileOpen);
   FILfile Test;
   Test.open("example.txt", FILfile::Rewrite);
   COLstring Life="Life";
   Test.write(Life);
   Test.close();
   
   FILfile Test2;
   Test2.open("example.txt", FILfile::Read);
   UNIT_ASSERT(Test2.position() == 0);
   UNIT_ASSERT(Test2.size() == 4);
   COLstring Buffer;
   Test2.read(&Buffer);
   UNIT_ASSERT_EQUALS(Buffer, Life);
   UNIT_ASSERT(Test2.position() == 4);
   COL_VAR2(Buffer, Test2.size());
   UNIT_ASSERT(Test2.size() == 4);
}

static void testFILfileStream(){
   COL_FUNCTION(testFILfileStream);
   COLostream Stream(new FILfile("stream.txt", FILfile::Rewrite), true);
   Stream << "Hello world" << newline << "Streaming..." << newline << flush;
}

static void testFILdirNonExistent(){
   COL_FUNCTION(testFILdirNonExistent);
   bool Result = false;
   try{
      FILfile BadOpen("nonExistentDir/log.txt", FILfile::Write);
   } catch (COLerror& Error){
      COL_VAR(Error);
      Result = true;
   }
   UNIT_ASSERT(Result == true);
}

static void testFILbadPosition(){
   COL_FUNCTION(testFILbadPosition);
   FILfile Source("short.txt", FILfile::Rewrite);
   Source.write("This is a line.");
   Source.close();

   FILfile Dest("short.txt", FILfile::Read);
   Dest.setPosition(10000);
   COL_VAR(Dest.position());
   COLstring Buffer;
   Buffer.setCapacity(1024);
   Dest.read(&Buffer);
   COL_VAR(Buffer);
}

static void testFILbadNegative(){
   COL_FUNCTION(testFILbadNegative);
   COLstring TestData = "The quick brown fox jumped over the lazy dog!";
   FILfile Source("fox.txt", FILfile::Rewrite);
   Source.write(TestData);
   Source.close();
   bool Result = true;
   FILfile Dest("fox.txt", FILfile::Read);
   try{
      Dest.setPosition(-100);
   } catch(COLerror& Error){
      COL_VAR(Error);
      Result = true;
   }
   UNIT_ASSERT(Result == true);
}

static void testFILappend(){
   COL_FUNCTION(testFILappend);
   COLstring Life = "Life";
   FILwriteFile("append.txt", Life);
   FILfile Appender("append.txt", FILfile::Append);
   Appender.write(Life);
   Appender.close();
   COLstring Result;
   FILreadFile("append.txt", &Result);
   UNIT_ASSERT_EQUALS(Result, Life+Life);
}

void testFILfile(UNITapp* pApp){
   pApp->add("file/open", &testFILfileOpen);
   pApp->add("file/stream", &testFILfileStream);
   pApp->add("file/dir/nonexistent", &testFILdirNonExistent);
   pApp->add("file/position/bad", &testFILbadPosition);
   pApp->add("file/position/negative", &testFILbadNegative);
   pApp->add("file/append", &testFILappend);
}

