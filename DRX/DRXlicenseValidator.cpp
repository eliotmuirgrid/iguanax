//---------------------------------------------------------------------------
// Copyright (C) 1997-2012 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DRXlicenseValidator
//
// Description:
//
// Implementation
//
// Author: Henry Tran
// Date: July 7, 2004
//---------------------------------------------------------------------------
#include "DRXlicenseValidator.h"
#include <DRX/DRXiguanaId.h>
#include <DRX/DRXlicenseRemote.h>
#include <DRX/DRXrate.h>
#include <DRX/DRXserialization.h>
#include <DRX/DRXutils.h>

#include <DBDVER/DBDVERversion.h>

#include <SIG/SIGsignaller.h>

#include <DIR/DIRpath.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <PRO/PROenv.h>
#include <RGN/RGNmachineId.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>

#include <COL/COLdateTime.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#ifdef CHM_LINUX
#include <sys/utsname.h>
#endif

DRXlicenseValidator::DRXlicenseValidator(SCKloop* pLoop)
: m_CachedLoadResult(LICENSE_NOT_LOADED),
  m_pLoop(pLoop){
   COL_METHOD(DRXlicenseValidator::DRXlicenseValidator);
}

DRXlicenseValidator::~DRXlicenseValidator(){
   COL_METHOD(DRXlicenseValidator::~DRXlicenseValidator);
   DRXcleanupTimers(this, m_pLoop);
   if (m_OnLicenseExpired) {
      delete m_OnLicenseExpired;
   }
}

const DRXinformation& DRXlicenseValidator::license(){
   COL_FUNCTION(DRXlicenseValidator::license);
   if (loadLicense() != DRXlicenseValidator::LICENSE_OK){
      COL_TRC("License load error");
      // throw COLerror(ErrorString, 0);
   }
   return m_LicenseInformation;
}

COLstring DRXlicenseValidatorResultToString(int ResultCode){
   COL_FUNCTION(DRXlicenseValidatorResultToString);
   switch (ResultCode){
      case DRXlicenseValidator::LICENSE_OK:           return "License ok";
      case DRXlicenseValidator::LICENSE_LOAD_ERROR:   return "License load error. Your IguanaX license file is corrupted. Contact the iNTERFACEWARE Sales Office to obtain a new license.";
      case DRXlicenseValidator::NO_LICENSE_FOUND:     return "No license found";
      case DRXlicenseValidator::LICENSE_EXPIRED:      return "License expired";
      case DRXlicenseValidator::IGUANA_ID_MISMATCH:   return "License load error. The license code was created for another IguanaX instance. Contact the iNTERFACEWARE Sales Office to obtain a new license or transfer an existing one.";
      case DRXlicenseValidator::LICENSE_NOT_LOADED:
      default:                                        return "No license loaded";
   }
}

// This used to load the license from file if it wasn't already loaded. In IX-2935 that was factored out to
// loadLicenseFromFile. This now just returns the cached result.
DRXlicenseValidator::eLicenseCheckResult DRXlicenseValidator::loadLicense(){
   COL_METHOD(DRXlicenseValidator::loadLicense);
   COL_VAR(m_CachedLoadResult);
   COL_TRC("Already loaded license.");
   return m_CachedLoadResult;
}

DRXlicenseValidator::eLicenseCheckResult DRXlicenseValidator::loadLicenseFromFile(COLostream& Out){
   COL_METHOD(DRXlicenseValidator::loadLicenseFromFile);
   try{
      // DRXclearLicense(&m_LicenseInformation); // this sets it to the free no license state
      COLstring LicenseFilePath = DIRfileConfigLicense();
      COL_VAR(LicenseFilePath);
      if (!FILfileExists(LicenseFilePath)){ // Free, out of the box, no license case
         COL_TRC("No license file on disk");
         // #10830 - Watch out!  Brittle dependencies on this text may exist in the Iguana windows installer
         // read ticket before altering this.
         Out << "iNTERFACEWARE " << DBD_APP_NAME << " license required." << newline
             << "Contact the iNTERFACEWARE Sales Office to purchase a license." << newline;
         m_CachedLoadResult = DRXlicenseValidator::NO_LICENSE_FOUND;
         return DRXlicenseValidator::NO_LICENSE_FOUND;
      }
      COL_TRC("Loading license file.");
      COLstring StringSink;
      FILreadFile(LicenseFilePath, &StringSink);
      COL_VAR(StringSink);
      COLstring IguanaId = DRXiguanaId();
      DRXinformation Temp;
      DRXparseLicense(StringSink, IguanaId, &Temp); // First parse into temp because if the license is expired we want to fall back to the free no license state
      if (!DRXlicenseExpiryCheck(time(nullptr), Temp)) {
         COL_TRC("License has expired");
         m_LicenseInformation = Temp;
         m_OnLicenseExpired->run();
         m_CachedLoadResult = DRXlicenseValidator::LICENSE_EXPIRED;
         return DRXlicenseValidator::LICENSE_EXPIRED;
      }
      m_LicenseInformation = Temp;
      COL_TRC(m_LicenseInformation.ProductName);
      COL_TRC("License OK");
      m_CachedLoadResult = DRXlicenseValidator::LICENSE_OK;
      return DRXlicenseValidator::LICENSE_OK;
   }
   catch ( COLerror& Error ){
      Out << "Error while attempting to read license information:" << newline
          << Error.description();
      if (Error.code() == DRX_IGUANA_ID_MISMATCH){
         COL_TRC("IGUANA_ID_MISMATCH!");
         m_CachedLoadResult = DRXlicenseValidator::IGUANA_ID_MISMATCH;
         return DRXlicenseValidator::IGUANA_ID_MISMATCH;
      } else {
         COL_TRC("LICENSE_LOAD_ERROR!");
         SDBIlogService(DRXlicenseValidatorResultToString(DRXlicenseValidator::LICENSE_LOAD_ERROR), "#error");
         m_CachedLoadResult = DRXlicenseValidator::LICENSE_LOAD_ERROR;
         return DRXlicenseValidator::LICENSE_LOAD_ERROR;
      }
   }
}

void DRXlicenseValidator::updateLicense(const COLstring& NewHexString){
   COL_METHOD(DRXlicenseValidator::updateLicense);
   COLstring FileName = DIRfileConfigLicense();
   COL_VAR2(FileName, NewHexString);
   FILwriteFile(FileName, NewHexString);
   m_CachedLoadResult = LICENSE_NOT_LOADED;
   COLstring DummyString;
   COLostream DummyStream(DummyString);
   loadLicenseFromFile(DummyStream);
   COL_VAR(DummyString);
}

bool DRXlicenseValidator::validateLicense(const COLstring& NewLicenseHex, COLstring* pErrorMessage) const{
   COL_METHOD(DRXlicenseValidator::validateLicense);
   COL_VAR(NewLicenseHex);
   // check the License for validity
   COLstring IguanaId = DRXiguanaId();
   DRXinformation TestLicense;
   try{
      DRXparseLicense(NewLicenseHex, IguanaId, &TestLicense);
      return true;
   } catch(const COLerror& Error){
      COLostream ErrorStream(*pErrorMessage);
      ErrorStream << Error.description();
      return false;
   }
}

void DRXonExpiredCallbackDone(){
   COL_FUNCTION(DRXonExpiredCallbackDone);
   COL_TRC("All components stopped.");
   // intentionally does nothing
}

void DRXsetupOnExpiredCallback(DRXlicenseValidator* pValidator, COLclosure0* pCallback){
   COL_FUNCTION(DRXsetupOnExpiredCallback);
   pValidator->m_OnLicenseExpired = pCallback;
}

void DRXsetupLicenseTimers(DRXlicenseValidator* pValidator, SCKloop* pLoop){
   COL_FUNCTION(DRXsetupLicenseTimers);
   // pValidator->m_DRXpushCountTimerId = pLoop->addTimer(COLnewClosure0(&DRXresetCount), 60 * 1000, true, "DRX push count reset timer"); // IX-2935 Uncomment if message rate limit is enabled
   pValidator->m_LicenseExpiryCheckTimer = pLoop->addTimer(COLnewClosure0(&DRXloadLicense, pValidator), 24 * 60 * 60 * 1000, true, "License expiry check timer");
}

void DRXcleanupTimers(DRXlicenseValidator* pValidator, SCKloop* pLoop){
   COL_FUNCTION(DRXcleanupTimers);
   COL_TRC("Cleaning up push count reset timer");
   // pLoop->cancelTimer(pValidator->m_DRXpushCountTimerId); // IX-2935 Uncomment if message rate limit is enabled
   pLoop->cancelTimer(pValidator->m_LicenseExpiryCheckTimer);
}

void DRXloadLicense(DRXlicenseValidator* pValidator){
   COL_FUNCTION(DRXloadLicense);
   COLstring ErrorMessage;
   COLostream ErrorStream(ErrorMessage);
   DRXlicenseValidator::eLicenseCheckResult LicenseResult = pValidator->loadLicenseFromFile(ErrorStream);
   if (DRXlicenseValidator::LICENSE_OK == LicenseResult){
      COL_TRC("License is ok");
   }
   else if (DRXlicenseValidator::NO_LICENSE_FOUND == LicenseResult){
      COL_TRC("No license file found, try obtaining a trial license");
      DRXrequestTrialLicense(pValidator);
   }
   else if (DRXlicenseValidator::LICENSE_NOT_LOADED == LicenseResult){
      COL_TRC("No license file, fall back to free license");
   }
   else if (DRXlicenseValidator::LICENSE_EXPIRED == LicenseResult) {
      COL_TRC("License has expired, fall back to free liecense");
   }
}