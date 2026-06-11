#ifndef __DRX_LICENSE_API_H__
#define __DRX_LICENSE_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseApi
//
// Description:
//
// Api to set and retrieve license information.
//
// Author: Vismay Shah
// Date:   Tuesday 28 March 2023 - 04:24PM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>
#include <DRX/DRXlicenseValidator.h> // can't forward declare this
class SCKloop;

void DRXlicenseApi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>> >& ApiMap, SCKloop* pLoop, DRXlicenseValidator* pLicenseValidator);

#endif // end of defensive include
