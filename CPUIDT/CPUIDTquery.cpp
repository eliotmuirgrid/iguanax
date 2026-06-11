//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CPUIDTquery
//
// Description:
//
// Implementation
//
// Author: Steven Dodd
// Date:   Friday, September 10th, 2010 @ 10:37:46 AM
//
//---------------------------------------------------------------------------
#include "CPUIDTprecomp.h"
#pragma hdrstop

#include "CPUIDTquery.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <CPUID/CPUIDquery.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void testBrandString()
{
#ifdef CPUID_AVAILABLE
   COLstring BrandString = CPUIDqueryBrandString();

   COLcout << "BrandString=" << BrandString << newline;
   // VirtualApple is for cross compiling for Intel Mac from ARM Mac
   TST_ASSERT(BrandString.find("Intel") != npos || BrandString.find("AMD") != npos || BrandString.find("VirtualApple") != npos);
#endif
}

TSTtestable* CPUIDTquery()
{
   TST_START_COLLECTION("CPUIDTquery")
      #if !(defined(__arm__) || defined(__aarch64__))
      TST_ADD_TEST_FUNCTION(testBrandString);
      #endif
   TST_END_FIXTURE
};
