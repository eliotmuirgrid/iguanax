#ifndef __TEST_SQLUTILS_H__
#define __TEST_SQLUTILS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testSQLutils.h
//
// Description:
//
// A set of tests of functions in the SQL library.
//
// Author: Eliot Muir
// Date:   Monday, July 25th, 2005 @ 08:35:55 PM
//
//---------------------------------------------------------------------------

class TSTtestable;

class testSQLutilsPrivate;

#include <SQL/SQLbusyHandler.h>

class testSQLiteratorBusyHandler : public SQLbusyHandler
{
public:
   testSQLiteratorBusyHandler () 
   {
      reset(0);
   }
   ~testSQLiteratorBusyHandler()
   {
   }

   void reset(COLuint32 iMaxCountOfCalled)
   {
      CountOfCalled = 0;
      MaxCountOfCalled = iMaxCountOfCalled;
   }

   COLuint32 MaxCountOfCalled;
   COLuint32 CountOfCalled;

private:
   bool onSqliteBusy(COLuint32 Step, bool AutoCommit)
   {
      CountOfCalled++;
      if (Step == MaxCountOfCalled)
      {
         return false;
      }
      return true;
   }
};

class testSQLutils
{
public:
   testSQLutils()  {}
   virtual ~testSQLutils() {}

   // create a pointer to a new test object
   static testSQLutils* create(){ return new testSQLutils; }

   void test();

   static TSTtestable* testCollection();

private:
   testSQLutils(const testSQLutils& Orig);  
   testSQLutils& operator=(const testSQLutils& Orig);

};

#endif // end of defensive include
