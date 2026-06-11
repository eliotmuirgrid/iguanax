 //---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestClass1
//
// Description: Implements TSTtestClass1 
//
// Author: Nasron Cheong
// Date:   Febuary 2004 
//
//---------------------------------------------------------------------------

#include <TSTexample/TSTexamplePrecompiled.h>
#pragma hdrstop

#include <TSTexample/TSTtestClass1.h>

COLstring TestString = "";

void appendSingleCharacter()
{
   //do the work
    TestString += 'd';

   //now we test the result  
   
   TST_ASSERT( TestString.size() == 6 )
   TST_ASSERT( TestString == "hellod" )

   //If no exception is thrown, we assume that this test passed.
}

void doTestException()
{
   //same thing as above. but we fake a failure

   //do the work
   TestString += " dolly";

   //now we test the result
   try
   {
      TST_ASSERT( TestString.size() != 11 )
   }
   catch (TSTexception&)
   {
      COLcout << "Exception thrown, as expected." << newline;
      return;
   }
   //if we get to here, there was a problem
   TST_ASSERT(false)
}

void class1SetUp()
{
   TST_ASSERT( TestString.size() == 0 )
   TestString = "hello";
}

void class1TearDown()
{
   TestString = "";
}

TSTtestable* tests1()
{
   //we will return a collection for these tests
   TST_START_COLLECTION("TSTtestClass1 COLstring Tests")
   TST_ADD_SET_UP_FUNCTION(class1SetUp);
   //add the functions that will be tested on. 
      
   TST_ADD_TEST_FUNCTION(appendSingleCharacter);
   TST_ADD_TEST_FUNCTION(doTestException);

   //before returning, we can add any TSTtestable instance if we wanted to
   //eg

   // pCollection->addTest(new MyTestableClass);

   //this returns the collection we've constructed
   TST_ADD_TEAR_DOWN_FUNCTION(class1TearDown);
   TST_END_COLLECTION
}
