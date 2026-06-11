#ifndef __TEST_TSMVARIANT_H__
#define __TEST_TSMVARIANT_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMvariant.h
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

class testTSMvariant
{
public:
   testTSMvariant()  {}
   virtual ~testTSMvariant() {}

   // create a pointer to a new test object
   static testTSMvariant* create(){ return new testTSMvariant; }

   void testDefaults();

   void testSetGet();
   void testAssignment();

   void testInplaceStringAsBoolean();
   void testInplaceStringAsInt();   
   void testInplaceStringAsString();   

   void testBoolean();
   void testCOLstring();   
   void testInt32();

   static TSTtestable* testCollection();

private:
   testTSMvariant(const testTSMvariant& Orig);  
   testTSMvariant& operator=(const testTSMvariant& Orig);

};

#endif // end of defensive include
