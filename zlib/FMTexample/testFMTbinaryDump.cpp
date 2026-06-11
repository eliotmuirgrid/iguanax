//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FMTexample class
//
// Description:
//
// Test for FMTbinaryDump
// 
// Author: Yunqi Zhang
// Date:   Fri 24/09/2004 
//
//---------------------------------------------------------------------------
#include <FMTexample/FMTexamplePrecomp.h>
#pragma hdrstop

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>
#include <DIF/DIFdiffer.h>

#include <COL/COLsinkString.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


static void convertFileToUnix(const COLstring& InFile, const COLstring& OutFile){
   COL_FUNCTION(convertFileToUnix);
   COLstring ExpectedUnix;
   FILreadFile(InFile, &ExpectedUnix);
   ExpectedUnix.replace("\r\n", "\n");
   FILwriteFile(OutFile, ExpectedUnix);
}

static void testBinaryDump() {
   
   //Test FMTtoHex
   COLstring Message1;
   COLsinkString Sink(Message1);
   FMTtoHex(Sink, 'C'); 
   Message1+=" ";
   FMTtoHex(Sink, 'a');
   Message1+=" ";
   FMTtoHex(Sink, '5');
   Message1+=" ";
   FMTtoHex(Sink, '@');
   Message1+=" ";
   FMTtoHex(Sink, ']');
   Message1+=" ";
   FMTtoHex(Sink, '\n');
   Message1+=" ";
   FMTtoHex(Sink, '\0');
   Message1+=" ";
   FMTtoHex(Sink, '0');
   Message1+=" ";
   FMTtoHex(Sink, 200);
   TST_ASSERT(Message1 =="43 61 35 40 5D 0A 00 30 C8")

   //Test FMTfromHex
   TST_ASSERT(FMTfromHex('4','3') == 'C');
   TST_ASSERT(FMTfromHex('6','1') == 'a');
   TST_ASSERT(FMTfromHex('5','D') == ']');
   TST_ASSERT(FMTfromHex('2','0') == ' ');
   TST_ASSERT(FMTfromHex('2','b') == '+');

   //Test FMTbinaryDump
   FILfile TestFile("test.txt",FILfile::Rewrite);
   COLostream Stream(TestFile);
   Stream.setNewLine("\n");

   COLstring TestString = "Hello World!";
   FMTbinaryDump(*Stream.sink(),(const COLuint8*)TestString.c_str(),TestString.size(), "\n");

   TestString = "#2@947#&Hd;a`/\n";
   
   FMTbinaryDump(*Stream.sink(),(const COLuint8*)TestString.c_str(),TestString.size(), "\n");
   
   TestString = COLstring(1, (char)13);
   FMTbinaryDump(*Stream.sink(),(const COLuint8*)TestString.c_str(),TestString.size(), "\n");

   FILfile aFile("long_text.txt",FILfile::Read);
   
   char Buffer[0x1000];
   COLuint32 AmountRead = 0;
   while ((AmountRead = aFile.read(Buffer,sizeof(Buffer)))) {
      FMTbinaryDump(*Stream.sink(),(const COLuint8*)Buffer,AmountRead, "\n");
   }

   aFile.close();
   TestFile.close();
   
   //Test FMTbinaryDump on a big file and display the time taken
   COLstring BigString = "This is a big string.........................";
   for(COLuint32 Index = 0; Index < 10000; Index++) {
      BigString +="This is a biiiig string.........................";
   }
   COLuint64 TimeTaken;
   COLstring text="";
   {
      COL_ENT("FMTbinaryDump of big string");
      COLsinkString Sink(text);
      FMTbinaryDump(Sink,(const COLuint8*)BigString.c_str(),BigString.size(), "\n");    
   }
   DIFdiffer Differ;

   convertFileToUnix("expected_out.txt", "expected_unix.txt");
   
   if (!Differ.filesMatch("expected_unix.txt", "test.txt", COLcerr) ) {
      COL_ERROR_STREAM("Expected output expected_unix.txt did not match test.txt", COLerror::PreCondition);   
   }
}

static void testFMTtoHexStream()
{
   COLstring Result;
   COLostream ResultStream(Result);
   FMTtoHex(ResultStream, (COLuint8)0xC9);
   FMTtoHex(ResultStream, (COLuint8)0x0f);
   FMTtoHex(ResultStream, (COLuint8)0x1e);
   FMTtoHex(ResultStream, (COLuint8)0x2d);
   FMTtoHex(ResultStream, (COLuint8)0x3c);
   FMTtoHex(ResultStream, (COLuint8)0x4b);
   FMTtoHex(ResultStream, (COLuint8)0x5a);
   FMTtoHex(ResultStream, (COLuint8)0x69);
   FMTtoHex(ResultStream, (COLuint8)0x78);
   FMTtoHex(ResultStream, (COLuint8)0x87);
   FMTtoHex(ResultStream, (COLuint8)0x96);
   FMTtoHex(ResultStream, (COLuint8)0xa5);
   FMTtoHex(ResultStream, (COLuint8)0xb4);
   FMTtoHex(ResultStream, (COLuint8)0xc3);
   FMTtoHex(ResultStream, (COLuint8)0xd2);
   FMTtoHex(ResultStream, (COLuint8)0xe1);
   FMTtoHex(ResultStream, (COLuint8)0xf0);
   TST_ASSERT_EQUALS("C90F1E2D3C4B5A69788796A5B4C3D2E1F0", Result);
}

static void testFMTtoHexSink()
{
   COLstring ResultString;
   COLsinkString Result(ResultString);
   FMTtoHex(Result, (COLuint8)0x0f);
   FMTtoHex(Result, (COLuint8)0x1e);
   FMTtoHex(Result, (COLuint8)0x2d);
   FMTtoHex(Result, (COLuint8)0x3c);
   FMTtoHex(Result, (COLuint8)0x4b);
   FMTtoHex(Result, (COLuint8)0x5a);
   FMTtoHex(Result, (COLuint8)0x69);
   FMTtoHex(Result, (COLuint8)0x78);
   FMTtoHex(Result, (COLuint8)0x87);
   FMTtoHex(Result, (COLuint8)0x96);
   FMTtoHex(Result, (COLuint8)0xa5);
   FMTtoHex(Result, (COLuint8)0xb4);
   FMTtoHex(Result, (COLuint8)0xc3);
   FMTtoHex(Result, (COLuint8)0xd2);
   FMTtoHex(Result, (COLuint8)0xe1);
   FMTtoHex(Result, (COLuint8)0xf0);
   FMTtoHex(Result, (COLuint8)0x9C);
   TST_ASSERT_EQUALS("0F1E2D3C4B5A69788796A5B4C3D2E1F09C", ResultString);
}

TSTtestable* FMTbinaryDumpTest()
{
   TST_START_COLLECTION("FMTbinaryDump Test")
      // Eliot - screw testing something which doesn't get broken and is a brittle source of noise.
      //TST_ADD_TEST_FUNCTION(testBinaryDump);   
      TST_ADD_TEST_FUNCTION(testFMTtoHexStream);
      TST_ADD_TEST_FUNCTION(testFMTtoHexSink);
   TST_END_COLLECTION
}

