// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMmanageInstallWindows
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Wednesday 12 July 2023 - 03:18PM
// ---------------------------------------------------------------------------
#include <SDM/SDMmanage.h>
#include <SDM/SDMpermissions.h>
#include <SDM/SDMhandleWindows.h>

#include <REG/REGregistryWindows.h>

#include <COL/COLerror.h>
#include <COL/COLwindows.h>

// This has to go after COLwindows.h
#include <SDM/SDMmanageWindows.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static bool SDMcheckUsernamePassword(const COLstring& User, const COLstring& Password, COLstring* pError) {
   COL_FUNCTION(SDMcheckUsernamePassword);
   COL_VAR2(User, Password);
   if (User.is_null()) {
      return true;
   }
   if (!SDMverifyPassword(User, Password)) {
      *pError = "Username " + User + " and password not accepted.";
      return false;
   }
   if (!SDMgiveUserRightToRunAsService(User, pError)) {
      return false;
   }
   return true;
}


static bool SDMcreateService(SC_HANDLE Manager, const COLstring& Name, const COLstring& DisplayName, bool AutoStart, const COLstring& Executable, COLstring User, const COLstring& Password, SC_HANDLE* pService, COLstring* pError) {
   COL_FUNCTION(SDMcreateService);
   COL_VAR4(Name, DisplayName, AutoStart, Executable);
   COL_VAR(User);
   LPCSTR pUserLogin = NULL;
   LPCSTR pPasswordLogin = NULL;
   DWORD ServiceType = SERVICE_WIN32_OWN_PROCESS;
   if (User.is_null()) {
      ServiceType |= SERVICE_INTERACTIVE_PROCESS;  // I don't think this will ever become useful. Interactive service is a pretty awful experience.
   } else {
      User.replace("/", "\\");
      if (User.find("\\") == npos) {
         COL_TRC("No domain so adding .\\ in front.");
         User = COL_T(".\\") + User;
      }
      pUserLogin = User.c_str();
      if (!Password.is_null()) {
         pPasswordLogin = Password.c_str();
      }
   }
   COL_VAR3(pUserLogin, pPasswordLogin, ServiceType);
   *pService = ::CreateService(Manager,            // handle to service control manager
                              Name.c_str(),        // pointer to name of service to start
                              DisplayName.c_str(), // pointer to display name
                              SERVICE_ALL_ACCESS,  // access rights
                              ServiceType,         // type of service
                              AutoStart ? SERVICE_AUTO_START : SERVICE_DEMAND_START, // when to start service - start condition
                              SERVICE_ERROR_NORMAL,// severity if service fails to start
                              Executable.c_str(),  // pointer to name of binary file
                              NULL,                // pointer to name of load ordering group
                              NULL,                // pointer to variable to get tag identifier
                              NULL,                // pointer to array of dependency names
                              pUserLogin,          // pointer to account name of service
                              pPasswordLogin);     // pointer to password for service account
   if (NULL == *pService) {
      *pError = "While creating service: " + COLlastErrorAsString();
      COL_VAR(*pError);
      return false;
   }
   return true;
}

void SDMenableEventLogging(const COLstring& Name) {
   COL_FUNCTION(SDMenableEventLogging);
   REGregistry Registry(HKEY_LOCAL_MACHINE);
   COLstring SubKey("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\");
   SubKey += Name;
   COL_TRC("Open key HKEY_LOCAL_MACHINE " << SubKey);
   Registry.openKey(SubKey, true);
   char szPath[MAX_PATH];
   if (!GetModuleFileName(NULL, szPath, MAX_PATH)) {
     COL_ERR("GetModuleFileName failed");
   }
   Registry.writeString("EventMessageFile", szPath);
   Registry.writeInteger("TypesSupported", EVENTLOG_ERROR_TYPE | EVENTLOG_WARNING_TYPE | EVENTLOG_INFORMATION_TYPE);
   COL_TRC("Write EventMessageFile and TypesSupported entries.");
}

static bool SDMconfigureServiceRecovery(SC_HANDLE Service, COLstring* pError) {
   COL_FUNCTION(SDMconfigureServiceRecovery);
   // restart the service after 3 minutes after it fails
   SC_ACTION Action;
   Action.Type = SC_ACTION_RESTART;
   Action.Delay = 0 * 60 * 1000;  // 0 minutes recovery - gulp!
   SERVICE_FAILURE_ACTIONSA Actions;
   Actions.dwResetPeriod = 0;
   Actions.lpRebootMsg = NULL;
   Actions.lpCommand = NULL;
   Actions.cActions = 1;
   Actions.lpsaActions = &Action;
   if (0 == ChangeServiceConfig2(Service, SERVICE_CONFIG_FAILURE_ACTIONS, &Actions)) {
      *pError = COLlastErrorAsString() + " while configuring service recovery";
      return false;
   }
   return true;
}

static bool SDMconfigureServiceDescription(SC_HANDLE Service, const COLstring& Description, COLstring* pError) {
   COL_FUNCTION(SDMconfigureServiceDescription);
   SERVICE_DESCRIPTION ServiceDescription;
   ServiceDescription.lpDescription = (char*) Description.c_str();
   if (0 == ChangeServiceConfig2(Service, SERVICE_CONFIG_DESCRIPTION, &ServiceDescription)) {
      *pError = COLlastErrorAsString() + " while setting service description";
      return false;
   }
   return true;
}

bool SDMserverInstall(const COLstring& Name, const COLstring& DisplayName, const COLstring& Description,
     const COLstring& Executable, const COLstring& User, const COLstring& Password, bool AutoStart, COLstring* pError){
   COL_FUNCTION(SDMserverInstall);
   COL_VAR4(Name, DisplayName, Description, Executable);
   COL_VAR3(User, Password, AutoStart);
   bool Exists;
   if (!SDMserverExists(Name, &Exists, pError)) { return false; }
   if (Exists) { *pError = "Service " + Name + " already exists!"; return false;}
   if (!SDMcheckUsernamePassword(User, Password, pError)) { return false; }
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CREATE_SERVICE, &Manager.handle, pError)) { return false; }
   if (!SDMcreateService(Manager.handle, Name, DisplayName, AutoStart, Executable, User, Password, &Service.handle, pError)) { return false;}
   SDMenableEventLogging(Name);
   if (!SDMconfigureServiceRecovery(Service.handle, pError)) {
      COL_TRC("Failed to configure service recovery: " << *pError);
      return false;
   }
   if (!Description.is_null() && !SDMconfigureServiceDescription(Service.handle, Description, pError)) {
      COL_TRC("Failed to configure service description: " << *pError);
      return false;
   }
  // if (AutoStart && !::StartService(Service.handle, 0, NULL)) {
  //    *pError = "While auto-starting service: " + COLlastErrorAsString();
  //    COL_TRC("Failed to auto-start service: " << *pError);
  //    return false;
  // }
   return true;
}