// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicense
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 28 March 2023 - 04:28PM
// ---------------------------------------------------------------------------
#include <DRX/DRXlicenseValidator.h>
#include <DRX/DRXlicense.h>
#include <DRX/DRXiguanaId.h>
#include <DRX/DRXinformation.h>
#include <DRX/DRXutils.h>

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
COL_LOG_MODULE;

static void DRXlicenseFormatLicenseData(DRXlicenseValidator* pLicenseValidator, COLvar* pOut){
   COL_FUNCTION(DRXlicenseFormatLicenseData);
   COLvar& Result = *pOut;
   COLvar Data;
   DRXinformationToVar(pLicenseValidator->license(), &Data);
   COL_TRC("License data successfully loaded");
   Data["license_status"] = DRXlicenseValidatorResultToString(pLicenseValidator->loadLicense());
   Result["license"] = Data;
}

// /license/get
void DRXlicenseInfo(const COLwebRequest& Request, DRXlicenseValidator* pLicenseValidator){
   COL_FUNCTION(DRXlicenseInfo);
   COLvar Result;
   // Can always get IguanaId
   Result["iguana_id"] = DRXiguanaId();
   // Get license details
   DRXlicenseValidator::eLicenseCheckResult LicenseResult = pLicenseValidator->loadLicense();
   if (DRXlicenseValidator::LICENSE_OK == LicenseResult){
      COL_TRC("License exists, try to load data");
      DRXlicenseFormatLicenseData(pLicenseValidator, &Result);
      Result["license_message"] = "To modify or extend your license, contact the iNTERFACEWARE Sales Office.";
   } else if (DRXlicenseValidator::NO_LICENSE_FOUND == LicenseResult) {
      COL_TRC("License not found. Return the free license information");
      DRXlicenseFormatLicenseData(pLicenseValidator, &Result);
      Result["license_message"] = "No license file was detected. If you are trying out Iguana, ensure you have internet access and restart Iguana to receive a trial license code.\nOtherwise, you can contact the iNTERFACEWARE Sales Office to purchase a license.";
   } else if (DRXlicenseValidator::LICENSE_EXPIRED == LicenseResult){
      COL_TRC("License has expired. Return the free license information");
      DRXlicenseFormatLicenseData(pLicenseValidator, &Result);
      Result["license_message"] = "Your existing license has expired.\nContact the iNTERFACEWARE Sales Office to purchase or renew your license.";
   } else if (DRXlicenseValidator::IGUANA_ID_MISMATCH == LicenseResult){
      COL_TRC("License for a different Iguana.\nContact the iNTERFACEWARE Sales Office to replace license.");
      Result["license_error"] = DRXlicenseValidatorResultToString(DRXlicenseValidator::IGUANA_ID_MISMATCH);
   } else {
      COL_TRC("Loaded license is invalid");
      Result["license_error"] = DRXlicenseValidatorResultToString(LicenseResult);
   }
   COLrespondSuccess(Request.Address, Result);
}
