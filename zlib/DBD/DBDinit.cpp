//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDinit
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Thu 25 Feb 2021
//---------------------------------------------------------------------------
#include <IWEBA/IWEBserver.h>
#include <BLUA/BLUAautoStart.h>
#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAsafe.h>
#include <COL/COLclosure6.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
#include <COL/COLmath.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <CRH/CRHcrash.h>
#include <CRH/CRHdetermineCrashLoop.h>
#include <CRH/CRHupload.h>
#include <DBD/DBDcrash.h>
#include <DBD/DBDdeleteComponent.h>
#include <DBD/DBDinit.h>
#include <DBD/DBDlog.h>
#include <DBD/DBDluaRegister.h>
#include <DBD/DBDmodeInitialization.h>
#include <DBD/DBDmodeStartup.h>
#include <DBD/DBDregisterNODmap.h>
#include <DBD/DBDsetup.h>
#include <DBD/DBDusage.h>
#include <DBD/DBDworld.h>
#include <DBDVER/DBDVERversion.h>
#include <DBG/DBGpdb.h>
#include <DELC/DELCdeleteComponent.h>
#include <DRX/DRXlicenseCheck.h>
#include <DRX/DRXlicenseValidator.h>
#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>
#include <GIT/GITnonInteractive.h>
#include <GITE/GITEbitbucket.h>
#include <GINS/GINSinitialize.h>
#include <GSV/GSVmap.h>
#include <ICOL/ICOLcomponent.h>
#include <ICOL/ICOLmap.h>
#include <INFO/INFOutils.h>
#include <INST/INSTupgradeRecord.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBload.h>
#include <LIG/LIGversion.h>
#include <LOG/LOGlog.h>
#include <LOGE/LOGEencryptionValidate.h>
#include <LUA/LUAutils.h>
#include <PASS/PASScreateSalt.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBcrypto.h>
#include <SDM/SDMlogEvent.h>
#include <SDM/SDMname.h>
#include <SEC/SECaccess.h>
#include <SSO/SSOinit.h>
#include <USR/USRpermissions.h>
#include <WEB/WEBcookie.h>
#include <WEB/WEBmainThread.h>
#include <WEB/WEBresponse.h>
#include <WEB/WEBserver.h>
#include <WEBI/WEBImimeLookup.h>
COL_LOG_MODULE;

static COLstring s_EncryptionKey;

static void DBDinitGlobals() {
   COL_FUNCTION(DBDinitGlobals);
   LIGspecifyIguanaVersion(DBD_VERSION_MAJOR, DBD_VERSION_MINOR, DBD_BUILD_NUMBER);
   COLdateTime::currentDate(); // see #13304
   COLmath::resetRandomGenerator();
}

static void DBDuploadCrashReport() {
   COL_FUNCTION(DBDuploadCrashReport);
   COLstring Filename, Message;
   int Code = CRHuploadCrashReport(&Filename);
   switch (Code) {
      case 0:     /* no new crash report to upload */
      case -99:   /* not on the latest version so skip upload */                                                         return;
      case 200:   Message = "Successfully uploaded crash report '" + Filename + "' to server.";                          break;
      default:    Message = "Failed to upload crash report '" + Filename + "' to server. Code " + COLintToString(Code);  break;
   }
   SDBIlogService(Message, "#startup");
   COLcout << Message << newline;
}

static void DBDclearScratchDir() {
   const COLstring ScratchDir = DIRscratchDir("");
   COLstring Err;
   if(FILfileExists(ScratchDir)) { FILremoveFullDirNewSafe(ScratchDir, &Err); }
}

static COLauto<COLclosure0> pStopDoneCallback;

static void DBDsimpleInit(DBDworld* pWorld, bool Quiet, const COLstring& LogEncryptionKey){
   COL_FUNCTION(DBDsimpleInit);
   COL_VAR(pWorld->FreshInstall);
   DBDclearScratchDir();
   SECinit(pWorld->FreshInstall);
   ICOLinit();
   ICOLcomponentInit();
   DBDcreate(pWorld, LogEncryptionKey);
   pStopDoneCallback = COLnewClosure0(&DRXonExpiredCallbackDone);
   DRXsetupOnExpiredCallback(pWorld->pLicenseValidator, COLnewClosure0(&BLUAinstanceStopAllWithoutCleanup, pStopDoneCallback.get(), pWorld->pBluaManager));
   DRXloadLicense(pWorld->pLicenseValidator); // TODO - do we want to log the current license info here?
   DRXsetLicenseCheckCallback(pWorld->pLoop, pWorld->pLicenseValidator);
   DRXsetupLicenseTimers(pWorld->pLicenseValidator, pWorld->pLoop);
   USRload(pWorld->pUserMap);
   SSOinit();
   DBDstartUsageCollection(pWorld->pLoop);
   CRHsetUpCrashLog();
   DBDuploadCrashReport();
   COL_VAR(pWorld);
   DBDcrashTest();
   GITsetNonInteractive();
   GSVmapInit();
   DBDinitGlobals();
   INFOsetFdLimitWrapper();
   WEBresponseSetServer(pWorld->pWebServer);
   WEBImimeLookupInitMap();
   DBDregisterNODmap();
   COLwebSetResponseCallback(COLnewClosure2(&WEBresponseSend)); // this one writes string data to socket (or posts then writes the data)
   PASScreateSalt();  // We need to load the password file.
   LUAsetOnLuaStateCreated(COLnewClosure2(&DBDluaRegister, pWorld));
   DELCsetOnComponentDelete(COLnewClosure3(&DBDdeleteComponent, pWorld->pLoop, pWorld->pLogQueue, pWorld->pComponentMap));
   LUAsetOnLuaStateIdle(COLnewClosure1(&DBDluaIdle));
   LOGlogSetCallback(COLnewClosure2(&DBDlogMessage, pWorld->pLoop, pWorld->pBluaManager));
   PRMsetRoleCallbacks(pWorld->pLoop, pWorld->pUserMap, pWorld->pRoleMap, pWorld->pComponentMap);
   COLstring Ver = DBD_STRING_VERSION; // IX-4287 trying to fix a crash
   SDBIlogService(COL_T("Iguana ") + Ver, "#startup");
   SDBIlogService("Configuration: " + FILworkingDir(), "#startup");
   if(Quiet) {
      COL_TRC("Suppressing output");
      return;
   }
   COL_TRC("Debug symbols: " << DBGprogramDatabase());
   COLcout << "Instance: " << FILworkingDir() << newline;
   COL_CRASH_HERE_IF_ENV_VAR_IS_SET();
}

void DBDlogUnlockInit(DBDworld* pWorld){
   COL_FUNCTION(DBDlogUnlockInit);
   DBDcreateCore(pWorld); // TODO - we could craft a similar function that only sets up what's needed along with a matching destroy
   CRHsetUpCrashLog();
   // DBDuploadCrashReport();
   USRload(pWorld->pUserMap);
   PRMsetRoleCallbacks(pWorld->pLoop, pWorld->pUserMap, pWorld->pRoleMap, pWorld->pComponentMap);
   DBDinitGlobals();
   WEBresponseSetServer(pWorld->pWebServer);
   WEBImimeLookupInitMap();
   COLwebSetResponseCallback(COLnewClosure2(&WEBresponseSend)); // this one writes string data to socket (or posts then writes the data)
}

void DBDrunLoopAndShutdown(DBDworld* pWorld){
   COL_FUNCTION(DBDrunLoopAndShutdown);
   // TODO - might need to consider start up mode before log system is initialized.... later consideration.
   COLsetMainThread();  // Always warn if we have blocking IO operations on the main thread.
   WEBsetMainThread();  // So that WEBresponseSend knows which thread is the main thread.
   COL_TRC("About to start loop.");
   pWorld->pLoop->start();
   COL_TRC("Ended Loop.start()");
   if (pWorld->InitError){
      COLcout << pWorld->ErrorLog << newline;
   }
   SDBIlogService("Shutdown complete.", "#shutdown");
   // last chance to SDBIlogService() before DBDdestroy()
   DBDdestroy(pWorld);
   if (pWorld->InitError){
      COL_TRC("Error occurred so throwing it.");
      throw COLerror(pWorld->ErrorLog, 0);
   }
   COLcout << "Shutdown completed." << newline;
}

// TODO update Windows installer to let user setup log encryption
void DBDinitInstall(DBDworld* pWorld, bool Quiet) {
   COL_FUNCTION(DBDinitInstall);
   DBDsimpleInit(pWorld, true, "");
   COLstring Error;
   IWEBload(pWorld->pWebConfig, &Error);
   WEBsetCookieHost(pWorld->pWebConfig->uniqueId());
   pWorld->pWebConfig->setWebPort(COLintToString(COLmath::randomNumber(50000)+1025));
   COLcout << "Running as installer (to exit use Control+C)" << newline;
   if (!IWEBserverInit(*pWorld->pWebServer, *pWorld->pWebConfig, Quiet, &Error)) {
      COLcout << Error << newline;
      COLcout << "Will try another port." << newline;
      pWorld->pWebConfig->setWebPort(COLintToString(COLmath::randomNumber(50000)+1025));
      if (!IWEBserverInit(*pWorld->pWebServer, *pWorld->pWebConfig, Quiet, &Error)){
         DBDdestroy(pWorld);
         COL_ERROR_STREAM_PLAIN("Unable to start:" << newline << Error, 0);
      }
   }
   DBDmodeStartup(pWorld);
   DBDrunLoopAndShutdown(pWorld);
}

static void DBDlogShutdown(SCKloop* pLoop) {
   pLoop->threadPool()->shutdown(COLthreadPool::shutdown_immediate);
   pLoop->shutdown();
   COLcout << "Shutdown event loop" << newline;
}

static void DBDcleanupMiniServer(DBDworld* pWorld, COLstring EncKey) {
   COL_FUNCTION(DBDcleanupMiniServer);
   // clean up mini server
   s_EncryptionKey = EncKey; // TODO - is there no better way?
   pWorld->pWebServer->shutdown();
   pWorld->pLoop->threadPool()->shutdown(COLthreadPool::shutdown_immediate);
   pWorld->pLoop->shutdown();
}

static void DBDlogEncryptionStartup(const COLwebRequest& Request, DBDworld* pWorld) {
   COL_FUNCTION(DBDlogEncryptionStartup);
   const COLvar& Data = Request.Data;
   COL_VAR(Data);
   const COLstring& EncKey = COLvarRequiredString(Data, "key");
   COL_TRC("Checking provided key");
   if (!LOGEcheckEncryptionKeySize(EncKey)){
      COL_TRC("Encryption key too short.");
      return COLrespondError(Request.Address, "Encryption key must be 32 bytes in length.");
   }
   COLstring Error;
   COL_TRC("Trying to decrypt validation string");
   COLclearMainThread(); // This prevents FIL from thowing an error complaining about main thread io
   if (!LOGEstartupEncryptionVerifyKey(EncKey, &Error)){
      return COLrespondError(Request.Address, Error);
   }
   COL_TRC("Decryption successful, continue to normal mode.");
   COLrespondSuccess(Request.Address);
   pWorld->pLoop->post(COLnewClosure0(&DBDcleanupMiniServer, pWorld, EncKey));
}

#include <SDM/SDMshutdown.h>
static void DBDmodeLogUnlock(DBDworld* pWorld) {
   COL_FUNCTION(DBDmodeLogUnlock);
   pWorld->Mode = DBD_MODE::LOG_UNLOCK;
   pWorld->pWebServer->m_RequireAuthentication = false;  // No login required for startup mode.
   WEBserverClearHandlers(pWorld->pWebServer);
   pWorld->pWebServer->m_MainThreadHandlers.add("/log/encryption/startup", COLnewClosure1(&DBDlogEncryptionStartup, pWorld));
   pWorld->pWebServer->m_MainThreadHandlers.add("/mode",                   COLnewClosure1(&DBDmodeWeb,              pWorld));
   SDMshutdownSet(COLnewClosure0(&DBDlogShutdown, pWorld->pLoop));
}

static bool DBDuserEnteredLogEncryptionkey() {
   COL_FUNCTION(DBDuserEnteredLogEncryptionkey);
   return !s_EncryptionKey.is_null();
}

void DBDinitNormalWithEncryption(DBDworld* pWorld){
   COL_FUNCTION(DBDinitNormalWithEncryption);
   DBDlogUnlockInit(pWorld); // todo - remove the usr and prm dependencies
   COLstring Error;
   if (!IWEBload(pWorld->pWebConfig, &Error)){
      DBDdestroy(pWorld);
      COL_ERROR_STREAM_PLAIN("Unable to start:" << newline << Error, 0);
   }
   if (!IWEBserverInit(*pWorld->pWebServer, *pWorld->pWebConfig, true, &Error)) {
      DBDdestroy(pWorld);
      COL_ERROR_STREAM_PLAIN("Unable to start:" << newline << Error, 0);
   }
   COLcout << "Log encryption mode." << newline;   // TODO remove after done
   DBDmodeLogUnlock(pWorld);
   DBDrunLoopAndShutdown(pWorld);
   if (DBDuserEnteredLogEncryptionkey()) {
      COLcout << "Now transition to normal mode." << newline;
      DBDinitNormal(pWorld, s_EncryptionKey);
   }
}

static void DBDdisableAutoStart(){
   COL_FUNCTION(DBDdisableAutoStart);
   if (CRHdetermineCrashLoop()){
      COL_TRC("In a crash loop - disable autostarting of components.");
      COLstring ErrorMessage("Iguana has detected a crash loop and has started in safe mode. No components have been autostarted.");
      SDMlogEvent(SDMserviceName(), ErrorMessage);
      SDBIlogService(ErrorMessage, "#error #startup #autostart");
      BLUAsetSafeMode(true);
      return;
   }
   COL_TRC("Not in crash loop - do nothing.");
}

void DBDinitNormal(DBDworld* pWorld, const COLstring& LogEncryptionKey) {
   COL_FUNCTION(DBDinitNormal);
   GINSinitialize();
   SDMlogEvent(SDMserviceName(), "Iguana service started. For troubleshooting please read:\n" + FILworkingDir() + "ServiceErrorLog.txt");
   DBDsimpleInit(pWorld, false, LogEncryptionKey);
   INSThandleUpgradeRecord();
   COLstring Error;
   if (!IWEBload(pWorld->pWebConfig, &Error)){
      DBDdestroy(pWorld);
      COL_ERROR_STREAM_PLAIN("Unable to start:" << newline << Error, 0);
   }
   WEBsetCookieHost(pWorld->pWebConfig->uniqueId());
   if (!IWEBserverInit(*pWorld->pWebServer, *pWorld->pWebConfig, false, &Error) ){
      DBDdestroy(pWorld);
      COL_ERROR_STREAM_PLAIN("Unable to start:" << newline << Error, 0);
   }
   DBDdisableAutoStart();
   COLcout << "Normal production mode." << newline;
   DBDmodeInitialization(pWorld);
   DBDrunLoopAndShutdown(pWorld);
}
