// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SECapi
//
// Description:
//
// Author: Matthew Sobkowski
// Date: 2024-11-28
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <SEC/SECapi.h>
#include <SEC/SECdata.h>
#include <SEC/SECupdate.h>
COL_LOG_MODULE;

void SECapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop) {
   COL_FUNCTION(SANDapi);
   ApiMap.add("/security/set", COLnewClosure1(&SECupdate, pLoop));
   ApiMap.add("/security/get", COLnewClosure1(&SECdata));
}
