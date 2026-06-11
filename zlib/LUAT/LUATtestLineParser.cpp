//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LUATtestLineParser
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday, June 30th, 2010 @ 10:26:22 AM
//
//---------------------------------------------------------------------------
#include "LUATprecomp.h"
#pragma hdrstop

#include "LUATtestLineParser.h"

#include <LUA/LUAlineParser.h>

#include <TST/TSThelpers.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>

#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testParser()
{
   COLarray<int> LineLengthVector;
   COLstring Code;
   FILreadFile("../LUAT/modules/hello.lua", &Code);
   LUAparseLineLengths(Code, &LineLengthVector);
}

TSTtestable* LUATtestLineParser()
{
   TST_START_COLLECTION("LUATtestLineParser")
      TST_ADD_TEST_FUNCTION(testParser);
   TST_END_FIXTURE
};

