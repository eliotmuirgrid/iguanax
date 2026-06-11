//---------------------------------------------------------------------------
// Copyright (C) 1997-2015 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMserver
//
// Description:
//
// Implementation
//
// Authors: Eliot Muir, Akshay Ganeshen
// Date:    Wed Mar  4 14:10:23 EST 2015
//---------------------------------------------------------------------------
#include <csignal>
#include <cerrno>

#pragma hdrstop

#include "SDMserver.h"
#include <SDM/SDMshutdown.h>
#include <SDM/SDMutils.h>

#include <PRO/PROexecute.h>
#include <PRO/PROutils.h>

#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool s_SDBrunningAsService = false;

void SDMsetRunningAsService(bool IsService){
   COL_FUNCTION(SDMsetRunningAsService);
   COL_VAR(IsService);
   s_SDBrunningAsService = IsService;
}

bool SDMrunningAsService(){
   return s_SDBrunningAsService;
}

static void signalHandler(int Signal) {
   COL_FUNCTION(signalHandler);
   COL_VAR(Signal);
   SDMshutdown(); // if pSDMserverShutdown in SDMshutdown is not NULL, run it.
}

class SDMserverPrivate {
public:
   SDMserverPrivate(const COLstring& ServiceName, SDMserver* pOwner) : m_ServiceName(ServiceName), m_pOwner(pOwner) {
      COL_METHOD(SDMserverPrivate::SDMServerPrivate);
   }
   ~SDMserverPrivate() {
      // NOTE : This implicitly waits on the signal thread, if it was started
      COL_METHOD(SDMserverPrivate::~SDMserverPrivate);
   }

   // TODO: This is kinda sketchy, will revisit when we work on graceful shutdown.
   void setOnKill(){
      COL_METHOD(SDMserverPrivate::setOnKill);
      void (*pInterrupt) (int) = &signalHandler;
      COL_TRC("Setting Handler For SIGTERM");
      if (::signal(SIGTERM, pInterrupt) == SIG_ERR) { COL_THROW_STRERROR_PLAIN("Cannot set sigterm handler"); }
      COL_TRC("Setting Handler For SIGINT");
      if (::signal(SIGINT, pInterrupt) == SIG_ERR) { COL_THROW_STRERROR_PLAIN("Cannot set sigint handler"); }
   }

   COLstring m_ServiceName;
   SDMserver* m_pOwner;
};

SDMserver::SDMserver(const COLstring& ServiceName) : pMember(NULL){
   COL_METHOD(SDMserver::SDMserver);
   pMember = new SDMserverPrivate(ServiceName, this);
}
   
SDMserver::~SDMserver(){
   COL_METHOD(SDMserver::~SDMserver);
   delete pMember;
}
 
void SDMserver::runServerAsNormalProcess() {
   COL_METHOD(SDMserver::runServerAsNormalProcess);
   pMember->setOnKill();
   try {
      COL_TRC("About to do run.");
      // Typically, the onRun will be an event loop, and the other thread will trigger onStopRequest, causing this to return
      m_pOnRun->run();
   } catch (const COLerror& Error) {
      COLcout << Error.description() << newline;
   }
}

static bool checkRunningAsService(){
   COL_FUNCTION(checkRunningAsService);
#ifdef __linux__
   // If running from a terminal environment, TERM should be set
   const char * Terminal = getenv("TERM");
   if (Terminal == NULL){
      COL_TRC("TERM not set");
      return true;
   }
   return false;
#endif
#ifdef __APPLE__
   return SDMisUnderLaunchD();
#endif
}

// We invoke this method to start the server as a daemon 
bool SDMserver::runServer() {
   COL_METHOD(SDMserver::runServer);
   if (checkRunningAsService()) {
      COL_TRC("Setting running as service");
      SDMsetRunningAsService(true); 
   }
   runServerAsNormalProcess();
   return true;
}

const COLstring& SDMserver::name() const { return pMember->m_ServiceName; }
