//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSFIxml
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

#include "SFITxml.h"
#include "SFITstream.h"

#include <SFI/SFIxmlAttributeDataEncodeFilter.h>
#include <SFI/SFIxmlDataEncodeFilter.h>
#include <SFI/SFIxmlTagEncodeFilter.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
#include <COL/COLsinkString.h>
COL_LOG_MODULE;

static void testAttributeData()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIxmlAttributeDataEncodeFilter TestFilter(&Sink);
   
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "1<2 && 4>3 \"quote test\" 'apostrophe test' tab\ttest newline\ntest carriage\rreturn\rtest";
   }
   TST_ASSERT(Data == "1&lt;2 &amp;&amp; 4>3 &quot;quote test&quot; &apos;apostrophe test&apos; tab&#x9;test newline&#xA;test carriage&#xD;return&#xD;test");
}

static void testData()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIxmlDataEncodeFilter TestFilter(&Sink);

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "1<2 && 4>3 \"quote test\" 'apostrophe test'";
   }
   TST_ASSERT(Data == "1&lt;2 &amp;&amp; 4&gt;3 &quot;quote test&quot; &apos;apostrophe test&apos;");
}

static void testTag()
{
   COLstring Data;
   COLsinkString Sink(Data);
   SFIxmlTagEncodeFilter TestFilter(&Sink);
   
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "Tag1";
   }   
   TST_ASSERT(Data == "Tag1");

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "3Tag1";
   }   
   TST_ASSERT(Data == "_Tag1");

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "!^@^@$^Tag";
   }
   TST_ASSERT(Data == "Tag");
   
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "!^@^@$^5Tag";
   }
   TST_ASSERT(Data == "_Tag");

   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << ":Tag1";
   }   
   TST_ASSERT(Data == ":Tag1");
   
   Data.clear();
   TestFilter.resetFilter();
   {
      SFITstream FilterStream(TestFilter);
      FilterStream.stream() << "This-is.a_tag:000";
   }   
   TST_ASSERT(Data == "This-is.a_tag:000");
}


TSTtestable* SFITxml()
{
   TST_START_COLLECTION("SFITxml")
      TST_ADD_TEST_FUNCTION(testAttributeData);
      TST_ADD_TEST_FUNCTION(testData);
      TST_ADD_TEST_FUNCTION(testTag);
   TST_END_FIXTURE
};
