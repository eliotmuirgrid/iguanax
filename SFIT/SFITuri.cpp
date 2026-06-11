//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIuri
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Friday, August 4th, 2006 @ 09:36:35 AM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITuri.h"
#include "SFITstream.h"

#include <SFI/SFIuriUnescapeFilter.h>
#include <SFI/SFIuriEscapeFilter.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
#include <COL/COLsinkString.h>
COL_LOG_MODULE;

static void testUnescape()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIuriUnescapeFilter TestFilter(&Sink);

   // Test it with everything being an escape character
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "%54%68%69%73%20%69%73%20%61%20%74%65%73%74%2e";
   }
   TST_ASSERT_EQUALS("This is a test.", Data);

   // Test it with mixed escape characters
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This+%69%73 a%20%74%65st.";      
   }   
   TST_ASSERT_EQUALS("This is a test.", Data);

   // Test the buffering logic
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This%" << '2' << "0is%" << "20a%2" << "0test.";   
   }   
   TST_ASSERT_EQUALS("This is a test.", Data);

   //Test '%' before '+' unescaping condition described in #5333
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This%20is%20a+test.";   
   }
   TST_ASSERT_EQUALS("This is a test.", Data);

   //Test '+' before '%' unescaping condition
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This+is%20a%20test.";   
   }
   TST_ASSERT_EQUALS("This is a test.", Data);

   //Test '%' only unescaping condition
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This%20is%20a%20test.";   
   }
   TST_ASSERT_EQUALS("This is a test.", Data);

   //Test '+' only unescaping condition
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This+is+a+test.";   
   }
   TST_ASSERT_EQUALS("This is a test.", Data);

}

static void testEscape()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIuriEscapeFilter TestFilter(&Sink);

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This is a test.";
   }
   TST_ASSERT(Data == "This+is+a+test.");

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "~This is also a test,\twith some odd punctuation.--\n";
   }
   TST_ASSERT(Data == "~This+is+also+a+test%2C%09with+some+odd+punctuation.--%0A");
}

TSTtestable* SFITuri()
{
   TST_START_COLLECTION("SFITuri")
      TST_ADD_TEST_FUNCTION(testUnescape);
      TST_ADD_TEST_FUNCTION(testEscape);
   TST_END_FIXTURE
};
