// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBGapi
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Wednesday 22 March 2023 - 11:07AM
// ---------------------------------------------------------------------------

#include <DBG/DBGapi.h>
#include <DBG/DBGget.h>
#include <DBG/DBGdelete.h>
#include <DBG/DBGdeleteAll.h>
#include <DBG/DBGlist.h>
#include <DBG/DBGdownloadPdb.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBGapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap){
   COL_FUNCTION(DBGapiAsync);
   ApiMap.add("/debug/crash/file/list",       &DBGlist);
   ApiMap.add("/debug/crash/file/get",        &DBGget);
   ApiMap.add("/debug/crash/file/delete",     &DBGdelete);
   ApiMap.add("/debug/crash/file/delete/all", &DBGdeleteAll);
   ApiMap.add("/debug/info/pdb",              &DBGpdbInfo);
}
