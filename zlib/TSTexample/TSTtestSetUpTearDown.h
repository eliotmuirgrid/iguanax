#ifndef __TST_TEST_SET_UP_TEAR_DOWN_H__
#define __TST_TEST_SET_UP_TEAR_DOWN_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSetUpFunctionality
//
// Description: Tests that SetUp and TearDown functions work correctly
//
// Author: Joanna Karlic
// Date:   May 2008
//
//---------------------------------------------------------------------------

#include <COL/COLarray.h>
#include <COL/COLstring.h>

class TSTtestable;

class TSTtestSetUpFunctionality
{
public:
   TSTtestSetUpFunctionality(){};
   virtual ~TSTtestSetUpFunctionality(){};

   //this static function is required by the fixture, so it 
   //knows how to create the instance for testing.
   static TSTtestSetUpFunctionality* create(){return new TSTtestSetUpFunctionality; };

   void addToString1();
   void setupStringUnchanged();

   void inherit1();
   void inherit2();

   //set up methods
   void setUpForSetUpTests();

   //creates a test collection with all our tests 
   //and functions
   static TSTtestable* setUpTests();

private:
   
   COLstring MyString;
   COLarray<COLstring> StringVector;
   
};

class TSTtestTearDownFunctionality
{
public:
   TSTtestTearDownFunctionality(){};
   virtual ~TSTtestTearDownFunctionality(){};

   //this static function is required by the fixture, so it 
   //knows how to create the instance for testing.
   static TSTtestTearDownFunctionality* create(){return new TSTtestTearDownFunctionality; };

   void addToString2();
   void afterTearDown();

   // tear down methods
   void tearDownForTearDownTests();

   //creates a test collection with all our tests 
   //and functions
   static TSTtestable* tearDownTests();

private:
   
   COLstring MyString;
   COLarray<COLstring> StringVector;
   
};

#endif   // end of defensive include
