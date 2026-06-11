// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPisService
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 18 August 2023 - 01:42PM
// ---------------------------------------------------------------------------

#include <DBDUP/DBDUPisService.h>

#include <SDM/SDMserver.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// /release/is_service
void DBDUPisService(const COLwebRequest& Request){
   COL_FUNCTION(DBDUPisService);
   COLvar Response;
   Response["is_service"] = SDMrunningAsService();
   COLrespondSuccess(Request.Address, Response);
}