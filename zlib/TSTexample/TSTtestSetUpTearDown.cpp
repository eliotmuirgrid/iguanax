//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSetUpTearDown
//
// Description: Tests that SetUp and TearDown functions work correctly
//
// Author: Joanna Karlic
// Date:   May 2008
//
//---------------------------------------------------------------------------

#include <TSTexample/TSTexamplePrecompiled.h>
#pragma hdrstop

#include <TSTexample/TSTtestSetUpTearDown.h>

void TSTtestSetUpFunctionality::addToString1()
{
   TST_ASSERT(MyString.size() == 5); // MyString has been set to "Setup" by SetUp function
   MyString.append("X", 1);

}

void TSTtestSetUpFunctionality::setupStringUnchanged()
{
   // the size of the string should be:
   // "Setup" (5) + "X" (1) + "Setup" (5) = 11
   TST_ASSERT(MyString.size() == 11);
}

void TSTtestSetUpFunctionality::inherit1()
{
   MyString = "new";
   TST_ASSERT(MyString.size() == 3);
}

void TSTtestSetUpFunctionality::inherit2()
{
   //MyString should not have been changed by the SetUp method
   TST_ASSERT(MyString.size() == 3);
   TST_ASSERT(MyString == "new");
}

void TSTtestSetUpFunctionality::setUpForSetUpTests()
{
   MyString.append("Setup", 5);

}

void TSTtestTearDownFunctionality::addToString2()
{
   MyString.append("abc", 3);

   TST_ASSERT(MyString.size() == 3);
}

void TSTtestTearDownFunctionality::afterTearDown()
{
   TST_ASSERT(MyString.size() == 0);
}

void TSTtestTearDownFunctionality::tearDownForTearDownTests()
{
   MyString.clear();
   TST_ASSERT(MyString.size()==0)
}

TSTtestable* TSTtestSetUpFunctionality::setUpTests()
{
   TST_START_FIXTURE(TSTtestSetUpFunctionality)
      TST_ADD_SET_UP_METHOD(setUpForSetUpTests)
      TST_ADD_TEST_METHOD(addToString1)
      TST_ADD_TEST_METHOD(setupStringUnchanged)
      TST_ADD_FIXTURE_INHERIT("ParentTest")
         TST_ADD_TEST_METHOD(inherit1);
         TST_ADD_TEST_METHOD(inherit2);
      TST_END_ADD_COLLECTION  
   TST_END_FIXTURE
}

TSTtestable* TSTtestTearDownFunctionality::tearDownTests()
{
   TST_START_FIXTURE(TSTtestTearDownFunctionality)
      TST_ADD_TEST_METHOD(addToString2);
      TST_ADD_TEST_METHOD(afterTearDown);
      TST_ADD_TEAR_DOWN_METHOD(tearDownForTearDownTests);
   TST_END_FIXTURE
}
