//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
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

#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <FIL/FILfile.h>
#include <FIL/FILutils.h>

#include <DIS/DISbinaryDisplay.h>

static COLstring makeTestString()
{
   COLstring TestString;
   for (COLuint32 Rounds = 0; Rounds < 3; Rounds++)
   {
      COLuint8 Char = 0;
      do 
      {
         TestString.write(&Char,1);
      } while (++Char);
      //do a bit of shifting
      for (COLuint32 ShiftIndex = 0; ShiftIndex < Rounds; ShiftIndex++)
      {
         TestString.write(" ",1);
      }
   }
   return TestString;
}

static void testBinaryDisplay()
{
   COLstring TestString = makeTestString();
   COLstring Out;
   COLostream OutStream(Out);
   DISbinaryDisplay::displayData(OutStream, TestString.c_str(), TestString.size(),-1,-1,false);
   //   COLcout << Out;
   COLstring Ref;
   FILreadFile("testDISreference.txt", &Ref);
   //remove /r /n for comparison
   Ref.stripAll('\r');
   Ref.stripAll('\n');
   Out.stripAll('\r');
   Out.stripAll('\n');

   TST_ASSERT_EQUALS(Ref,Out);
}

static void testBinaryDisplayHtml()
{
   COLstring TestString = makeTestString();
   COLstring Out;
   COLostream OutStream(Out);
   DISbinaryDisplay::displayData(OutStream, TestString.c_str(), TestString.size());

//   COLcout << Out;
   COLstring Ref;
   FILreadFile("testDISreferenceHtml.txt", &Ref);

   //remove /r /n for comparison
   Ref.stripAll('\r');
   Ref.stripAll('\n');
   Out.stripAll('\r');
   Out.stripAll('\n');

   TST_ASSERT_EQUALS(Ref,Out);
}

TSTtestable* DISbinaryDisplayTest()
{
   TST_START_COLLECTION("DISbinaryDisplayTest")
      TST_ADD_TEST_FUNCTION(testBinaryDisplay);   
      TST_ADD_TEST_FUNCTION(testBinaryDisplayHtml);
   TST_END_COLLECTION
}

