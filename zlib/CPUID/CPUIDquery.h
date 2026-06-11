#ifndef __CPUID_QUERY_H__
#define __CPUID_QUERY_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CPUIDquery
//
// Description:
//
// Wrapper around the x86/x64 CPUID instruction.
// 
// Among other interesting details, this can be used to retrieve the processor's
// "brand string", e.g.
// 
//    AMD Phenom(tm) II X4 965 Processor
//    Intel(R) Core(TM)2 Duo CPU    P7350  @ 2.00GHz
//
// Author: Steven Dodd
// Date:   Friday, September 10th, 2010 @ 10:15:49 AM
//
//---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

// Available only on x86/x64 systems.  Queries are no-ops elsewhere.
#if defined(_WIN32) || defined(__APPLE__) || defined(__linux__)
#define CPUID_AVAILABLE 1
#endif

void CPUIDquery(int CPUInfo[4], int InfoType);

COLstring CPUIDqueryBrandString();

#endif // end of defensive include
