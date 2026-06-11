#ifndef __TST_TEST_FUNCTION_H__
#define __TST_TEST_FUNCTION_H__
//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFunction
//
// Description: A testable class that uses a function pointer
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------

#include <TST/TSTtestable.h>
class TSTflags;

class TSTtestFunction : public TSTtestable
{
public:
   typedef void (*fpTestFunctionDef)();

   TSTtestFunction(const COLstring& TestName, 
                   fpTestFunctionDef ipFunctionPointer);
   virtual ~TSTtestFunction();

   TSTtype& testType() const;

   virtual void runTest(const TSTflags& Flags);

   virtual TSTtestable* clone() const;

private:
   fpTestFunctionDef pFunctionPointer; 
};

#endif // end of defensive include


