//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXinformation
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Tuesday, May 13th, 2008 @ 02:21:47 PM
//
//---------------------------------------------------------------------------

#include "DRXinformation.h"
#include <DRX/DRXutils.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

const int       DRXinformation::Version = 1;
const COLstring DRXinformation::ProductName = "Iguana X";

DRXinformation::DRXinformation()
  : m_ExpiryTimestamp(DRXdaysToExpiryToTimestamp(DRX_EXPIRED_VALUE)), m_ComponentLimit(DRX_EXPIRED_VALUE),
    m_LogSearchLimit(DRX_EXPIRED_VALUE), m_MessageRateLimit(0) {
}

DRXinformation::DRXinformation(time_t ExpiryTimestamp, int Components, int LogSearch, int MessageRate, const COLvar& FeatureMap)
 : m_ExpiryTimestamp(ExpiryTimestamp), m_ComponentLimit(Components), m_LogSearchLimit(LogSearch), m_MessageRateLimit(MessageRate), m_Features(FeatureMap) {
}

DRXinformation::DRXinformation(time_t ExpiryTimestamp, int Components, int LogSearch, int MessageRate, bool SingleSignOn, bool HighAvailability, bool GlobalDashboard)
 : m_ExpiryTimestamp(ExpiryTimestamp), m_ComponentLimit(Components), m_LogSearchLimit(LogSearch), m_MessageRateLimit(MessageRate) {
   if (SingleSignOn)       { m_Features[DRX_FEATURE_SINGLE_SIGN_ON]    = true; }
   if (HighAvailability)   { m_Features[DRX_FEATURE_HIGH_AVAILABILITY] = true; }
   if (GlobalDashboard)    { m_Features[DRX_FEATURE_GLOBAL_DASHBOARD]  = true; }
}

DRXinformation::~DRXinformation(){
}

bool DRXinformation::allFeaturesOff() const {
   if (m_Features.isNull()) { return true; }
   const auto& FeatureMap = m_Features.map();
   for (const auto& Entry : FeatureMap) {
      if (!Entry.second.isNull()) { return false; }
   }
   return true;
}

void DRXclearLicense(DRXinformation* pLicense){
   COL_FUNCTION(DRXclearLicense);
   *pLicense = DRXinformation();
}

void DRXinformationToVar(const DRXinformation& LicenseInfo, COLvar* pOut){
   COL_FUNCTION(DRXinformationToVar);
   COLvar& Result = *pOut;
   Result["component_limit"]    = LicenseInfo.componentLimit();
   Result["search_limit"]       = LicenseInfo.logSearchLimit();
   Result["message_rate_limit"] = LicenseInfo.messageRateLimit();
   Result["single_sign_on"]     = LicenseInfo.singleSignOn();
   Result["high_availability"]  = LicenseInfo.highAvailability();
   Result["global_dashboard"]   = LicenseInfo.globalDashboard();
   Result["expiry"]             = COLintToString(LicenseInfo.expiryTimestamp());
   COL_VAR(Result);
}

time_t DRXdaysToExpiryToTimestamp(int DaysToExpiry){
   COL_FUNCTION(DRXdaysToExpiryToTimestamp);
   if (0 == DaysToExpiry) { return 0; } // unlimited case - license does not expire
   time_t Expiry = time(NULL) + (DaysToExpiry * 24 * 60 * 60);
   return Expiry;
}