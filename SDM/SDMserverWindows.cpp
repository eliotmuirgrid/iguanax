//---------------------------------------------------------------------------
// Copyright (C) 1997-2013 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMserver
//
// Author: Eliot Muir
//
// Description:
//
// Implementation
//---------------------------------------------------------------------------

#include <SDM/SDMserver.h>
#include <SDM/SDMshutdown.h>
#include <SDM/SDMlogEvent.h>

#include <COL/COLwindows.h>

#include <cstdlib>
#include <csignal>

#include <COL/COLutils.h>
#include <COL/COLevent.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static const int SERVICE_CONTROL_USER = 128;

bool s_SDBrunningAsService = false;

void SDMsetRunningAsService(bool IsService){
   COL_FUNCTION(SDMsetRunningAsService);
   COL_VAR(IsService);
   s_SDBrunningAsService = IsService;
}

bool SDMrunningAsService(){
   return s_SDBrunningAsService;
}

class SDMserverPrivate {
 public:
   SDMserverPrivate(const COLstring& iServiceName, SDMserver* ipParent) : ServiceName(iServiceName) {
      COLASSERT(pServer== nullptr); // there can be only one singleton!
      pParent = ipParent;
      pServer = this;
   }

   ~SDMserverPrivate() = default;

   SERVICE_STATUS_HANDLE ServiceStatusHandle{};

   static SDMserverPrivate* pServer;

   SDMserver* pParent;
   COLevent  StoppedEvent; // Event object that is for signaling that the server has stopped
   COLstring ServiceName;
};

SDMserverPrivate* SDMserverPrivate::pServer = nullptr;

const char* SDMdescribeServiceStatus(DWORD Status){
   switch(Status){
      case SERVICE_STOPPED:          return "SERVICE_STOPPED";
      case SERVICE_START_PENDING:    return "SERVICE_START_PENDING";
      case SERVICE_STOP_PENDING:     return "SERVICE_STOP_PENDING";
      case SERVICE_RUNNING:          return "SERVICE_RUNNING";
      case SERVICE_CONTINUE_PENDING: return "SERVICE_CONTINUE_PENDING";
      case SERVICE_PAUSE_PENDING:    return "SERVICE_PAUSE_PENDING";
      case SERVICE_PAUSED:           return "SERVICE_PAUSED";
      default:                       return "UNKNOWN CODE";
   }
}

// TODO - change to return bool?
void SDMsetServerStatus(SERVICE_STATUS_HANDLE ServiceHandle, DWORD NewStatus) {
   COL_FUNCTION(SDMsetServerStatus);
   COL_VAR3(ServiceHandle, NewStatus, SDMdescribeServiceStatus(NewStatus));
   SERVICE_STATUS CurrentStatus;
   CurrentStatus.dwServiceType             = SERVICE_WIN32; // Service type
   CurrentStatus.dwCurrentState            = NewStatus; // Current status
   CurrentStatus.dwControlsAccepted        = SERVICE_ACCEPT_STOP;  // Accepted control codes
   CurrentStatus.dwWin32ExitCode           = 0; // Error code
   CurrentStatus.dwServiceSpecificExitCode = 0; // Specific error code - when starting or stopping
   CurrentStatus.dwCheckPoint              = 0; // Value that the service increments to show status in operations such as start,stop...
   CurrentStatus.dwWaitHint                = 0; // Estimate amount of time while completing operation before next call
   if (0 == ::SetServiceStatus(ServiceHandle, &CurrentStatus)){
      long ErrNo = COLlastError();
      COLstring ErrorMsg = COLerrorString(ErrNo);
      COL_ERR("SetServiceStatus failed while trying to set status: " << ErrNo << ":" << ErrorMsg);
   }
}

void SDMnotifyStartProblem(SERVICE_STATUS_HANDLE ServiceHandle, int ErrorCode) {
   COL_FUNCTION(SDMnotifyStartProblem);
   COL_VAR2(ServiceHandle, ErrorCode);
   SERVICE_STATUS CurrentStatus;
   CurrentStatus.dwServiceType             = SERVICE_WIN32; // Service type
   CurrentStatus.dwCurrentState            = SERVICE_STOPPED; // Current status
   CurrentStatus.dwControlsAccepted        = 0;  // Accepted control codes
   CurrentStatus.dwWin32ExitCode           = ERROR_SERVICE_SPECIFIC_ERROR; // Error code
   CurrentStatus.dwServiceSpecificExitCode = ErrorCode; // Specific error code - when starting or stopping
   CurrentStatus.dwCheckPoint              = 0; // Value that the service increments to show status in operations such as start,stop...
   CurrentStatus.dwWaitHint                = 0; // Estimate amount of time while completing operation before next call
   if (0 == ::SetServiceStatus(ServiceHandle, &CurrentStatus)){
      long ErrNo = COLlastError();
      COLstring ErrorMsg = COLerrorString(ErrNo);
      COL_ERR("SetServiceStatus failed while trying to notify start problem: " << ErrorMsg << ":" << ErrNo);
   } else {
      COL_TRC("Notified start problem.");
   }
}

void __cdecl SDMcontrolCHandler(int SignalValue) {
   COL_FUNCTION(SDMcontrolCHandler);
   COLASSERT(SDMserverPrivate::pServer != nullptr);
   COLASSERT(SDMserverPrivate::pServer->pParent != nullptr);
   COLcout << "Ctrl+C detected, sending stop signal." << newline << flush;   
   try{
      SDMshutdown();
   }catch (COLerror& Error){
      COLcout << Error << newline << flush;
      COL_ERR(Error);
   }
}

void SDMinitControlCSignalHandler() {
   COL_FUNCTION(SDMinitControlCSignalHandler);
   if (SIG_ERR == ::signal(SIGINT, &SDMcontrolCHandler)){
      COL_ERR("Trying to set the SIGINT (control+C) handler returned SIG_ERR");
   } else {
      COL_TRC("Successfully registered control+C handler");
   }
}

void SDMstopService(SDMserverPrivate* pServer) {
   COL_FUNCTION(SDMstopService);
   COL_TRC("About to signal stopped event.");
   pServer->StoppedEvent.signal();
   COL_TRC("Signaled stopped event.");
}

const char* SDMoperationCodeAsString(DWORD controlCode) {
   switch (controlCode) {
      case SERVICE_CONTROL_STOP:                  return "SERVICE_CONTROL_STOP";
      case SERVICE_CONTROL_PAUSE:                 return "SERVICE_CONTROL_PAUSE";
      case SERVICE_CONTROL_CONTINUE:              return "SERVICE_CONTROL_CONTINUE";
      case SERVICE_CONTROL_INTERROGATE:           return "SERVICE_CONTROL_INTERROGATE";
      case SERVICE_CONTROL_SHUTDOWN:              return "SERVICE_CONTROL_SHUTDOWN";
      case SERVICE_CONTROL_PARAMCHANGE:           return "SERVICE_CONTROL_PARAMCHANGE";
      case SERVICE_CONTROL_NETBINDADD:            return "SERVICE_CONTROL_NETBINDADD";
      case SERVICE_CONTROL_NETBINDREMOVE:         return "SERVICE_CONTROL_NETBINDREMOVE";
      case SERVICE_CONTROL_NETBINDENABLE:         return "SERVICE_CONTROL_NETBINDENABLE";
      case SERVICE_CONTROL_NETBINDDISABLE:        return "SERVICE_CONTROL_NETBINDDISABLE";
      case SERVICE_CONTROL_DEVICEEVENT:           return "SERVICE_CONTROL_DEVICEEVENT";
      case SERVICE_CONTROL_HARDWAREPROFILECHANGE: return "SERVICE_CONTROL_HARDWAREPROFILECHANGE";
      case SERVICE_CONTROL_POWEREVENT:            return "SERVICE_CONTROL_POWEREVENT";
      case SERVICE_CONTROL_SESSIONCHANGE:         return "SERVICE_CONTROL_SESSIONCHANGE";
      case SERVICE_CONTROL_PRESHUTDOWN:           return "SERVICE_CONTROL_PRESHUTDOWN";
      case SERVICE_CONTROL_TIMECHANGE:            return "SERVICE_CONTROL_TIMECHANGE";
      case SERVICE_CONTROL_TRIGGEREVENT:          return "SERVICE_CONTROL_TRIGGEREVENT";
      case SERVICE_CONTROL_LOWRESOURCES:          return "SERVICE_CONTROL_LOWRESOURCES";
      case SERVICE_CONTROL_SYSTEMLOWRESOURCES:    return "SERVICE_CONTROL_SYSTEMLOWRESOURCES";
      default:                                    return "UNKNOWN";
   }
}

void WINAPI SDMserviceHandler(DWORD OperationCode) {
   COL_FUNCTION(SDMserverHandler);
   COL_VAR2(OperationCode, SDMoperationCodeAsString(OperationCode));
   // Get a pointer to the object
   COLASSERT(SDMserverPrivate::pServer != nullptr);
   try{
      switch (OperationCode){
         case SERVICE_CONTROL_STOP:
            COL_TRC("Set stop pending.");
            SDMsetServerStatus(SDMserverPrivate::pServer->ServiceStatusHandle, SERVICE_STOP_PENDING);
            COL_TRC("Sending stop signal");
            SDMshutdown(); // server would be running until this is signaled
            SDMserverPrivate::pServer->StoppedEvent.wait();
            COL_TRC("Service stopped.");
            //SDMsetServerStatus(SDMserverPrivate::pServer, SERVICE_STOPPED);
            COL_TRC("Done");
            break;
         default:
            if (OperationCode >= SERVICE_CONTROL_USER){
               COL_ERR("The service library does not support user control events.");
            } else {
               COL_ERR("The service library received an unknown control event: " << OperationCode);
            }
      }
   }catch (const COLerror& Error){
      // How we actually exercise this code is another question.
      COL_ERR(Error << " occured. Will try and log it in event log.");
      SDMlogEvent(SDMserverPrivate::pServer->pParent->name(), Error.description());
   }
}

void WINAPI SDMserviceMain(DWORD argc, LPTSTR* argv) {
   COL_FUNCTION(SDMserviceMain);
   COL_VAR2(argc, argv[0]);
   COL_TRC("Setting running as a service.");
   SDMsetRunningAsService(true);
   try {
      COLASSERT(SDMserverPrivate::pServer != nullptr);
      // Handler function is registered. i.e. If a control request comes in, it will be handled by the handler function.
      COL_TRC("Register service control handler.");
      SDMserverPrivate::pServer->ServiceStatusHandle =
         ::RegisterServiceCtrlHandler(SDMserverPrivate::pServer->ServiceName.c_str(), // address of name of service
                                      &SDMserviceHandler); // address of handler function

      if(0 == SDMserverPrivate::pServer->ServiceStatusHandle) {
         long ErrorCode = COLlastError();
         COL_ERR("::RegisterServiceCtrlHandler failed: " << COLerrorString(ErrorCode) << ":" << ErrorCode);
         return;
      }
      SDMsetServerStatus(SDMserverPrivate::pServer->ServiceStatusHandle, SERVICE_RUNNING);
      COL_TRC("Calling onRun");
      SDMserverPrivate::pServer->pParent->m_pOnRun->run(); // NOTE: onRun() will keep executing until it gets a signal to stop
      COL_TRC("Exiting run loop of server.");
      SDMsetServerStatus(SDMserverPrivate::pServer->ServiceStatusHandle, SERVICE_STOPPED);
   } catch(COLerror& Error) {
      COL_ERR(Error);
      SDMnotifyStartProblem(SDMserverPrivate::pServer->ServiceStatusHandle, Error.code());
      SDMlogErrorEvent(SDMserverPrivate::pServer->ServiceName, Error.description());
   }
   COL_TRC("About to call stop service");
   SDMstopService(SDMserverPrivate::pServer);
}

SDMserver::SDMserver(const COLstring& ServiceName) : pMember(nullptr) {
   COL_METHOD(SDMserver::SDMserver);
   COL_VAR(ServiceName);
   pMember = new SDMserverPrivate(ServiceName, this);
}

SDMserver::~SDMserver() {
   COL_METHOD(SDMserver::~SDMserver);
   try {
      COLASSERT(pMember != nullptr);
      delete pMember;
   } catch(const COLerror& Error) {
      COL_ERR(Error);
   }
}

const COLstring& SDMserver::name() const { return pMember->ServiceName; }

bool SDMserver::runServer(){
   COL_METHOD(SDMserver::runServer);
   SERVICE_TABLE_ENTRY ServiceTable[] = {
      {pMember->ServiceName.get_buffer() , SDMserviceMain},
      {nullptr, nullptr}
   };
   if (0 == ::StartServiceCtrlDispatcher(ServiceTable)){
      DWORD ErrorCode = ::GetLastError();
      COL_TRC("StartServiceCtrlDispatcher failed with code " << ErrorCode << " " << COLerrorString(ErrorCode));
      // Dispatcher does not succeed however error is due to the fact that service is run from command line
      if (ERROR_FAILED_SERVICE_CONTROLLER_CONNECT == ErrorCode){
         COL_TRC("Service run from command line -- starting as normal process");
         runServerAsNormalProcess();
         return true;
      }else{
         return false;
      }
   }
   return true;
}

void SDMserver::runServerAsNormalProcess() {
   COL_METHOD(SDMserver::runServerAsNormalProcess);
   // COLcout << "Running service as command line application. (ctrl+C to exit)" << newline;  // Removing this, can output this in a more granular function
   // Specific signal and its handler are specified. The code moves on after that.
   SDMinitControlCSignalHandler();
   try {
      m_pOnRun->run();
   } catch(const COLerror& Error) {
      COLcout << Error.description() << newline;
      SDMlogErrorEvent(name(), Error.description());
   } catch(...) {
      COLcout << "Exception thrown in onRun - server exiting." << newline;
      throw;
   }
   COLcout << "Exiting server." << newline << flush;
}