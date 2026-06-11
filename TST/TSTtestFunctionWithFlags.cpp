//---------------------------------------------------------------------------
// Copyright (C) 1997-2008 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TSTtestFunctionWithFlags
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday, May 29th, 2008 @ 11:02:35 AM
//
//---------------------------------------------------------------------------
#include "TSTprecomp.h"
#pragma hdrstop

#include "TSTtestFunctionWithFlags.h"

TSTtestFunctionWithFlags::TSTtestFunctionWithFlags
(
   const COLstring& TestName, 
   fpTestFunctionDef ipFunctionPointer  
)
   : TSTtestable(TestName)
{
   pFunctionPointer = ipFunctionPointer;
}

TSTtestFunctionWithFlags::~TSTtestFunctionWithFlags()
{
}

TSTtype& TSTtestFunctionWithFlags::testType() const
{
   return TSTtypeTestFunctionWithFlags::instance();
}

void TSTtestFunctionWithFlags::runTest(const TSTflags& Flags)
{
   (*pFunctionPointer)(Flags);
}

TSTtestable* TSTtestFunctionWithFlags::clone() const
{
   return new TSTtestFunctionWithFlags(testName(),pFunctionPointer);
}
