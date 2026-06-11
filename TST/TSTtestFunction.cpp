//---------------------------------------------------------------------------
//
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFunction
//
// Description:
//
// TSTtestFunction Represents a TSTtestable object that uses a plain function pointer
//
// Author: Nasron Cheong
// Date:   Feb 2004 
//
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestFunction.h"

TSTtestFunction::TSTtestFunction
(
   const COLstring& TestName, 
   fpTestFunctionDef ipFunctionPointer  
)
   : TSTtestable(TestName)
{
   pFunctionPointer = ipFunctionPointer;
}

TSTtestFunction::~TSTtestFunction()
{
}

TSTtype& TSTtestFunction::testType() const
{
   return TSTtypeTestFunction::instance();
}

void TSTtestFunction::runTest(const TSTflags& Flags)
{
   (*pFunctionPointer)();
}

TSTtestable* TSTtestFunction::clone() const
{
   return new TSTtestFunction(testName(),pFunctionPointer);
}
