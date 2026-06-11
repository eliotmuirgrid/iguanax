//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMattribute
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

#include "testTSMattribute.h"

#include <TSM/TSMattribute.h>
#include <TSM/TSMlabel.h>
#include <TSM/TSMvariant.h>

void testTSMattribute::testSetGet()
{
   TSMattribute Att;
   TSMlabel Label(0, "Name!");
   TSMvariant Value;

   Value.setCOLstring("Value!");

   Att.name() = Label;
   Att.value() = Value;

   TST_ASSERT_EQUALS(Label, Att.name());   
   TST_ASSERT_EQUALS(Value, Att.value());
}

void testTSMattribute::testConstness()
{
   TSMattribute Att;
   TSMlabel Label(0, "Name!");
   TSMvariant Value;

   Value.setCOLstring("Value!");

   Att.name() = Label;
   Att.value() = Value;

   const TSMattribute& CAtt = Att;
   TST_ASSERT_EQUALS(Label, CAtt.name());
   TST_ASSERT_EQUALS(Value, CAtt.value());
}

void testTSMattribute::testAssignment()
{
   TSMattribute Att;
   TSMlabel Label(0, "Name!");
   TSMvariant Value;

   Value.setCOLstring("Value!");

   Att.name() = Label;
   Att.value() = Value;

   TSMattribute Att2(Att);
   TST_ASSERT_EQUALS(Label, Att2.name());
   TST_ASSERT_EQUALS(Value, Att2.value());

   TSMattribute Att3;
   Att3 = Att;
   TST_ASSERT_EQUALS(Label, Att3.name());
   TST_ASSERT_EQUALS(Value, Att3.value());
}

TSTtestable* testTSMattribute::testCollection()
{
   TST_START_FIXTURE(testTSMattribute)
      TST_ADD_TEST_METHOD(testSetGet);
      TST_ADD_TEST_METHOD(testConstness);
      TST_ADD_TEST_METHOD(testAssignment);
   TST_END_FIXTURE
}
;
