// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: LOGEencryptionSet
//
// Description:
//
// Implementation
//
// Author: Vismay Shah
// Date:   Tuesday 29 October 2024 - 01:36PM
// ---------------------------------------------------------------------------
#include <BLUA/BLUAmanager.h>
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <DIR/DIRpath.h>
#include <FIL/FILutils.h>
#include <GINS/GINSaddAndCommit.h>
#include <LOGE/LOGEencrypt.h>
#include <LOGE/LOGEencryptionSet.h>
#include <LOGE/LOGEencryptionValidate.h>
#include <PRM/PRMcallbacks.h>
#include <PRO/PROlaunch.h>
#include <SCK/SCKworker.h>
#include <SDB/SDBdir.h>
#include <SDM/SDMrestart.h>
COL_LOG_MODULE;

static void LOGErestartService() {
   COL_FUNCTION(LOGErestartService);
#ifdef _WIN32
   COLstring Cmd = FILexecutableFileName() + " --service restart";
   COL_VAR2(FILexecutableDir(), Cmd);
   int ProcessId = PROlaunch(FILexecutableDir(), Cmd);
#elif __APPLE__
   SDMrequestRestart();
#else
   SDMrequestRestart();
#endif
}

static void LOGEencryptSetDone(COLvar Result, COLaddress Address) {
   COL_FUNCTION(LOGEencryptSetDone);
   if (Result["success"].asBool()){
      COLrespondSuccess(Address);
      LOGErestartService();
   } else {
      COLrespondError(Address, Result["error"].asString());
   }
}

static void LOGEencyptSetPrepare(const COLvar& In, COLvar* pOut){
   COL_FUNCTION(LOGEencyptSetPrepare);
   const bool UseEncryption = In["encrypt"];
   COLstring EncryptionKey;
   if (UseEncryption) { EncryptionKey = COLvarRequiredString(In, "key"); }
   COLvar LogConfig;
   COLstring Error;
   if (!SDBreadLogConfig(&LogConfig, &Error)){
      COL_TRC("File probably doesn't exist, this is ok");
      Error.clear();
   }

   LogConfig["encryption"] = UseEncryption;
   LogConfig["initial_setup"] = true;
   if (UseEncryption && !LOGEwriteEncryptionValidation(EncryptionKey, &LogConfig)) {
      COL_TRC("Failed to encrypt validation phrase");
      (*pOut)["error"] = "Failed to encrypt validation data.";
      (*pOut)["success"] = false;
      return;
   }
   (*pOut)["success"] = FILsave(DIRfileConfigLog(), LogConfig.json(1), &Error);
   if(!(*pOut)["success"]) { (*pOut)["error"] = Error; }
   COLvar Components;
   COLvar Data;
   Data["user"]        = In["user"];
   Data["message"]     = UseEncryption ? "Enabled log encryption." : "Disabled log encryption.";
   Data["allow_empty"] = true;
   Data["settings"]    = true;
	GINSaddAndCommitImpl(Data, &Error);
}

static bool LOGEencryptCheckKeys(const COLstring& Key, const COLstring& ConfirmKey, COLstring* pError){
   COL_FUNCTION(LOGEencryptCheckKeys);
   if (!Key.equals(ConfirmKey))          { *pError = "Encryption keys provided do not match";                   return false; }
   if (!LOGEcheckEncryptionKeySize(Key)) { *pError = "Encryption keys provided must be exactly 32 bytes long."; return false; }
   return true;
}

static void LOGEencryptSetImpl(const COLstring& User,const bool UseEncryption, const COLstring& Key, const COLstring& ConfirmKey, bool* pUseEncryption, const COLaddress& Address, SCKloop* pLoop){
   COL_FUNCTION(LOGEencryptSetImpl);
   COLstring Error;
   if (UseEncryption && !LOGEencryptCheckKeys(Key, ConfirmKey, &Error)){ return COLrespondError(Address, Error); }
   COL_TRC("Encryption keys ok.");
   *pUseEncryption = UseEncryption;
   COL_TRC("Set encryption to " << UseEncryption);
   COLvar Data;
   Data["encrypt"] = UseEncryption;
   if (UseEncryption){
      Data["key"] = Key;
   }
   Data["user"] = User;
   // Now fire off to worker thread to save to disk
   SCKinvokeWorker(pLoop, LOGEencyptSetPrepare, Data, COLnewClosure1(&LOGEencryptSetDone, Address));
}

// /log/encryption/set
void LOGEencryptSet(const COLwebRequest& Request, bool* pUseEncryption, BLUAinstanceManager* pManager, SCKloop* pLoop){
   COL_FUNCTION(LOGEencryptSet);
   COL_VAR(Request.Data);
   const bool UseEncryption    = COLvarRequiredBool(Request.Data,   "encrypt");
   const COLstring& Key        = COLvarRequiredString(Request.Data, "key");
   const COLstring& KeyConfirm = COLvarRequiredString(Request.Data, "key_confirm");
   COLstring Error;
   if (!PRMcheckRoleAccess(Request.FromTranslator, Request.User, "", ROLid::EDIT_LOG_ENCRYPTION, &Error)){
      return COLrespondError(Request.Address, PRMsetError(ROLid::EDIT_LOG_ENCRYPTION));
   }
   if (!BLUAallComponentsStopped(*pManager)) {
      return COLrespondError(Request.Address, "All components must be stopped prior to changing the log encryption setting.");
   }
   if (!(*pUseEncryption) && !UseEncryption){
      COL_TRC("Encryption already disabled.");
      COLvar Result;
      Result = COL_T("Encryption is already disabled");
      return COLrespondSuccess(Request.Address, Result);  // if log encryption is disabled its ok to try to disable again.
   }
   if (*pUseEncryption == UseEncryption) { // This also makes it so you cannot change encryption keys without turning encryption off and on.
      COL_TRC("Not switching modes. Ignore encryption keys and return.");
      return COLrespondError(Request.Address, "Encryption is already enabled. To switch encryption keys you must first disable encryption, then re-enable it with your new key.");
   }
   LOGEencryptSetImpl(Request.User, UseEncryption, Key, KeyConfirm, pUseEncryption, Request.Address, pLoop);
}