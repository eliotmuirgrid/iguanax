#ifndef __DRX_LICENSE_CHECK_H__
#define __DRX_LICENSE_CHECK_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseCheck
//
// Description:
//
// Retrieve the relevant license entitlement information from any thread.
//
// Author: Tyler Brown 
// Date:   Thursday 18 January 2024 - 11:17AM
// ---------------------------------------------------------------------------
class COLvar;
class DRXlicenseValidator;
class SCKloop;

void DRXsetLicenseCheckCallback(SCKloop* pLoop, DRXlicenseValidator* pValidator);
void DRXlicenseEntitlement(COLvar* pLicenseResult);

#endif // end of defensive include
