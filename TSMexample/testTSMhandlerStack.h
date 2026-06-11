#ifndef __TEST_TSMHANDLER_STACK_H__
#define __TEST_TSMHANDLER_STACK_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMhandlerStack.h
//
// Description:
//
// A set of tests of functions in the TSM library.
//
// Author: Matt Lawrence
// Date:   Tuesday, February 27th, 2007 @ 08:50:35 AM
//
//---------------------------------------------------------------------------

class TSTtestable;

class testTSMhandlerStack
{
public:
   testTSMhandlerStack()  {}
   virtual ~testTSMhandlerStack() {}

   // create a pointer to a new test object
   static testTSMhandlerStack* create(){ return new testTSMhandlerStack; }

   void testDefaults();
   void testInit();
   void testSetGet();
   void testPushPop();   

   static TSTtestable* testCollection();

private:
   testTSMhandlerStack(const testTSMhandlerStack& Orig);  
   testTSMhandlerStack& operator=(const testTSMhandlerStack& Orig);

};

#endif // end of defensive include
