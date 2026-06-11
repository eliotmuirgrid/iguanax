//---------------------------------------------------------------------------
// Copyright (C) 1997-2004 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: FMTexample class
//
// Description:
//
// Test for FMTlineFeedConvert
// 
// Author: Yunqi Zhang
// Date:   Fri 24/09/2004 
//
//---------------------------------------------------------------------------
#include <FMTexample/FMTexamplePrecomp.h>
#pragma hdrstop


void testLineFeedConvert()
{
   char Test1[] = "abcde\rg";
   FMTlineFeedConvert Convert(Test1,"f");
   COLstring Result;
   COLostream Stream(Result);
   Convert.PrintOn(Stream);
   TST_ASSERT(Result == "abcdefg")
  
   FMTlineFeedConvert Convert2(Test1,"\n");
   Stream<<(Stream,Convert2);
   TST_ASSERT(Result == "abcdefgabcde\ng")

   char Test3[] = "Hello\0DBye";
   FMTlineFeedConvert Convert3(Test3,"\0");
   Convert3.PrintOn(Stream);
   TST_ASSERT(Result =="abcdefgabcde\ngHello");

   //test the new makePrintable method
   FMTlineFeedConvert Convert4("MSH|\rPID|@\r\n","\r\n");
   Convert4.PrintOn(Stream);
   TST_ASSERT(Result =="abcdefgabcde\ngHelloMSH|\r\nPID|@\r\n");

}

TSTtestable* FMTlineFeedConvertTest()
{
   TST_START_COLLECTION("FMTlineFeedConvert Test")
      TST_ADD_TEST_FUNCTION(testLineFeedConvert);   
   TST_END_COLLECTION
}

