//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDsourceHash.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/11/23 10:08 AM
//  ---------------------------------------------------------------------------
#include <DBD/DBDsourceHash.h>

#include <CACHE/CACHEfile.h>

#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void DBDsourceHash(const COLwebRequest& Request) {
   COL_FUNCTION(DBDsourceHash);
   COLvar Result;
   Result["hash"] = CACHEhash();
   COLrespondSuccess(Request.Address, Result);
}
