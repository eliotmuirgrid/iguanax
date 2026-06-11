#ifndef __TEST_COLDATE_TIME_H__
#define __TEST_COLDATE_TIME_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2007 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLdateTime.h
//
// Description:
//
// A set of utilities to test COLdateTime objects.
//
// Author: Michael Truong
// Date:   Monday, February 14, 2005 at 11:07 AM EST 
//
//
//---------------------------------------------------------------------------

class COLdateTime;
class TSTtestable;

class testCOLdateTimePrivate;
class testCOLdateTime
{
public:
   testCOLdateTime() {}
   virtual ~testCOLdateTime() {}

   // create a pointer to a new test object
   static testCOLdateTime* create(){ return new testCOLdateTime; }
   
   static TSTtestable* COLdateTimeTests();

private:
   testCOLdateTime(const testCOLdateTime& DateTimeTest);  
   testCOLdateTime& operator=(const testCOLdateTime& DateTimeTest);
};

#endif   // end of defensive include
