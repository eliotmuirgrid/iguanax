#ifndef __TEST_TSMLABEL_H__
#define __TEST_TSMLABEL_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMlabel.h
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

class testTSMlabel
{
public:
   testTSMlabel()  {}
   virtual ~testTSMlabel() {}

   // create a pointer to a new test object
   static testTSMlabel* create(){ return new testTSMlabel; }

   void testDefaults();
   void testConstructors();
   void testAssignment();
   void testSetGet();
   void testEquality();
   void testStream();

   static TSTtestable* testCollection();

private:
   testTSMlabel(const testTSMlabel& Orig);  
   testTSMlabel& operator=(const testTSMlabel& Orig);

};

#endif // end of defensive include
