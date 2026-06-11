// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseUpdate
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 29 March 2023 - 03:53PM
// ---------------------------------------------------------------------------

#include <DRX/DRXlicenseUpdate.h>

#include <PRM/PRMcallbacks.h>

#include <ROL/ROLid.h>

#include <COL/COLlog.h>
#include <COL/COLthreadPool.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <DRX/DRXutils.h>
#include <SDB/SDBcapi.h>
#include <SCK/SCKloop.h>
COL_LOG_MODULE;

static void DRXwriteLicenseDetailsToLogs(DRXlicenseValidator* pLicenseValidator, COLostream& LogStream){
   COL_FUNCTION(DRXwriteLicenseDetailsToLogs);
   DRXinformation LicenseInfo = pLicenseValidator->license();
   COL_TRC("License data successfully loaded");
   LogStream << COL_T("Maximum Number of Components: ") << LicenseInfo.componentLimit() << newline;
   LogStream << COL_T("Log Search Limit: ") << LicenseInfo.logSearchLimit() << newline;
   LogStream << COL_T("Message Rate Limit: ") << LicenseInfo.messageRateLimit() << newline;
}

static void DRXlogOldLicenseDetails(COLostream& LogStringStream, DRXlicenseValidator* pLicenseValidator){
   // If a previous license existed log the details.
   DRXlicenseValidator::eLicenseCheckResult LicenseResult = pLicenseValidator->loadLicense();
   if (LicenseResult == DRXlicenseValidator::LICENSE_OK ||
       LicenseResult == DRXlicenseValidator::LICENSE_EXPIRED){
      COL_TRC("Retrieving and logging old license details.");
      LogStringStream << COL_T("Old License Entitlement:") << newline;
      DRXwriteLicenseDetailsToLogs(pLicenseValidator, LogStringStream);
   }
}

static void DBDlogNewLicenseDetails(COLostream& LogStringStream, DRXlicenseValidator* pLicenseValidator){
   COL_TRC("Retrieving and logging new license details.");
   LogStringStream << COL_T("New License Entitlement:") << newline;
   DRXwriteLicenseDetailsToLogs(pLicenseValidator, LogStringStream);
}

static void DRXlicenseUpdateImpl(const COLstring& NewLicense, DRXlicenseValidator* pLicenseValidator, SCKloop* pLoop, const COLaddress& Address){
   COL_FUNCTION(DRXlicenseUpdateImpl);
   COL_VAR(NewLicense);
   COLstring LicenseTestResult;
   COL_TRC("Attempting to validate new license key...");
   if (!pLicenseValidator->validateLicense(NewLicense, &LicenseTestResult)){
      COL_TRC("Error while validating license key.");
      return COLrespondError(Address, LicenseTestResult);
   }
   COLstring  LogString;
   COLostream LogStringStream(LogString);
   LogStringStream << COL_T("License key validated") << COL_T(".") << newline << newline;
   COL_TRC("License key validated.");
   DRXlogOldLicenseDetails(LogStringStream, pLicenseValidator);
   COL_TRC("Updating license details...");
   pLicenseValidator->updateLicense(NewLicense);
   COL_TRC("License successfully updated");
   // Retrieve the new license details and log them. The new details are returned to the user.
   // LogStringStream << COL_T("License updated by user \"") + Username << COL_T("\".") << newline << newline;  // TODO: figure out which user performed the update
   DBDlogNewLicenseDetails(LogStringStream, pLicenseValidator);
   // Add the created log reflecting the license changes.
   SDBIlogService(LogString);
   COLrespondSuccess(Address);
}

// /license/set
void DRXlicenseUpdate(const COLwebRequest& Request, SCKloop* pLoop, DRXlicenseValidator* pLicenseValidator){
   COL_FUNCTION(DRXlicenseUpdate);
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_LICENSE, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_LICENSE));
   }
   const COLvar& Data = Request.Data;
   // TODO - Discuss if this is worth spawning a thread for.
   DRXlicenseUpdateImpl(COLvarRequiredString(Data, "license"), pLicenseValidator, pLoop, Request.Address);
}