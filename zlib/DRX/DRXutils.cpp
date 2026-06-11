// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXutils
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Wednesday 29 March 2023 - 05:02PM
// ---------------------------------------------------------------------------
#include <DRX/DRXutils.h>
#include <DRX/DRXinformation.h>
#include <DRX/DRXlicenseCheck.h>
#include <DRX/DRXlicenseValidator.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DRXcomponentCountCheck(int Current, COLstring* pReason){
   COL_FUNCTION(DRXcomponentCountCheck);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   int LicenseStatus = Entitlement["license_status"];
   if (DRXlicenseValidator::LICENSE_OK != LicenseStatus){
      *pReason = DRXlicenseValidatorResultToString(LicenseStatus);
      COL_TRC("License not ok: " << *pReason);
      return false;
   }
   // Only checking limit if we have a valid license.
   int ComponentLimit = Entitlement["component_limit"];
   if (ComponentLimit == 0) {
      COL_TRC("No component limit for this license.");
      return true;
   }
   if (Current > ComponentLimit){
      COL_VAR2(Current, ComponentLimit);
      *pReason = "Hit license limit. License entitlement: " + COLintToString(ComponentLimit);
      return false;
   }
   return true;
}

COLuint64 DRXlogSearchTimeLimit(){
   COL_FUNCTION(DRXlogSearchTimeLimit);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   int LicenseStatus = Entitlement["license_status"];
   if (DRXlicenseValidator::LICENSE_OK != LicenseStatus){
      COLstring LicenseError = DRXlicenseValidatorResultToString(LicenseStatus);
      COL_TRC("License not ok: " << LicenseStatus);
      return time(NULL);
   }
   // Only checking limit if we have a valid license.
   int Days = Entitlement["search_limit"];
   if(Days == 0){
      COL_VAR("There is not limit on log searching.");
      return 0;
   }
   COL_VAR(Days);
   time_t Now = time(NULL);
   time_t TimeLimit = Now - (Days * 24 * 60 * 60);
   return TimeLimit * 1000;
}

int DRXmessageRateLimit(){
   COL_FUNCTION(DRXmessageRateLimit);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   return Entitlement["message_rate_limit"].asInt();
}

bool DRXsingleSignOnEnabled(){
   COL_FUNCTION(DRXsingleSignOnEnabled);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   return Entitlement["single_sign_on"].asBool();
}

bool DRXhighAvailabilityEnabled(){
   COL_FUNCTION(DRXhighAvailabilityEnabled);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   return Entitlement["high_availability"].asBool();
}

bool DRXglobalDashboardEnabled() {
   COL_FUNCTION(DRXglobalDashboardEnabled);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   return Entitlement["global_dashboard"].asBool();
}


bool DRXlicenseExpiryCheck(time_t Now, DRXinformation TempLicense){
   COL_FUNCTION(DRXlicenseExpiryCheck);
   time_t ExpiryTime = TempLicense.expiryTimestamp();
   if (0 == ExpiryTime){
      COL_TRC("This license doesn't expire");
      return true;
   }
   return ExpiryTime > Now;
}

bool DRXlicenseIsEnterprise(){
   COL_FUNCTION(DRXlicenseIsEnterprise);
   COLvar Entitlement;
   DRXlicenseEntitlement(&Entitlement);
   if ((Entitlement.exists("component_limit")    && Entitlement["component_limit"].asInt() == 0) && // 0 = no limit = unlimited
       (Entitlement.exists("search_limit")       && Entitlement["search_limit"].asInt() == 0) && // 0 = no search limit = unlimited
      //  (Entitlement.exists("message_rate_limit") && Entitlement["message_rate_limit"].asInt() == 0)  && // not used?
       (Entitlement.exists("expiry")             && Entitlement["expiry"].asInt() == 0)){ // 0 = doesn't expire

      COL_TRC("Everything unlimited, this is an enterprise license");
      return true;
   }
   return false;
}