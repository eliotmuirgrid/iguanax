//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SFITmd5
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, January 15th, 2013 @ 03:52:34 PM
//
//---------------------------------------------------------------------------
#include "SFITprecomp.h"
#pragma hdrstop

#include "SFITmd5.h"

#include <SFI/SFImd5.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void testMd5(){
   COL_FUNCTION(testMd5);
   COLstring Data = "This is life la de da da";
   COLstring Hash = SFImd5((void*)Data.c_str(), Data.size());
   COLstring Hash2 = SFImd5(Data);

   COL_VAR2(Hash, Hash2);

   TST_ASSERT_EQUALS("CA31CEB6F6B44304AAEF750BBC862850", Hash);
   TST_ASSERT_EQUALS("CA31CEB6F6B44304AAEF750BBC862850", Hash2);
}

TSTtestable* SFITmd5()
{
   TST_START_COLLECTION("SFITmd5")
      TST_ADD_TEST_FUNCTION(testMd5);
   TST_END_FIXTURE
};

