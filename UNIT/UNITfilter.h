#ifndef __UNIT_FILTER_H__
#define __UNIT_FILTER_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITfilter
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Functions related to filtering the tests - implementation only.
//---------------------------------------------------------------------------
class COLstring;

#include "UNITapp.h"

void UNITfilterTests(const COLstring& Glob, COLmap<COLstring, COLauto<COLclosure0> >* pTests);

#endif // end of defensive include
