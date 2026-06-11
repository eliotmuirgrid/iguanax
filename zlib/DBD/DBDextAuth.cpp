// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDextAuth
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Monday 19 September 2022 - 12:57PM
// ---------------------------------------------------------------------------
#include <WEB/WEBsession.h>

#include <USR/USRuser.h>

#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <DBD/DBDextAuth.h>
#include <DIR/DIRpath.h>

#include <FIL/FILutils.h>

#include <BLUA/BLUAmanager.h>
#include <BLUA/BLUAstart.h>
#include <BLUA/BLUAcall.h>

#include <CFG/CFGmap.h>

#include <COL/COLthreadPool.h>
#include <COL/COLclosure6.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void DBDextAuthLoginFail(COLclosure1<COLvar>* pCallback) {
   COL_FUNCTION(DBDextAuthLoginFail);
   COLvar Result;
   Result["authenticated"] = false;
   pCallback->runAndDelete(Result);
}

// The next 4 functions here are for creating a user directory for externally authenticated users if required
static void DBDrespondAfterDirCreated(COLvar Result, COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(DBDrespondAfterDirCreated);
   pCallback->runAndDelete(Result);
}

static void DBDrespondDirCreateError(COLclosure1<COLvar>* pCallback){
   COL_FUNCTION(DBDrespondDirCreateError);
   DBDextAuthLoginFail(pCallback);
}

static void DBDcreateExternalUserDirImpl(COLstring Username, COLvar Result, COLclosure1<COLvar>* pCallback, SCKloop* pLoop){
   COL_FUNCTION(DBDcreateExternalUserDirImpl);
   COLstring FileName = DIRconfigUserDir(Username);
   if (FILfileExists(FileName)){
      if(!FILisDirectory(FileName)){
         SDBIlogService(FileName + " already exists but is not a directory. Please delete this file and try again.", "#error #login");
         pLoop->post(COLnewClosure0(&DBDrespondDirCreateError, pCallback));
      } else {
         COL_TRC("User directory already exists");
         pLoop->post(COLnewClosure0(&DBDrespondAfterDirCreated, Result, pCallback));
      }
      return;
   }
   try {
      FILcreateDirectoryTreeToFile(FileName, FIL_USER_RWX | FIL_GROUP_RX | FIL_OTHER_RX);
      pLoop->post(COLnewClosure0(&DBDrespondAfterDirCreated, Result, pCallback));
   } catch (const COLerror& Error){
      SDBIlogService("Failed to create user directory " + FileName, "#error #login");
      pLoop->post(COLnewClosure0(&DBDrespondDirCreateError, pCallback));
   }
}

static void DBDcreateExternalUserDir(COLstring Username, COLvar Result, COLclosure1<COLvar>* pCallback, SCKloop* pLoop){
   COL_FUNCTION(DBDcreateExternalUserDir);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&DBDcreateExternalUserDirImpl, Username, Result, pCallback, pLoop));
}

static void DBDextAuthHandleLoginResult(COLvar Result, COLclosure1<COLvar>* pCallback, COLstring Username, SCKloop* pLoop) {
   COL_FUNCTION(DBDextAuthHandleLoginResult);
   if(!Result.isArray()) {
      SDBIlogService("The 'EAUTHlogin' result returned was not an array.", "#error #login");
      return DBDextAuthLoginFail(pCallback);
   }
   Result = Result[0];
   COL_TRC("EAUTHlogin returned " << Result);
   if (Result.exists("error")) {
      SDBIlogService("The 'EAUTHlogin' function in the external authentication component failed with error:\n" + Result["error"].str(), "#error #login");
      return DBDextAuthLoginFail(pCallback);
   }
   COL_TRC("EAUTHlogin returned without any errors");
   if (!Result.exists("authenticated")) {
      SDBIlogService("Failed to parse external authentication result returned by the 'EAUTHlogin' function. Missing boolean field 'authenticated'.", "#error #login");
      return DBDextAuthLoginFail(pCallback);
   }
   if (Result.exists("roles")) {
      // An empty Lua roles array is converted to an empty COLvar table
      // Need to make sure it's always COLvar array.
      Result["roles"].setArrayType();
   }
   if (Result["authenticated"]){
      COL_TRC("Successful login, create user directory if needed");
      DBDcreateExternalUserDir(Username, Result, pCallback, pLoop); // this will happen on a worker thread
   } else {
      COL_TRC("Unsuccessful login");
      pCallback->runAndDelete(Result);
   }
}

void DBDextAuthenticateUser(COLstring Username, COLstring Password, COLclosure1<COLvar>* pCallback, BLUAinstanceManager* pManager, CFGmap* pMap, SCKloop* pLoop) {
   COL_FUNCTION(DBDextAuthenticateUser);
   if ("admin" == Username) {
      COL_TRC("External authentication is disabled for the admin user.");
      return DBDextAuthLoginFail(pCallback);
   }
   COLstring Component = getenv(DBD_EXT_AUTH_COMPONENT_ENV_NAME);
   if (Component.is_null()) {
      COL_TRC("External authentication is disabled.");
      return DBDextAuthLoginFail(pCallback);
   }
   auto Config = pMap->ConfigMap.find(Component);
   if (pMap->ConfigMap.end() == Config) {
      SDBIlogService("The configured external authentication component '" + Component + "' does not exist.", "#warning #login");
      return DBDextAuthLoginFail(pCallback);
   }
   if (!BLUAinstanceRunning(*pManager, Component)) {
      COL_TRC("Starting the external authentication component " << Component);
      BLUAinstanceStart(pManager, &Config.value(), pLoop);
   }
   COLvar Inputs;
   Inputs["username"] = Username;
   Inputs["password"] = Password;
   COL_TRC("Calling EAUTHlogin with " << Inputs);
   COLclosure1<COLvar>* pDone = COLnewClosure1(&DBDextAuthHandleLoginResult, pCallback, Username, pLoop);
   // run the EAUTHlogin function in external auth component to authenticate the user
   BLUAcallbackCall(pManager->m_Instances[Component], "EAUTHlogin", Inputs, pDone);
}

// -----------------------------------------------
// Login functions above, refresh functions below
// -----------------------------------------------

static COLvar DBDextAuthGetCurrentExternalUserList(COLmap<COLstring, USRuser>* pUserMap) {
   COL_FUNCTION(DBDextAuthGetCurrentExternalUserList);
   COLvar ExternalUsers;
   ExternalUsers.setArrayType();
   for (auto i = pUserMap->begin(); i != pUserMap->end(); i++) {
      if (i.value().isExternalUser()) {
         ExternalUsers.push_back(i.key());
      }
   }
   return ExternalUsers;
}

static void DBDextAuthUpdateUserSessionMap(const COLvar& Result, COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap) {
   COL_FUNCTION(DBDextAuthUpdateUserSessionMap);
   auto i = pUserMap->begin();
   while (i != pUserMap->end()) {
      const COLstring& Username = i.key();
      if (!i.value().isExternalUser()) {
         COL_TRC("Skip local user " << Username);
         i++;
         continue;
      }
      if (Result.exists(Username)) {
         COL_TRC("Update tags for external user " << Username);
         i.value().setRoles(Result[Username]);
         i++;
         continue;
      }
      COL_TRC("Disable removed external user " << Username);
      WEBdeleteUserSession(pSessionMap, Username);
      i = pUserMap->erase(i);
      SDBIlogService("Removed session for external user '" + Username + "' because their Iguana access had been revoked remotely.", "#login");
   }
}

static void DBDextAuthHandleRefreshResult(COLvar Result, COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, COLclosure0* pDone) {
   COL_FUNCTION(DBDextAuthHandleRefreshResult);
   COL_TRC("EAUTHrefresh returned " << Result);
   if (Result.exists("error")) {
      SDBIlogService("The 'EAUTHrefresh' function in the external authentication component failed with error:\n" + Result["error"].str(), "#error #login");
      pDone->runAndDelete();
      return;
   }
   if (!Result.isArray() || Result.size() < 1){
      SDBIlogService("The 'EAUTHrefresh' function in the external authentication component did not return in the expected manner.", "#error #login");
      pDone->runAndDelete();
      return;
   }
   COLvar ResultData = Result[0];
   DBDextAuthUpdateUserSessionMap(ResultData, pUserMap, pSessionMap);
   pDone->runAndDelete();
}

static void DBDextAuthRefresh(BLUAinstanceManager* pManager, CFGmap* pComponentMap, COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop, COLclosure0* pDone) {
   COL_FUNCTION(DBDextAuthRefresh);
   COLstring Component = getenv(DBD_EXT_AUTH_COMPONENT_ENV_NAME);
   COL_VAR(Component);
   if (Component.is_null() || pComponentMap->ConfigMap.count(Component) == 0) {
      COL_TRC("External auth component is not configured or invalid.");
      pDone->runAndDelete();
      return;
   }
   COLvar ExternalUsers = DBDextAuthGetCurrentExternalUserList(pUserMap);
   if (ExternalUsers.size() == 0) {
      COL_TRC("No external users found. Skip refresh.");
      pDone->runAndDelete();
      return;
   }
   if (!BLUAinstanceRunning(*pManager, Component)) {
      COL_TRC("Starting the external authentication component " << Component);
      BLUAinstanceStart(pManager, &pComponentMap->ConfigMap[Component], pLoop);
   }
   COL_TRC("Calling EAUTHrefresh with " << ExternalUsers);
   COLclosure1<COLvar>* pCallback = COLnewClosure1(&DBDextAuthHandleRefreshResult, pUserMap, pSessionMap, pDone); // cleaned up by BLUAreportCallResult
   // run the EAUTHrefresh function in external auth component to check if ldap users are still valid
   BLUAcallbackCall(pManager->m_Instances[Component], "EAUTHrefresh", ExternalUsers, pCallback);
}

static int DBDextAuthGetPollInterval() {
   COL_FUNCTION(DBDextAuthGetPollInterval);
   const char* pValue = getenv(DBD_EXT_AUTH_POLL_TIME_ENV_NAME);
   int Interval = DBD_EXT_AUTH_DEFAULT_POLL_TIME_IN_SECONDS;
   if (pValue) { Interval = atoi(pValue); }
   if (Interval < 1) { Interval = DBD_EXT_AUTH_DEFAULT_POLL_TIME_IN_SECONDS; }
   return Interval;
}

void DBDextAuthSetupRefresh(BLUAinstanceManager* pManager, CFGmap* pComponentMap, COLmap<COLstring, USRuser>* pUserMap, COLmap<COLstring, WEBsession>* pSessionMap, SCKloop* pLoop, int* pTimerId) {
   COL_FUNCTION(DBDextAuthSetupRefresh);
   int PollTime = DBDextAuthGetPollInterval();
   COLclosure0* pDone = COLnewClosure0(&DBDextAuthSetupRefresh, pManager, pComponentMap, pUserMap, pSessionMap, pLoop, pTimerId);
   *pTimerId = pLoop->addTimer(COLnewClosure0(&DBDextAuthRefresh, pManager, pComponentMap, pUserMap, pSessionMap, pLoop, pDone), PollTime * 1000, false, "ExtAuthTimer");
   COL_TRC("Created ext auth refresh timer " << *pTimerId);
}

void DBDextAuthTearDownRefresh(SCKloop* pLoop, int& TimerId) {
   COL_FUNCTION(DBDextAuthTearDownRefresh);
   pLoop->cancelTimer(TimerId);
   COL_TRC("Cancelled ext auth refresh timer " << TimerId);
   TimerId = 0;
}
