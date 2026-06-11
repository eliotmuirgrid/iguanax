//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMvariant
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

#include "testTSMvariant.h"

#include <TSM/TSMvariant.h>

void testTSMvariant::testDefaults()
{
   TSMvariant Variant;
   
   TST_ASSERT_EQUALS(TSM_INPLACE_STRING, Variant.valueType());
   TST_ASSERT_EQUALS(0, Variant.asInt32());
   TST_ASSERT_EQUALS(false,  Variant.asBoolean());
   TST_ASSERT_EQUALS("", Variant.operator COLstring());
}

void testTSMvariant::testSetGet()
{
   TSMvariant Variant;

   Variant.setValueType(TSM_BOOLEAN);
   TST_ASSERT_EQUALS(TSM_BOOLEAN, Variant.valueType());
}

void testTSMvariant::testAssignment()
{
   TSMvariant Variant;

   Variant.setBoolean(true);   
   {   
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_BOOLEAN, VariantCopy.valueType());
      TST_ASSERT_EQUALS(true, VariantCopy.asBoolean());
      TST_ASSERT_EQUALS(TSM_BOOLEAN, VariantEquals.valueType());
      TST_ASSERT_EQUALS(true, VariantEquals.asBoolean());
   }

   Variant.setInt32(255);   
   {   
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_INT32, VariantCopy.valueType());
      TST_ASSERT_EQUALS(TSM_INT32, VariantEquals.valueType());
   }

   Variant.setInt32(65536);   
   {   
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_INT32, VariantCopy.valueType());
      TST_ASSERT_EQUALS(65536, VariantCopy.asInt32());
      TST_ASSERT_EQUALS(TSM_INT32, VariantEquals.valueType());
      TST_ASSERT_EQUALS(65536, VariantEquals.asInt32());
   }

   Variant.setInt32(-42);   
   {   
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_INT32, VariantCopy.valueType());
      TST_ASSERT_EQUALS(-42, VariantCopy.asInt32());
      TST_ASSERT_EQUALS(TSM_INT32, VariantEquals.valueType());
      TST_ASSERT_EQUALS(-42, VariantEquals.asInt32());
   }


   Variant.setCOLstring("foobar");   
   {   
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_COLSTRING, VariantCopy.valueType());
      TST_ASSERT_EQUALS("foobar", VariantCopy.operator COLstring());
      TST_ASSERT_EQUALS(TSM_COLSTRING, VariantEquals.valueType());
      TST_ASSERT_EQUALS("foobar", VariantEquals.operator COLstring());
   }

   Variant.setValueType(TSM_INPLACE_STRING);
   Variant.setInplaceStringWithSize("test", 4);
   {
      TSMvariant VariantCopy(Variant);
      TSMvariant VariantEquals = Variant;
      TST_ASSERT_EQUALS(TSM_INPLACE_STRING, VariantCopy.valueType());
      TST_ASSERT_EQUALS("test", VariantCopy.operator COLstring());
      TST_ASSERT_EQUALS(TSM_INPLACE_STRING, VariantEquals.valueType());
      TST_ASSERT_EQUALS("test", VariantEquals.operator COLstring());

   }
}

void testTSMvariant::testInplaceStringAsBoolean()
{
   TSMvariant Variant;

   Variant.setInplaceString("true");
   TST_ASSERT_EQUALS(true, Variant.asBoolean());
   
   Variant.setInplaceString("truely");   
   TST_ASSERT_EQUALS(false, Variant.asBoolean());

   Variant.setInplaceString("tru");
   TST_ASSERT_EQUALS(false, Variant.asBoolean());

   Variant.setInplaceString("false");
   TST_ASSERT_EQUALS(false, Variant.asBoolean());

   Variant.setInplaceStringWithSize("truely", 4);
   TST_ASSERT_EQUALS(true, Variant.asBoolean());

   Variant.setInplaceStringWithSize("true", 3);
   TST_ASSERT_EQUALS(false, Variant.asBoolean());

   TST_ASSERT_EQUALS(0, Variant.asInt32());
   TST_ASSERT_EQUALS("tru", Variant.operator COLstring());

   Variant.setInplaceStringWithSize("true", 4);
}

void testTSMvariant::testInplaceStringAsInt() 
{
   TSMvariant Variant;

   Variant.setInplaceString("5");
   TST_ASSERT_EQUALS(5, Variant.asInt32());

   Variant.setInplaceString("65536");
   TST_ASSERT_EQUALS(65536, Variant.asInt32());

   Variant.setInplaceString("-1");
   TST_ASSERT_EQUALS(-1, Variant.asInt32());

   Variant.setInplaceStringWithSize("12345", 3);
   TST_ASSERT_EQUALS(123, Variant.asInt32());

   TST_ASSERT_EQUALS(false, Variant.asBoolean());
   TST_ASSERT_EQUALS("123", Variant.operator COLstring());
}

void testTSMvariant::testInplaceStringAsString()
{
   TSMvariant Variant;

   Variant.setInplaceString("yarr!");
   TST_ASSERT_EQUALS("yarr!", Variant.operator COLstring());

   Variant.setInplaceStringWithSize("whee!", 3);
   TST_ASSERT_EQUALS("whe", Variant.operator COLstring());  
}

void testTSMvariant::testBoolean()
{
   TSMvariant Variant;

   Variant.setBoolean(true);
   TST_ASSERT_EQUALS(TSM_BOOLEAN, Variant.valueType());
   TST_ASSERT_EQUALS(true,  Variant.asBoolean());
   
   Variant.setBoolean(false);
   TST_ASSERT_EQUALS(TSM_BOOLEAN, Variant.valueType());
   TST_ASSERT_EQUALS(false, Variant.asBoolean());
   
   TST_ASSERT_EXCEPTION(Variant.asInt32(), COLerror("Wrong type", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(Variant.operator COLstring(), COLerror("Wrong type", COLerror::PreCondition));
}

void testTSMvariant::testCOLstring()
{
   TSMvariant Variant;
   COLstring Temp = "STRING!";

   Variant.setCOLstring(Temp);
   TST_ASSERT_EQUALS(TSM_COLSTRING, Variant.valueType());
   TST_ASSERT_EQUALS("STRING!", Variant.operator COLstring());
   
   TST_ASSERT_EXCEPTION(Variant.asBoolean(), COLerror("Wrong type", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(Variant.asInt32(), COLerror("Wrong type", COLerror::PreCondition));
}


void testTSMvariant::testInt32()
{
   TSMvariant Variant;
   
   Variant.setInt32(0x7FFFFFFF);
   TST_ASSERT_EQUALS(TSM_INT32, Variant.valueType());
   TST_ASSERT_EQUALS(0x7FFFFFFF, Variant.asInt32());

   TST_ASSERT_EXCEPTION(Variant.asBoolean(), COLerror("Wrong type", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(Variant.operator COLstring(), COLerror("Wrong type", COLerror::PreCondition));

   Variant.setInt32(-0x80000000);
   TST_ASSERT_EQUALS(TSM_INT32, Variant.valueType());
   TST_ASSERT_EQUALS(-0x80000000, Variant.asInt32());

   TST_ASSERT_EXCEPTION(Variant.asBoolean(), COLerror("Wrong type", COLerror::PreCondition));
   TST_ASSERT_EXCEPTION(Variant.operator COLstring(), COLerror("Wrong type", COLerror::PreCondition));
}

TSTtestable* testTSMvariant::testCollection()
{
   TST_START_FIXTURE(testTSMvariant)
      TST_ADD_TEST_METHOD(testDefaults);
  
      TST_ADD_TEST_METHOD(testSetGet);
      TST_ADD_TEST_METHOD(testAssignment);

      TST_ADD_TEST_METHOD(testInplaceStringAsBoolean);
      TST_ADD_TEST_METHOD(testInplaceStringAsInt);   
      TST_ADD_TEST_METHOD(testInplaceStringAsString);   

      TST_ADD_TEST_METHOD(testBoolean);
      TST_ADD_TEST_METHOD(testCOLstring);
      TST_ADD_TEST_METHOD(testInt32);

   TST_END_FIXTURE
}
;
