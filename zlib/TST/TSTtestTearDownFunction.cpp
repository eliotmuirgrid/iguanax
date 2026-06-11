//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestTearDownFunction
//
// Description:
//
// Implementation
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:00:53 PM
//
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestTearDownFunction.h"

TSTtestTearDownFunction::TSTtestTearDownFunction(): TSTtestable("")
{
}

TSTtestTearDownFunction::TSTtestTearDownFunction
(
   const COLstring& TestName, 
   fpTearDownFunctionDef ipTearDownFunctionDef  
)
   : TSTtestable(TestName)
{
   pTearDownFunctionPointer = ipTearDownFunctionDef;
}

TSTtestTearDownFunction::~TSTtestTearDownFunction()
{
}

void TSTtestTearDownFunction::runTearDown()
{
   (*pTearDownFunctionPointer)();
}

void TSTtestTearDownFunction::runTest(const TSTflags& Flags)
{
   runTearDown();
}

TSTtestable* TSTtestTearDownFunction::clone() const
{
   return new TSTtestTearDownFunction(testName(), pTearDownFunctionPointer);
}

TSTtype& TSTtestTearDownFunction::testType() const
{
   return TSTtypeTestTearDownFunction::instance();
}
