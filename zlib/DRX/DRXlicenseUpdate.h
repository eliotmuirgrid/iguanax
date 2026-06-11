#ifndef __DRX_LICENSE_UPDATE_H__
#define __DRX_LICENSE_UPDATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseUpdate
//
// Description:
//
// Api for updating an existing license or setting a new one.
//
// Author: Vismay Shah 
// Date:   Wednesday 29 March 2023 - 03:53PM
// ---------------------------------------------------------------------------

#include <DRX/DRXlicenseValidator.h>
class COLwebRequest;
class SCKloop;

void DRXlicenseUpdate(const COLwebRequest& Request, SCKloop* pLoop, DRXlicenseValidator* pLicenseValidator);

#endif // end of defensive include
