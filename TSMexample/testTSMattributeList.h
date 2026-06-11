#ifndef __TEST_TSMATTRIBUTE_LIST_H__
#define __TEST_TSMATTRIBUTE_LIST_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testTSMattributeList.h
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

class testTSMattributeList
{
public:
   testTSMattributeList()  {}
   virtual ~testTSMattributeList() {}

   // create a pointer to a new test object
   static testTSMattributeList* create(){ return new testTSMattributeList; }

   void testDefaults();
   void testAdd();
   void testClear();

   static TSTtestable* testCollection();

private:
   testTSMattributeList(const testTSMattributeList& Orig);  
   testTSMattributeList& operator=(const testTSMattributeList& Orig);

};

#endif // end of defensive include
