#ifndef __DRX_LICENSE_REMOTE_H__
#define __DRX_LICENSE_REMOTE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseRemote
//
// Description:
//
// Requests a trial license from the licensing server
//
// Author: Vismay Shah 
// Date:   Tuesday 30 January 2024 - 12:53PM
// ---------------------------------------------------------------------------
class DRXlicenseValidator;

void DRXrequestTrialLicense(DRXlicenseValidator* pValidator);

#endif // end of defensive include
