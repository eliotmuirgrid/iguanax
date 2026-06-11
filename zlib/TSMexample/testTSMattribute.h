#ifndef __TEST_TSMATTRIBUTE_H__
#define __TEST_TSMATTRIBUTE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMattribute.h
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

class testTSMattribute
{
public:
   testTSMattribute()  {}
   virtual ~testTSMattribute() {}

   // create a pointer to a new test object
   static testTSMattribute* create(){ return new testTSMattribute; }

   void testSetGet();
   void testConstness();
   void testAssignment();

   static TSTtestable* testCollection();

private:
   testTSMattribute(const testTSMattribute& Orig);  
   testTSMattribute& operator=(const testTSMattribute& Orig);

};

#endif // end of defensive include
