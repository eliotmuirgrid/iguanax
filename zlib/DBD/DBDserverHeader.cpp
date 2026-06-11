// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDserverHeader
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 21 September 2022 - 06:05PM
// ---------------------------------------------------------------------------
#include <DBD/DBDserverHeader.h>

#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <DBDVER/DBDVERversion.h>
#include <IWEB/IWEBconfig.h>
COL_LOG_MODULE;

// /header
void DBDserverHeader(const COLwebRequest& Request, IWEBconfig* pConfig) {
   COL_FUNCTION(DBDserverHeader);
   COLvar Result;
   Result["server_description"] = COLexpandEnvironmentVariables(pConfig->serverDescription());
   Result["server_color"]       = COLexpandEnvironmentVariables(pConfig->serverColour());
   Result["version"]            = DBD_STRING_VERSION;
   COLrespondSuccess(Request.Address, Result);
}
