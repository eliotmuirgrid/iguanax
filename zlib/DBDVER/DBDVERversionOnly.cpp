// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERapi
//
// Description: Implementation
//
// Author: Matthew Sobkowski
// Date:   Friday 1 September 2023 - 12:07PM
// ---------------------------------------------------------------------------
#include <DBDVER/DBDVERversionOnly.h>
#include <DBDVER/DBDVERversion.h>

#include <COL/COLweb.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDBVERversionOnly (const COLwebRequest& Request) {
   COLvar Result;
   Result["version"] = DBD_STRING_VERSION;
   COL_VAR(Result);
   COLrespondSuccess(Request.Address, Result);
}