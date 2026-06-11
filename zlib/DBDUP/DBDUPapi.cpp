// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDUPapi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Monday 14 August 2023 - 10:15AM
// ---------------------------------------------------------------------------

#include <DBDUP/DBDUPapi.h>
#include <DBDUP/DBDUPreleaseInfo.h>
#include <DBDUP/DBDUPreleaseNotes.h>
#include <DBDUP/DBDUPisService.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDUPapi(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(DBDUPapi);
   ApiMap.add("/release/info",             DBDUPreleaseInfo);
   ApiMap.add("/release/changelog",        DBDUPreleaseNotes);
   ApiMap.add("/release/is_service",       DBDUPisService);
}