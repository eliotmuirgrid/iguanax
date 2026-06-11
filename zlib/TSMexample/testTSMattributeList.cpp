//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMattributeList
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

#include "testTSMattributeList.h"

#include <TSM/TSMattribute.h>
#include <TSM/TSMattributeList.h>
#include <TSM/TSMlabel.h>
#include <TSM/TSMvariant.h>

void testTSMattributeList::testDefaults()
{
   TSMattributeList List;
   
   TST_ASSERT_EQUALS(0, List.countOfAttribute());
   
   // Exceptions now result in asserts.
  // TST_ASSERT_EXCEPTION_MESSAGE(List.attribute(0), COLerror("Failed precondition: AttributeIndex < CountOfAttribute", COLerror::PreCondition), "TSMattributeList::attribute() didn't throw on invalid attribute index."); 

   //const TSMattributeList& CList = List;
  // TST_ASSERT_EXCEPTION_MESSAGE(CList.attribute(0), COLerror("Failed precondition: AttributeIndex < CountOfAttribute", COLerror::PreCondition), "TSMattributeList::attribute() const didn't throw on invalid attribute index."); 
}

void testTSMattributeList::testAdd()
{
   TSMattributeList List;
   const TSMattributeList& CList = List;

   {   
      TSMattribute Att;     

      List.addAttribute(Att);

      TST_ASSERT_EQUALS(1, List.countOfAttribute());
      TST_ASSERT_EQUALS(Att.name(), List.attribute(0).name());
      TST_ASSERT_EQUALS(Att.value(), List.attribute(0).value());
      TST_ASSERT_EQUALS(Att.name(), CList.attribute(0).name());
      TST_ASSERT_EQUALS(Att.value(), CList.attribute(0).value());
   }

   {
      TSMattribute& Att = List.addAttribute();
      TSMlabel Name(123, "foo");
      TSMvariant Value;
      Value.setCOLstring("bar");

      Att.name() = Name;
      Att.value() = Value;

      TST_ASSERT_EQUALS(2, List.countOfAttribute());
      TST_ASSERT_EQUALS(Name, List.attribute(1).name());
      TST_ASSERT_EQUALS(Value, List.attribute(1).value());
      TST_ASSERT_EQUALS(Name, CList.attribute(1).name());
      TST_ASSERT_EQUALS(Value, CList.attribute(1).value());
   }   
}

void testTSMattributeList::testClear()
{
   TSMattributeList List;
   List.addAttribute();
   List.clear();

   // Exceptions now result in asserts.
   TST_ASSERT_EQUALS(0, List.countOfAttribute());
   //TST_ASSERT_EXCEPTION_MESSAGE(List.attribute(0), COLerror("Failed precondition: AttributeIndex < CountOfAttribute", COLerror::PreCondition), "TSMattributeList::attribute() didn't throw on invalid attribute index after clearing."); 

   List.addAttribute();
   TST_ASSERT_EQUALS(1, List.countOfAttribute());
   //TST_ASSERT_EXCEPTION_MESSAGE(List.attribute(1), COLerror("Failed precondition: AttributeIndex < CountOfAttribute", COLerror::PreCondition), "TSMattributeList::attribute() didn't throw on invalid attribute index after clearing."); 

   List.clear();
   List.addAttribute(TSMattribute());
   TST_ASSERT_EQUALS(1, List.countOfAttribute());
   //TST_ASSERT_EXCEPTION_MESSAGE(List.attribute(1), COLerror("Failed precondition: AttributeIndex < CountOfAttribute", COLerror::PreCondition), "TSMattributeList::attribute() didn't throw on invalid attribute index after clearing."); 
}

TSTtestable* testTSMattributeList::testCollection()
{
   TST_START_FIXTURE(testTSMattributeList)
      TST_ADD_TEST_METHOD(testDefaults);
      TST_ADD_TEST_METHOD(testAdd);
      TST_ADD_TEST_METHOD(testClear);
   TST_END_FIXTURE
}
;
