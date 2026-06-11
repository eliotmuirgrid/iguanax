//---------------------------------------------------------------------------
// Copyright (C) 1997-2009 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: testCOLstring.cpp
//
// Description:
//
// Implementation
//
// Author:   Eliot Muir
// Date:     Fri 11/05/2004 
//
//---------------------------------------------------------------------------
#include "COLexamplePrecomp.h"
#pragma hdrstop

#include <COL/COLstring.h>
#include <COL/COLdateTime.h>
#include <TST/TSTtestCollection.h>
#include <TST/TSTtestFunction.h>
#include <TST/TSThelpers.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void testCOLstrerror()
{
   for (COLuint32 ErrorIndex = 0; ErrorIndex< 255; ErrorIndex++)
   {
      COLstring str = COLerrorString(ErrorIndex);
      COL_TRC(str); 
   }
}

TSTtestable* COLstrerrorTests()
{
   TST_START_COLLECTION("testCOLstrerror")
      TST_ADD_TEST_FUNCTION(testCOLstrerror);
   TST_END_COLLECTION
}


