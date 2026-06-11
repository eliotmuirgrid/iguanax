//---------------------------------------------------------------------------
// Copyright (C) 1997-2014 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: COLTvarUtils
//
// Description:
//
// Implementation
//
// Author: Akshay Ganeshen
// Date:   Mon 10 Nov 2014 09:03:56 EST
//---------------------------------------------------------------------------
#include "COLTprecomp.h"
#pragma hdrstop

#include "COLTvarUtils.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvarUtils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void testSetDefaultValue() {
   COL_FUNCTION(testSetDefaultValue);
   COLvar Map = COLvar().setMapType();

   COLstring DefaultValueKey = "foo";
   COLstring DefaultValue = "bar";
   COLstring DefaultValueSecond = "baz";

   COLsetDefault(&Map, DefaultValueKey, DefaultValue);
   TST_ASSERT_MESSAGE(Map.exists(DefaultValueKey), "Setting default value should have created key.");
   TST_ASSERT_EQUALS_MESSAGE(DefaultValue, Map[DefaultValueKey], "The Map's value did not match the default value.");

   COLsetDefault(&Map, DefaultValueKey, DefaultValueSecond);
   TST_ASSERT_MESSAGE(Map[DefaultValueKey].str() != DefaultValueSecond, "Setting default value should not override previous value.");

   Map.erase(DefaultValueKey);

   COLsetDefault(&Map, DefaultValueKey, DefaultValueSecond);
   TST_ASSERT_EQUALS_MESSAGE(DefaultValueSecond, Map[DefaultValueKey], "The Map's value did not match the second default value after erasing the key.");

   Map.reset();

   COLsetDefault(&Map, DefaultValueKey, DefaultValue);
   TST_ASSERT_MESSAGE(Map.isMap(), "Setting a default value on a non-Map type should convert it to a Map.");
   TST_ASSERT_MESSAGE(Map.exists(DefaultValueKey), "After resetting, setting default value should have converted created key.");
   TST_ASSERT_EQUALS_MESSAGE(DefaultValue, Map[DefaultValueKey], "After resetting, the Map's value did not match the default value.");
}

static void testAssertMapValueTypes() {
   COL_FUNCTION(testAssertMapValueTypes);
   COLvar Map = COLvar().setMapType();

   COLstring StringKey = "string";
   COLstring IntKey = "int";
   COLstring BoolKey = "bool";
   COLstring MissingKey = "fred";
   COLstring ArrayOfStringsKey = "arrayofstrings";
   COLstring MixedArrayKey = "mixedarray";
   COLstring MapKey = "anothermap";
   COLstring EmptyArrayKey = "emptyarray";

   COLvar ArrayOfStrings = COLvar().setArrayType();
   ArrayOfStrings.push_back("One fish");
   ArrayOfStrings.push_back("Two fish");

   COLvar MixedArray = COLvar().setArrayType();
   MixedArray.push_back("Testing testing");
   MixedArray.push_back(1);
   MixedArray.push_back(2);
   MixedArray.push_back(3);

   COLvar EmptyArray = COLvar().setArrayType();

   COLvar AnotherMap = COLvar().setMapType();
   AnotherMap[StringKey] = "string";

   /* Set up values of various types in the map */
   Map[StringKey] = "string";
   Map[IntKey] = 2;
   Map[BoolKey] = true;
   Map[ArrayOfStringsKey] = ArrayOfStrings;
   Map[MixedArrayKey] = MixedArray;
   Map[EmptyArrayKey] = EmptyArray;
   Map[MapKey] = AnotherMap;

   // Generally there are only two cases for each assert macro - it fails or it doesn't
   // So test the combinations for required and optional
   TST_ASSERT_MESSAGE(!Map.exists(MissingKey), "The key '" << MissingKey << "' was never assigned a value in the map. It should not exist in the map.");

   TST_ASSERT_NO_EXCEPTION(COLassertParamIsString(Map, StringKey));   // it's there, and it's a string
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsString(Map, MissingKey), COLerror("Map " + MissingKey + " must be provided as a string.", COLerror::PreCondition), "Asserting required param should fail when key is not in the map.");
   
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsInt(Map, IntKey));
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsInt(Map, MissingKey), COLerror("Map " + MissingKey + " must be provided as an integer.", COLerror::PreCondition), "Asserting required param should fail when key is not in the map.");
   
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsBool(Map, BoolKey));
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsBool(Map, MissingKey), COLerror("Map " + MissingKey + " must be provided as a bool.", COLerror::PreCondition), "Asserting required param should fail when key is not in the map.");
   
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsStringIfExists(Map, StringKey)); // it's there, so it should be a string
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsStringIfExists(Map, MissingKey));   // it's not there, that's fine
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsStringIfExists(Map, IntKey), COLerror("Map " + IntKey + " must be a string.", COLerror::PreCondition), "Asserting optional param of a specific type should fail when it is set as another type.");

   TST_ASSERT_NO_EXCEPTION(COLassertParamIsIntIfExists(Map, IntKey)); // it's there, so it should be an int
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsIntIfExists(Map, MissingKey));   // it's not there, that's fine
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsIntIfExists(Map, StringKey), COLerror("Map " + StringKey + " must be an integer.", COLerror::PreCondition), "Asserting optional param of a specific type should fail when it is set as another type.");

   TST_ASSERT_NO_EXCEPTION(COLassertParamIsArrayOrStringIfExists(Map, StringKey));
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsArrayOrStringIfExists(Map, ArrayOfStringsKey));
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsArrayOrStringIfExists(Map, EmptyArrayKey));   // even empty arrays should be counted as strings (as they convert to "")
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsArrayOrStringIfExists(Map, MixedArrayKey), COLerror("Map " + MixedArrayKey + " must be an array or a string.", COLerror::PreCondition), "Asserting that mixed array is an array of strings should fail.");

   TST_ASSERT_NO_EXCEPTION(COLassertParamIsBoolIfExists(Map, BoolKey)); // it's there, so it should be an int
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsBoolIfExists(Map, MissingKey));   // it's not there, that's fine
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsBoolIfExists(Map, StringKey), COLerror("Map " + StringKey + " must be a bool.", COLerror::PreCondition), "Asserting optional param of a specific type should fail when it is set as another type.");

   TST_ASSERT_NO_EXCEPTION(COLassertParamIsTableIfExists(Map, MapKey)); // it's there, so it should be an int
   TST_ASSERT_NO_EXCEPTION(COLassertParamIsTableIfExists(Map, MissingKey));   // it's not there, that's fine
   TST_ASSERT_EXCEPTION_MESSAGE(COLassertParamIsTableIfExists(Map, StringKey), COLerror("Map " + StringKey + " must be a table.", COLerror::PreCondition), "Asserting optional param of a specific type should fail when it is set as another type.");
}

static void testCastValuesToInt() {
   COL_FUNCTION(testCastValuesToInt);

   // These conversions should pass
   const int CastableIntCases = 8;
   COLvar IntsAsVar[CastableIntCases];
   int    IntsCasted[CastableIntCases];
   IntsAsVar[0]  = 1111;
   IntsCasted[0] = 1111;
   IntsAsVar[1]  = 10.0;   // hopefully there are no rounding issues
   IntsCasted[1] = 10;
   IntsAsVar[2]  = "1234";
   IntsCasted[2] =  1234;
   IntsAsVar[3]  = "-4321";
   IntsCasted[3] =  -4321;
   IntsAsVar[4]  = "0xFF";
   IntsCasted[4] =  0xFF;
   IntsAsVar[5]  = "0744";
   IntsCasted[5] =  0744;
   IntsAsVar[6]  = "-0xAA";
   IntsCasted[6] =  -0xAA;
   IntsAsVar[7]  = "0";
   IntsCasted[7] =  0;

   for (int i = 0; i < CastableIntCases; i++) {
      long long Casted;
      TST_ASSERT_MESSAGE(COLvarCast(IntsAsVar[i], &Casted), "Failed to cast value '" << IntsAsVar[i] <<
                         "' to int (i = " << i << ")");
      TST_ASSERT_EQUALS(IntsCasted[i], Casted);
   }

   // These conversions should fail
   const int UncastableIntCases = 9;
   COLvar UncastableToInt[UncastableIntCases];
   UncastableToInt[0].reset();
   UncastableToInt[1].setArrayType();
   UncastableToInt[2].setMapType();
   UncastableToInt[3] = "";
   UncastableToInt[4] = "hello";
   UncastableToInt[5] = "0x";
   UncastableToInt[6] = "-";
   UncastableToInt[7] = "01234ABCD";   // octal with hex characters
   UncastableToInt[8] = 12.3;
   for (int i = 0; i < UncastableIntCases; i++) {
      long long Casted; // unused
      TST_ASSERT_MESSAGE(!COLvarCast(UncastableToInt[i], &Casted), "It should not be possible to cast '" <<
                         UncastableToInt[i] << "' to int (i = " << i << ")");
   }
}

static void testCastValuesToBool() {
   COL_FUNCTION(testCastValuesToBool);

   // This test isn't comprehensive - just tests the common string names of booleans

   // These conversions should pass
   const int CastableBoolCases = 6;
   COLvar BoolsAsVar[CastableBoolCases];
   int    BoolsCasted[CastableBoolCases];
   BoolsAsVar[0]  = true;
   BoolsCasted[0] = true;
   BoolsAsVar[1]  = false;
   BoolsCasted[1] = false;
   BoolsAsVar[2]  = "true";
   BoolsCasted[2] =  true;
   BoolsAsVar[3]  = "false";
   BoolsCasted[3] =  false;
   BoolsAsVar[4]  = "t";
   BoolsCasted[4] =  true;
   BoolsAsVar[5]  = "f";
   BoolsCasted[5] =  false;

   for (int i = 0; i < CastableBoolCases; i++) {
      bool Casted;
      TST_ASSERT_MESSAGE(COLvarCast(BoolsAsVar[i], &Casted), "Failed to cast value '" << BoolsAsVar[i] <<
                         "' to bool (i = " << i << ")");
      TST_ASSERT_EQUALS(BoolsCasted[i], Casted);
   }

   // These conversions should fail
   const int UncastableBoolCases = 6;
   COLvar UncastableToBool[UncastableBoolCases];
   UncastableToBool[0].reset();
   UncastableToBool[1].setArrayType();
   UncastableToBool[2].setMapType();
   UncastableToBool[3] = 1;
   UncastableToBool[4] = 20.0;
   UncastableToBool[5] = "tralse";
   for (int i = 0; i < UncastableBoolCases; i++) {
      bool Casted; // unused
      TST_ASSERT_MESSAGE(!COLvarCast(UncastableToBool[i], &Casted), "It should not be possible to cast '" <<
                         UncastableToBool[i] << "' to bool (i = " << i << ")");
   }
}

static void testCastValuesToString() {
   COL_FUNCTION(testCastValuesToString);

   // All COLvars can be converted to strings via ::json, but this test is basically asserting that
   // we only cast scalars to strings

   // These conversions should pass
   const int CastableStringCases = 5;
   COLvar    StringsAsVar[CastableStringCases];
   // no need to manually fill in the "expected" string - it just calls toString
   StringsAsVar[0].reset();
   StringsAsVar[1] = 1;
   StringsAsVar[2] = 2.0;
   StringsAsVar[3] = "3";
   StringsAsVar[4] = true;

   for (int i = 0; i < CastableStringCases; i++) {
      COLstring Casted; // unused - don't care what it is
      TST_ASSERT_MESSAGE(COLvarCast(StringsAsVar[i], &Casted), "Failed to cast value '" << StringsAsVar[i] <<
                         "' to string (i = " << i << ")");
   }

   // These conversions should fail
   COLstring Casted; // unused
   COLvar MapType;
   MapType.setMapType();
   TST_ASSERT_MESSAGE(!COLvarCast(MapType,   &Casted), "It should not be possible to cast maps to strings");
   COLvar ArrayType;
   ArrayType.setArrayType();
   TST_ASSERT_MESSAGE(!COLvarCast(ArrayType, &Casted), "It should not be possible to cast arrays to strings");
}

TSTtestable* COLTvarUtils() {
   TST_START_COLLECTION("COLTvarUtils")
      TST_ADD_TEST_FUNCTION(testSetDefaultValue);
      TST_ADD_TEST_FUNCTION(testAssertMapValueTypes);
      TST_ADD_TEST_FUNCTION(testCastValuesToInt);
      TST_ADD_TEST_FUNCTION(testCastValuesToBool);
      TST_ADD_TEST_FUNCTION(testCastValuesToString);
   TST_END_FIXTURE
}
