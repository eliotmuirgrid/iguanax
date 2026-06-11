#ifndef __UNIT_RUN_H__
#define __UNIT_RUN_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: UNITrun
//
// Author: Eliot Muir
//
// Date: Wed 17 Mar 2021 13:18:26 EDT
//
// Description:
//
// Functions related to running the tests.  Not required for users of the
// the library - use UNITapp.h instead.
//---------------------------------------------------------------------------

#include "UNITapp.h"

class SCKloop;

class UNITsummary{
public:
   UNITsummary(int Count) : Hung(false), TimeoutTimerId(-1), TestCount(Count), CountOfFail(Count) {}
   ~UNITsummary();
   COLmap<COLstring, COLstring> m_FailedBuilds;


   int TestCount;
   int CountOfFail;
   bool Hung;
   int TimeoutTimerId;
};

int UNITrunTestCollection(const COLmap<COLstring, COLauto<COLclosure0> >& Tests);
int UNITrunSingleThreadedTestCollection(const COLmap<COLstring, COLauto<COLclosure0> >& Tests);


#endif // end of defensive include
