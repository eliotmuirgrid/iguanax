#ifndef __TST_TEST_FUNCTION_WITH_FLAGS_H__
#define __TST_TEST_FUNCTION_WITH_FLAGS_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFunctionWithFlags
//
// Description:
//
// A testable class that uses a function pointer that takes a const TSTflags& argument
//
// Author: Eliot Muir
// Date:   Thursday, May 29th, 2008 @ 11:02:35 AM
//
//---------------------------------------------------------------------------

#include "TSTtestable.h"

class TSTflags;

class TSTtestFunctionWithFlags : public TSTtestable
{
public:
   typedef void (*fpTestFunctionDef)(const TSTflags& Flags);

   TSTtestFunctionWithFlags(const COLstring& TestName, 
                           fpTestFunctionDef ipFunctionPointer);
   virtual ~TSTtestFunctionWithFlags();

   TSTtype& testType() const;

   virtual void runTest(const TSTflags& Flags);

   virtual TSTtestable* clone() const;

private:
   fpTestFunctionDef pFunctionPointer; 
};

#endif // end of defensive include
