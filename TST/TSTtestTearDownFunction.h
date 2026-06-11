#ifndef __TST_TEST_TEAR_DOWN_FUNCTION_H__
#define __TST_TEST_TEAR_DOWN_FUNCTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestTearDownFunction
//
// Description:
//
// TSTtestTearDownFunction class.
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:00:53 PM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>
#include <TST/TSTtestable.h>

class TSTtestTearDownFunctionPrivate;

class TSTtestTearDownFunction : public TSTtestable
{
public:
   typedef void (*fpTearDownFunctionDef)();
   TSTtestTearDownFunction();
   TSTtestTearDownFunction(const COLstring& TestName, fpTearDownFunctionDef ipTearDownFunctionDef);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSTtestTearDownFunction();

   void runTearDown();
   
   virtual void runTest(const TSTflags& Flags);
   virtual TSTtestable* clone() const;
   virtual TSTtype& testType() const;

private:
   fpTearDownFunctionDef pTearDownFunctionPointer;

};
#endif // end of defensive include
