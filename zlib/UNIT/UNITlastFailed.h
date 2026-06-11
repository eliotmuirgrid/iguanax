#ifndef __UNIT_LAST_FAILED_H__
#define __UNIT_LAST_FAILED_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITlastFailed
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Code that is used to implement logic for saving the tests that need to
// be re-run.  Implementation only - users use UNITapp.h instead.
//---------------------------------------------------------------------------

#include "UNITapp.h"

class UNITsummary;

void UNITfilterFailedTests(COLmap<COLstring, COLauto<COLclosure0> >* pTests);
void UNITsaveFailedTests(const UNITsummary& Summary);

#endif // end of defensive include
