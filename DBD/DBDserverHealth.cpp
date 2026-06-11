// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverHealth
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 14 November 2025 - 09:41AM
// ---------------------------------------------------------------------------
#include <DBD/DBDserverHealth.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDserverHealth(const COLwebRequest& Request) {
   COL_FUNCTION(DBDserverHealth);
   COLvar Result;
   Result["status"] = "up";
   COLrespondSuccess(Request.Address, Result);
}
