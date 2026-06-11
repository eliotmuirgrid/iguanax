// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseRemote
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 30 January 2024 - 12:53PM
// ---------------------------------------------------------------------------

#include <DRX/DRXlicenseRemote.h>
#include <DRX/DRXiguanaId.h>
#include <DRX/DRXlicenseValidator.h>
#include <WEB/WEBrequest.h>
#include <WEB/WEBresponse.h>
#include <FIL/FILutils.h>
#include <DIR/DIRpath.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBcapi.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#define DRX_LICENSE_SERVER_URL "https://my.interfaceware.com/trial_license"

static void DRXlicenseSaved(bool Success, COLstring Error, DRXlicenseValidator* pValidator) {
   COL_FUNCTION(DRXlicenseSaved);
   COL_VAR2(Success, Error);
   if (Success){
      COLstring DummyString;
      COLostream DummyStream(DummyString);
      pValidator->loadLicenseFromFile(DummyStream);
   } else {
      COL_TRC("Failed to save file");
      SDBIlogService("Could not save license code. Please make sure IguanaX has the necessary permissions for its working directory and restart to try again.", "#error");
   }
}

static void DRXhandleTrialLicenseResponse(DRXlicenseValidator* pValidator, const WEBresponse& Response){
   COL_FUNCTION(DRXhandleTrialLicenseResponse);
   COL_VAR(Response.ReturnCode);
   if (200 == Response.ReturnCode){
      COLvar Data;
      Data.parse(Response.Body);
      COL_VAR(Data);
      if (Data["success"].asBool()){
         SCKwriteFile(pValidator->m_pLoop, DIRfileConfigLicense(), Data["license"], COLnewClosure2(&DRXlicenseSaved, pValidator));
      } else {
         COL_TRC("License server responded with error");
         SDBIlogService("License server failed to return a license code. Please contact iNTERFACEWARE Sales for support.", "#error");
      }
   } else {
      COL_TRC("Product was not IguanaX, or maybe server was not reachable");
      SDBIlogService("License server failed to return a license code. Please contact iNTERFACEWARE Sales for support.", "#error");
   }
}

void DRXrequestTrialLicense(DRXlicenseValidator* pValidator){
   COL_FUNCTION(DRXrequestTrialLicense);
   WEBrequest  Request;
   WEBresponse Response;
   Request.Url     = DRX_LICENSE_SERVER_URL;
   Request.Timeout = 3000;
   Request.GetVariables.add("product", "iguanax");
   Request.GetVariables.add("instanceid", DRXiguanaId());
   WEBrequestBlockingGet(Request, &Response);
   DRXhandleTrialLicenseResponse(pValidator, Response);
}