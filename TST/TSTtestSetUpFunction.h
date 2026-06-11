#ifndef __TST_TEST_SET_UP_FUNCTION_H__
#define __TST_TEST_SET_UP_FUNCTION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSetUpFunction
//
// Description:
//
// TSTtestSetUpFunction class.
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:01:02 PM
//
//---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>
#include <TST/TSTtestable.h>

class TSTtestSetUpFunctionPrivate;

class TSTtestSetUpFunction : public TSTtestable
{
public:
   typedef void (*fpSetUpFunctionDef)();
   TSTtestSetUpFunction();
   TSTtestSetUpFunction(const COLstring& TestName, fpSetUpFunctionDef ipSetUpFunctionDef);
   // This is non virtual because we are assuming no classes will inherit from this class
   ~TSTtestSetUpFunction();

   void runSetUp();

   virtual void runTest(const TSTflags& Flags);
   virtual TSTtestable* clone() const;
   virtual TSTtype& testType() const;
   

private:
   fpSetUpFunctionDef pSetUpFunctionPointer;

};

#endif // end of defensive include
