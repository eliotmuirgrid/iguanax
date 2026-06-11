//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: PEERserverGuid.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   22/01/25 11:44 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLweb.h>
#include <IWEB/IWEBconfig.h>
#include <PEER/PEERserverGuid.h>
COL_LOG_MODULE;

// /peers/server_guid
void PEERserverGuid(const COLwebRequest& Request, IWEBconfig* pConfig) {
   COL_FUNCTION(PEERserverGuid);
   COLvar Data;
   Data["endpoint"] = "/peers/server_guid";
   Data["guid"]     = pConfig->uniqueId();
   COLrespondSuccess(Request.Address, Data);
}