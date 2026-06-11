// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseCheck
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 18 January 2024 - 11:17AM
// ---------------------------------------------------------------------------

#include <DRX/DRXlicenseCheck.h>
#include <DRX/DRXlicense.h>
#include <DRX/DRXlicenseValidator.h>

#include <SCK/SCKloop.h>

#include <COL/COLevent.h>
#include <COL/COLioCheck.h>
#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static SCKloop* pDRXsckLoop = nullptr;
static COLauto<COLclosure1<COLvar*>> pDRXlicenseChecker = NULL;

static void DRXlicenseChecker(COLvar* pOut, DRXlicenseValidator* pValidator){
   COL_FUNCTION(DRXlicenseChecker);
   DRXinformation LicenseInfo = pValidator->license();
   DRXinformationToVar(LicenseInfo, pOut); 
   (*pOut)["license_status"] = (int)(pValidator->loadLicense());
}

void DRXsetLicenseCheckCallback(SCKloop* pLoop, DRXlicenseValidator* pValidator){
   COL_FUNCTION(DRXsetLicenseCheckCallback);
   pDRXsckLoop = pLoop;
   pDRXlicenseChecker = COLnewClosure1(&DRXlicenseChecker, pValidator);
}

static void DRXcheckLicenseOnMain(COLvar* pLicenseResult, COLevent* pEvent){
   COL_FUNCTION(DRXcheckLicenseOnMain);
   pDRXlicenseChecker->run(pLicenseResult);
   pEvent->signal();
}

// This function can be run from a worker thread or the main thread
void DRXlicenseEntitlement(COLvar* pLicenseResult){
   COL_FUNCTION(DRXlicenseEntitlement);
   if (COLisMainThread()) {
      COL_TRC("On main thread already, can just run the closure");
      pDRXlicenseChecker->run(pLicenseResult);
   } else {
      COL_TRC("On worker thread, will have to post back to main first");
      if (pDRXsckLoop){
         COLevent Event;
         pDRXsckLoop->post(COLnewClosure0(&DRXcheckLicenseOnMain, pLicenseResult, &Event));
         Event.wait();
         return;
      }
   }
}