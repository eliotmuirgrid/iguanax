//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestSetUpFunction
//
// Description:
//
// Implementation
//
// Author: Joanna Karlic
// Date:   Tuesday, May 6th, 2008 @ 12:01:02 PM
//
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestSetUpFunction.h"

TSTtestSetUpFunction::TSTtestSetUpFunction(): TSTtestable("")
{
}

TSTtestSetUpFunction::TSTtestSetUpFunction
(
   const COLstring& TestName, 
   fpSetUpFunctionDef ipSetUpFunctionDef  
)
   : TSTtestable(TestName)
{
   pSetUpFunctionPointer = ipSetUpFunctionDef;
}

TSTtestSetUpFunction::~TSTtestSetUpFunction()
{
}

void TSTtestSetUpFunction::runSetUp()
{
   (*pSetUpFunctionPointer)();
}

void TSTtestSetUpFunction::runTest(const TSTflags& Flags)
{
   runSetUp();
}

TSTtestable* TSTtestSetUpFunction::clone() const
{
   return new TSTtestSetUpFunction(testName(),pSetUpFunctionPointer);
}

TSTtype& TSTtestSetUpFunction::testType() const
{
   return TSTtypeTestSetUpFunction::instance();
}
