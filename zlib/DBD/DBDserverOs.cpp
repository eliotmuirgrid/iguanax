//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: DBDserverOs.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   06/11/23 10:47 PM
//  ---------------------------------------------------------------------------
#include <DBD/DBDserverOs.h>

#include <COL/COLos.h>
#include <COL/COLlog.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void DBDserverOs(const COLwebRequest& Request) {
   COL_FUNCTION(DBDserverOs);
   return COLrespondSuccess(Request.Address, COLos());
}