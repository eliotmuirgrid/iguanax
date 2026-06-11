#ifndef __DRX_INFORMATION_H__
#define __DRX_INFORMATION_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2011 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXinformation
//
// Description:
//
// DRXinformation - license information for Iguana.
//
// Author: Eliot Muir
// Date:   Tuesday, May 13th, 2008 @ 02:21:47 PM
//
//---------------------------------------------------------------------------

#include <time.h>
#include <COL/COLstring.h>
#include <COL/COLvar.h>

#define DRX_EXPIRED_VALUE -1

#define DRX_FEATURE_SINGLE_SIGN_ON     "SSO"
#define DRX_FEATURE_HIGH_AVAILABILITY  "HA"
#define DRX_FEATURE_GLOBAL_DASHBOARD   "GDASH"

class COLvar;
class DRXinformation{
public:
   DRXinformation();
   // constructor for license parsing
   DRXinformation(time_t ExpiryTimestamp, int Components, int LogSearch, int MessageRate, const COLvar& FeatureMap);
   // constructor for license generator
   DRXinformation(time_t ExpiryTimestamp, int Components, int LogSearch, int MessageRate, bool SingleSignOn = false, bool HighAvailability = false, bool GlobalDashboard = false);
   ~DRXinformation();

   time_t expiryTimestamp()  const { return m_ExpiryTimestamp; }
   int    componentLimit()   const { return m_ComponentLimit; }
   int    logSearchLimit()   const { return m_LogSearchLimit; }
   int    messageRateLimit() const { return m_MessageRateLimit; }
   // features
   const COLvar& features()  const { return m_Features; }
   bool   allFeaturesOff()   const;
   bool   singleSignOn()     const { return m_Features[DRX_FEATURE_SINGLE_SIGN_ON]; }
   bool   highAvailability() const { return m_Features[DRX_FEATURE_HIGH_AVAILABILITY]; }
   bool   globalDashboard()  const { return m_Features[DRX_FEATURE_GLOBAL_DASHBOARD]; }

   static const int       Version;
   static const COLstring ProductName;
private:
   time_t m_ExpiryTimestamp;
   int    m_ComponentLimit;
   int    m_LogSearchLimit;   // in days
   int    m_MessageRateLimit; // queue.push allowed per minute - not implemented in crm/mils so no yet used
   COLvar m_Features;         // use COLvar because some features are ON/OFF while others could have limits
};

void DRXclearLicense(DRXinformation* pLicense);
void DRXinformationToVar(const DRXinformation& LicenseInfo, COLvar* pOut);
time_t DRXdaysToExpiryToTimestamp(int DaysToExpiry);

#endif // end of defensive include
