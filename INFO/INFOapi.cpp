//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INFOapi
//
// Description:
//
// API for info stats for IguanaX monitoring
//
// Author: Matthew Sobkowski
// Date:   Tue 11 May 2021 1:03:45 EDT
//---------------------------------------------------------------------------

#include <INFO/INFOapi.h>
#include <INFO/INFOpanelStats.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void INFOapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap) {
   COL_FUNCTION(INFOapi);
   ApiMap.add("/info/side_panel_stats", COLnewClosure1(&INFOsidePanelStats));
}
