//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLthreadLocal.cpp
//
// Description:
//
// Implementation
//
// Author: Nicolas Lehman
// Date:   Wed 21 Apr 2021 12:19:00 EDT
//---------------------------------------------------------------------------
#include "testCOLthreadLocal.h"

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLthreadLocal.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void testCOLthreadLocal(){
   COL_FUNCTION(testCOLthreadLocal);

   COLthreadLocalLong ThreadLocalLong(true);
   long Value = ThreadLocalLong.get();
   COL_VAR(Value);
   ThreadLocalLong.set(++Value);
   long NextValue = ThreadLocalLong.get();
   COL_VAR(NextValue);
   TST_ASSERT(NextValue == Value);
}

TSTtestable* COLthreadLocalTests(){
   TST_START_COLLECTION("testCOLthreadLocal")
      TST_ADD_TEST_FUNCTION(testCOLthreadLocal);
   TST_END_FIXTURE
}
