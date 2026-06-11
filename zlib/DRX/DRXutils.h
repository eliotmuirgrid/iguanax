#ifndef __DRX_UTILS_H__
#define __DRX_UTILS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXutils
//
// Description:
//
// Helper functions to check license entitlement limitations.
//
// Author: Vismay Shah
// Date:   Wednesday 29 March 2023 - 05:02PM
// ---------------------------------------------------------------------------
#include <time.h>
#include <COL/COLstring.h>
class COLstring;
class DRXinformation;

int    DRXmessageRateLimit();
COLuint64 DRXlogSearchTimeLimit();
bool DRXsingleSignOnEnabled();
bool DRXhighAvailabilityEnabled();
bool DRXglobalDashboardEnabled();

bool   DRXcomponentCountCheck(int Current, COLstring* pReason);
bool   DRXlicenseExpiryCheck(time_t Now, DRXinformation TempLicense);
bool   DRXlicenseIsEnterprise();
#endif // end of defensive include
