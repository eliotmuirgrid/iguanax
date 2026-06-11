// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseApi
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 28 March 2023 - 04:24PM
// ---------------------------------------------------------------------------
#include <DRX/DRXlicenseApi.h>
#include <DRX/DRXlicense.h>
#include <DRX/DRXlicenseUpdate.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void DRXlicenseApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DRXlicenseValidator* pLicenseValidator){
   COL_FUNCTION(DRXlicenseApi);
   ApiMap.add("/license/get", COLnewClosure1(&DRXlicenseInfo, pLicenseValidator));
   ApiMap.add("/license/set", COLnewClosure1(&DRXlicenseUpdate, pLoop, pLicenseValidator));
}