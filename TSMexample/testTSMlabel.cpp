//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMlabel
//
// Description:
//
// Implementation
//
// Author: Matt Lawrence
// Date:   Tuesday, February 27th, 2007 @ 08:50:35 AM
//
//---------------------------------------------------------------------------
#include "TSMexamplePrecomp.h"
#pragma hdrstop

#include "testTSMlabel.h"

#include <TSM/TSMlabel.h>

void testTSMlabel::testDefaults()
{
   TSMlabel Label;
   TST_ASSERT_EQUALS(0, Label.id());
   TST_ASSERT_EQUALS(0, Label.size());
   TST_ASSERT(NULL != Label.label());
}

void testTSMlabel::testConstructors()
{
   TSMlabel Label0(1, "foo", 3);
   TSMlabel Label1(2, "bar");
   TSMlabel Label2(3, "foobar", 3);
   TSMlabel Label3(Label0);
   
   TST_ASSERT_EQUALS(1, (COLuint32)Label0.id());
   TST_ASSERT(memcmp("foo", Label0.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label0.size());

   TST_ASSERT_EQUALS(2, (COLuint32)Label1.id());
   TST_ASSERT(memcmp("bar", Label1.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label1.size());
      
   TST_ASSERT_EQUALS(3, (COLuint32)Label2.id());
   TST_ASSERT(memcmp("foo", Label2.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label2.size());
   
   TST_ASSERT_EQUALS(1, (COLuint32)Label3.id());   
   TST_ASSERT(memcmp("foo", Label3.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label3.size());
}

void testTSMlabel::testAssignment()
{
   TSMlabel Label0(255, "foo");
   TSMlabel Label1;
   Label1 = Label0;
   TST_ASSERT_EQUALS(Label0.id(), (COLuint32)Label1.id());
   TST_ASSERT(memcmp(Label0.label(), Label1.label(), Label0.size()) == 0);
   TST_ASSERT_EQUALS(Label0.size(), Label0.size());
}

void testTSMlabel::testSetGet()
{
   TSMlabel Label;

   Label.setId(1);
   TST_ASSERT_EQUALS(1, (COLuint32)Label.id());

   Label.setString("foo");
   TST_ASSERT(memcmp("foo", Label.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label.size());
   
   Label.setStringWithSize("foobar", 3);   
   TST_ASSERT(memcmp("foo", Label.label(), 3) == 0);
   TST_ASSERT_EQUALS(3, Label.size());   
}

void testTSMlabel::testEquality()
{
   TSMlabel Label0(0, "foo");
   TSMlabel Label1(0, "bar");
   TSMlabel Label2(1, "foo");
   TSMlabel Label3(2, "foobar");

   TST_ASSERT_EQUALS(true, Label0 == Label0);
   TST_ASSERT_EQUALS(false, Label0 != Label0);
   
   TST_ASSERT_EQUALS(true, Label0 == Label1);
   TST_ASSERT_EQUALS(false, Label0 != Label1);

   TST_ASSERT_EQUALS(true, Label0 == Label2);
   TST_ASSERT_EQUALS(false, Label0 != Label2);   
   
   TST_ASSERT_EQUALS(false, Label0 == Label3);
   TST_ASSERT_EQUALS(true, Label0 != Label3);

   TST_ASSERT_EQUALS(false, Label1 == Label2);
   TST_ASSERT_EQUALS(true, Label1 != Label2);

   TST_ASSERT_EQUALS(true, Label0 == "foo");
   TST_ASSERT_EQUALS(false, Label0 != "foo");

   TST_ASSERT_EQUALS(false, Label0 == "foobar");
   TST_ASSERT_EQUALS(true, Label0 != "foobar");

   TST_ASSERT_EQUALS(false, Label0 == "bar");
   TST_ASSERT_EQUALS(true, Label0 != "bar");
}

void testTSMlabel::testStream()
{
   TSMlabel Label(0, "test");
   COLstring String;
   COLostream Stream(String);

   Stream << Label;

   TST_ASSERT_EQUALS("test", String);
}

TSTtestable* testTSMlabel::testCollection()
{
   TST_START_FIXTURE(testTSMlabel)
      TST_ADD_TEST_METHOD(testDefaults);
      
      TST_ADD_TEST_METHOD(testConstructors);
      TST_ADD_TEST_METHOD(testAssignment);
      TST_ADD_TEST_METHOD(testSetGet);
      TST_ADD_TEST_METHOD(testEquality);

      TST_ADD_TEST_METHOD(testStream);
   TST_END_FIXTURE
}
;
