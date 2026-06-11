#ifndef __DRX_LICENSE_VALIDATOR_H__
#define __DRX_LICENSE_VALIDATOR_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2010 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseValidator
//
// Description:
//
// I think this is used by other parts of Iguana to check the license status
// of the application?
//
// Author: Henry Tran
// Date: July 7, 2004
//
//---------------------------------------------------------------------------

#include <SIG/SIGsignaller2.h>
#include <DRX/DRXinformation.h>
#include <COL/COLclosure.h>

#ifndef DISABLE_RGN_OBFUSCATION
// See #12100 for rationale for license code obfuscation.
#  define DRXlicenseValidator        dfuihe9rh
#  define DRXlicenseValidatorPrivate jk0sdfg0
#  define licenseExpired             dskf9fe893
#  define licenseExpiredOrExpiringSoon sx25psqvua
#  define maintenanceExpired         nmd89wjd2
#  define channelVacancy             o348fjdf8y
#endif

class DRXinformation;
class SCKloop;

class DRXlicenseValidator{
public:
   DRXlicenseValidator(SCKloop* pLoop);
   ~DRXlicenseValidator();
   // Return codes for the installer
   // These must be kept in sync with nsis_iguana/iguana-utils.nsh
   enum eLicenseCheckResult{
      LICENSE_NOT_LOADED = -1,
      LICENSE_OK = 0,
      LICENSE_LOAD_ERROR = 1,
      NO_LICENSE_FOUND = 2,
      LICENSE_EXPIRED = 3,
      IGUANA_ID_MISMATCH = 4
   };

   void updateLicense  (const COLstring& NewHexString); 
   bool validateLicense(const COLstring& NewLicenseHex, COLstring* pErrorMessage) const;

   const DRXinformation& license();
   eLicenseCheckResult loadLicense();
   eLicenseCheckResult loadLicenseFromFile(COLostream& Out);
   COLclosure0* m_OnLicenseExpired = nullptr;
   SCKloop* m_pLoop;
   // int m_DRXpushCountTimerId; // IX-2935 Enable if message rate limit is enabled
   int m_LicenseExpiryCheckTimer;
private:
   DRXinformation      m_LicenseInformation;
   eLicenseCheckResult m_CachedLoadResult;
   DRXlicenseValidator( const DRXlicenseValidator& Orig ); // not allowed
   DRXlicenseValidator& operator=( const DRXlicenseValidator& Orig ); // not allowed
};

void DRXonExpiredCallbackDone();
void DRXsetupOnExpiredCallback(DRXlicenseValidator* pValidator, COLclosure0* pCallback);
void DRXsetupLicenseTimers(DRXlicenseValidator* pValidator, SCKloop* pLoop);
void DRXcleanupTimers     (DRXlicenseValidator* pValidator, SCKloop* pLoop);
// Function called by the load license timer
void DRXloadLicense       (DRXlicenseValidator* pValidator);

COLstring DRXlicenseValidatorResultToString(int ResultCode);

#endif // end of defensive include
