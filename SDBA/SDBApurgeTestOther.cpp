// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeTestOther
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 14 February 2025 - 01:26PM
// ---------------------------------------------------------------------------

#include <SDBA/SDBApurgeTestOther.h>
#include <SDB/SDBpurgeTest.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBApurgeTestOtherDone(COLvar Result, COLaddress Address){
   COL_FUNCTION(SDBApurgeTestOtherDone);
   COL_VAR(Result);
   COLrespondSuccess(Address, Result);
}

void SDBApurgeTestOther(const COLwebRequest& Request, SDBsystem* pSystem){
   COL_FUNCTION(SDBApurgeTestOther);
   COLvar Result;
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&SDBApurgeTestOtherDone, Request.Address);
   SDBpurgeTestOther(pSystem, pCallback);
}
