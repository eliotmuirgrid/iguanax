// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMmanage
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
#include <COL/COLlog.h>
COL_LOG_MODULE;


bool SDMopenManager(DWORD Permission, SC_HANDLE* pManager, COLstring* pError) {
   COL_FUNCTION(SDMopenManager);
   COL_VAR(*pManager);
   *pManager = ::OpenSCManager(NULL, NULL, Permission);
   if (NULL == *pManager) {
      *pError = "While opening Service Control Manager: " + COLlastErrorAsString();
      return false;
   }
   COL_VAR(*pManager);
   return true;
}

bool SDMopenService(SC_HANDLE Manager, const COLstring& Name, DWORD Permission, SC_HANDLE* pService, COLstring* pError) {
   COL_FUNCTION(SDMopenService);
   COL_VAR(*pService);
   *pService = ::OpenService(Manager, Name.c_str(), Permission);
   if (NULL == *pService) {
      *pError = "While opening service: " + COLlastErrorAsString();
      return false;
   }
   COL_VAR(*pService);
   return true;
}


bool SDMserverExists(const COLstring& Name, bool* pExists,  COLstring* pError) {
   COL_FUNCTION(SDMserverExists);
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   *pExists = SDMopenService(Manager.handle, Name, SERVICE_QUERY_CONFIG, &Service.handle, pError);
   return true;
}

bool SDMserverRunning(const COLstring& Name, bool* pRunning,  COLstring* pError) {
   COL_FUNCTION(SDMserverRunning);
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, SERVICE_QUERY_STATUS, &Service.handle, pError)) { return false; }
   SERVICE_STATUS Status;
   bool Success = ::QueryServiceStatus(Service.handle, &Status);
   COL_VAR(Success);
   if (Success) {
      *pRunning = SERVICE_RUNNING == Status.dwCurrentState;
   } else {
      *pError = "While querying service status: " + COLlastErrorAsString();
   }
   return Success;
}

bool SDMserverStart(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(SDMserverStart);
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, SERVICE_START, &Service.handle, pError)) { return false; }
   bool Success = ::StartService(Service.handle, 0, NULL);
   if (!Success) {
      *pError = "While starting service: " + COLlastErrorAsString();
   }
   return Success;
}

bool SDMserverStop(const COLstring& Name, COLstring* pError){
   COL_FUNCTION(SDMserverStop);
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, SERVICE_STOP, &Service.handle, pError)) { return false; }
   SERVICE_STATUS Status;
   bool Success = ::ControlService(Service.handle, SERVICE_CONTROL_STOP, &Status);
   if (!Success) {
      *pError = "While stopping service: " + COLlastErrorAsString();
   }
   return Success;
}

static void SDMdisableEventLogging(const COLstring& Name) {
   COL_FUNCTION(SDMdisableEventLogging);
   REGregistry Registry(HKEY_LOCAL_MACHINE);
   Registry.openKey("SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\", true);
   Registry.deleteSubKey(Name);
   COL_TRC("Deleted sub key SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\" << Name);
}

bool SDMserverRemove(const COLstring& Name, COLstring* pError) {
   COL_FUNCTION(SDMserverRemove);
   SDMhandle Manager;
   SDMhandle Service;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, DELETE, &Service.handle, pError)) { return false; }
   bool Success = ::DeleteService(Service.handle);
   if (Success) {
      SDMdisableEventLogging(Name);
   } else {
      *pError = "While deleting service: " + COLlastErrorAsString();
   }
   return Success;
}

void SDMcreateQueryInfoBuffer(SC_HANDLE ServiceHandle, void** ppBuffer, int* pSize){
   COL_FUNCTION(SDMcreateQueryInfoBuffer);
   COL_TRC("Calling QueryServiceConfig to find the size of the buffer.");
   // A trick to get size of buffer
   // Note that where size should be, a 0 is input
   // This will cause the query to return the size needed
   // in sizeOfBuffer parameter which can the be used to populate the buffer
   DWORD SizeOfBuffer;
   ::QueryServiceConfig(ServiceHandle, NULL, 0,&SizeOfBuffer);
   *pSize = SizeOfBuffer;
   COL_VAR4(SizeOfBuffer, *pSize, GetLastError(), ERROR_INSUFFICIENT_BUFFER);
   *ppBuffer = new char[SizeOfBuffer];
   ::memset(*ppBuffer, 0, SizeOfBuffer);
   COL_TRC("Created and cleared buffer.");
}

bool SDMpopulateQueryInfoBuffer(void* ServiceHandle, void** ppBuffer, int* pSize, COLstring* pError){
   COL_FUNCTION(SDMpopulateQueryInfoBuffer);
   SDMcreateQueryInfoBuffer((SC_HANDLE)ServiceHandle, ppBuffer, pSize);
   COLASSERT(*ppBuffer);
   DWORD CountOfByteNeeded=0;
   COL_TRC("About to call QueryServiceConfig");
   if (0 == ::QueryServiceConfig((SC_HANDLE)ServiceHandle,(QUERY_SERVICE_CONFIG*)*ppBuffer, *pSize, &CountOfByteNeeded)){
      *pError = COLlastErrorAsString() + " while querying service";
      COL_ERR(*pError);
      return false;
   }
   COL_VAR2(*pSize, CountOfByteNeeded);
   return true;
}

bool SDMserverExe(const COLstring& Name, COLstring* pExe, COLstring* pError){
   COL_FUNCTION(SDMserverExe);
   SDMhandle Manager;
   SDMhandle Service;
   void* pBuffer;
   int Size;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, SERVICE_QUERY_CONFIG, &Service.handle, pError)) { return false; }
   if (!SDMpopulateQueryInfoBuffer(Service.handle, &pBuffer, &Size, pError)) { return false; }
   *pExe = ((QUERY_SERVICE_CONFIG*)pBuffer)->lpBinaryPathName;
   COL_VAR(*pExe);
   delete pBuffer;
   return true;
}

bool SDMserverUser(const COLstring& Name, COLstring* pUser, COLstring* pError){
   COL_FUNCTION(SDMserverUser);
   SDMhandle Manager;
   SDMhandle Service;
   void* pBuffer;
   int Size;
   if (!SDMopenManager(SC_MANAGER_CONNECT, &Manager.handle, pError)) { return false; }
   if (!SDMopenService(Manager.handle, Name, SERVICE_QUERY_CONFIG, &Service.handle, pError)) { return false; }
   if (!SDMpopulateQueryInfoBuffer(Service.handle, &pBuffer, &Size, pError)) { return false; }
   *pUser = ((QUERY_SERVICE_CONFIG*)pBuffer)->lpServiceStartName;
   COL_VAR(*pUser);
   delete pBuffer;
   return true;
}
