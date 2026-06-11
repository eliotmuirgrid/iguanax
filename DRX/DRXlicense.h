#ifndef __DRX_LICENSE_H__
#define __DRX_LICENSE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicense
//
// Description:
//
// Api to get license information.
//
// Author: Vismay Shah 
// Date:   Tuesday 28 March 2023 - 04:28PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class DRXlicenseValidator;

void DRXlicenseInfo(const COLwebRequest& Request, DRXlicenseValidator* pLicenseValidator);


#endif // end of defensive include
