//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOpanelStats
//
// Description:
//
// API for info stats for IguanaX monitoring
//
// Author: Matthew Sobkowski
// Date:   Tue 11 May 2021 1:03:45 EDT
//---------------------------------------------------------------------------
#include <INFO/INFOpanelStats.h>
#include <INFO/INFOusage.h>

#include <DBDVER/DBDVERversion.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

void INFOgetFdUsage(COLvar* pResponse){
   COL_FUNCTION(INFOgetFdUsage);
   COLvar& Response = *pResponse;
   Response["max"]  = INFOfileHandleLimit();
   Response["used"] = INFOfileHandleCount();
}

// /info/side_panel_stats
void INFOsidePanelStats(const COLwebRequest& Request) {
   COL_FUNCTION(INFOsidePanelStats);
   COLvar Response;
   Response["version"] = DBD_STRING_VERSION;
   INFOmemoryUsage(&Response);
   INFOcpuUsage(&Response);
   INFOdiskUsage(&Response);
   INFOgetFdUsage(&Response);
   COLrespondSuccess(Request.Address, Response);
}