//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestClass2
//
// Description: Implements TSTtestClass2
//
// Author: Nasron Cheong
// Date:   Febuary 2004 
//
//---------------------------------------------------------------------------

#include <TSTexample/TSTexamplePrecompiled.h>
#pragma hdrstop

#include <TSTexample/TSTtestClass2.h>

//push a string back, and check the result
void TSTtestClass2::pushBackString()
{
   StringVector.push_back("A String");
   TST_ASSERT(StringVector.size() == 1)
}

//pop the string, and make sure its what we pushed back
void TSTtestClass2::popString()
{
   COLstring Popped = StringVector.back();
   StringVector.pop_back();
   TST_ASSERT( Popped == "A String")
   TST_ASSERT( StringVector.size() == 0 )
}

TSTtestable* TSTtestClass2::tests2()
{
   //we will return a fixture for these tests
   TST_START_FIXTURE(TSTtestClass2)
   //The methods will be invoked in the order declared here. Before the first
   //method is called, a new TSTtestClass2 will be constructed for it.
   TST_ADD_TEST_METHOD(pushBackString);
   TST_ADD_TEST_METHOD(popString);
      TST_ADD_FIXTURE_INHERIT("ParentTest")
         TST_ADD_TEST_METHOD(pushBackString);
         TST_ADD_TEST_METHOD(popString);
      TST_END_ADD_COLLECTION  
   //this returns the fixture we've constructed
   TST_END_FIXTURE
}
