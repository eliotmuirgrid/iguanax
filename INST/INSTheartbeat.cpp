// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTheartbeat
//
// Description: Installer heartbeat
//
// Author: Matthew Sobkowski
// Date:   Friday 11 August 2023
// ---------------------------------------------------------------------------
#include <INST/INSTheartbeat.h>

#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// install/heartbeat
void INSTheartbeat(const COLwebRequest& Request){
   COL_FUNCTION(INSTheartbeat);
   return COLrespondSuccess(Request.Address);
}