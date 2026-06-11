// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeTest
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Thursday 28 September 2023 - 01:34PM
// ---------------------------------------------------------------------------

#include <SDBA/SDBApurgeTest.h>
#include <SDB/SDBpurgeTest.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBApurgeTestDone(COLvar Result, COLaddress Address){
   COL_FUNCTION(SDBApurgeTestDone);
   COL_VAR(Result);
   COLrespondSuccess(Address, Result);
}

void SDBApurgeTest(const COLwebRequest& Request, SDBsystem* pSystem){
   COL_FUNCTION(SDBApurgeTest);
   COLvar Result;
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&SDBApurgeTestDone, Request.Address);
   SDBpurgeTest(pSystem, pCallback);
}