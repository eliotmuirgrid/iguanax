// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVaction
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Thursday 22 September 2022 - 03:41PM
// ---------------------------------------------------------------------------

#ifndef _WIN32
#include <unistd.h>
#include <pwd.h>
#endif

#include <IWEBA/IWEBserver.h>
#include <COL/COLerror.h>
#include <COL/COLlog.h>
#include <FIL/FILutils.h>
#include <IP/IPmine.h>
#include <IWEB/IWEBconfig.h>
#include <IWEB/IWEBiguanaUrl.h>
#include <IWEB/IWEBload.h>
#include <LINK/LINKconstants.h>
#include <POP/POPcolor.h>
#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>
#include <SERV/SERVaction.h>
#include <SERV/SERVinstall.h>
#include <SERV/SERVrestart.h>
#include <SERV/SERVuninstall.h>
#include <SERV/SERVutils.h>
#include <USR/USRpermissions.h>
#include <stdlib.h>
COL_LOG_MODULE;

bool SERVverifyUser(COLstring* pUsername, const COLstring& Password, COLstring* pErr) {
   COL_FUNCTION(SERVverifyUser);
   COLstring& Username = *pUsername;
#ifndef _WIN32
   if(Username == "root") {
      COLcout << "Installing service under root user" << newline;
      Username.clear();
      return true;
   }
   if(Username.is_null()) {
      Username = ::getlogin(); // since we have to run command as sudo this is necessary over FILcurrentUser()
      COLcout << "Defaulting user to " << Username << newline;
   }
   struct passwd const* pw = ::getpwnam(Username.c_str());
   if(pw == nullptr) {
      *pErr = "Username does not exist. Cancelling install";
      return false;
   }
   return true;
#else // windows
   // IF USERNAME IS NULL THEN WE ARE USING THE LOCALSYSTEM account - do not change this logic
   if(!Password.is_null() && Username.is_null()) {
      Username = FILcurrentUser();
      COLcout << "Defaulting user to " << Username << newline;
   }
   return true; // We'll verify the windows password later.
#endif
}

int SERVstatus() {
   COL_FUNCTION(SERVstatus);
   bool Exists;
   COLstring Error;
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
      COLcout << Error << newline;
      return EXIT_SUCCESS;
   }
   if (!Exists) {
      COLcout << "Not installed" << newline;
      return EXIT_SUCCESS;
   }
   bool Running;
   if (!SDMserverRunning(SDMserviceName(), &Running, &Error)) {
      COLcout << Error << newline;
      return EXIT_SUCCESS;
   }
   if (Running) {
      COLcout << "Iguana service is RUNNING" << newline;
      #ifdef __linux__
      COLcout << "How do I configure my firewall so I can connect to Iguana?" << newline;
      COLcout << "See " << POPpop(LINKservAction) << newline;
      #endif
      COLcout << "Web interface on:" << newline;
      COLcout << IWEBiguanaUrl() << "      (from the server itself)" << newline << flush;
      COLstring MyIp;
      bool Success = IPmine(&MyIp);
      if (Success){
      COLcout << IWEBiguanaUrl(MyIp) <<  " (assuming firewalls allow access)" << newline;
      }

   } else {
      COLcout << "Not running" << newline;
   }
   return EXIT_SUCCESS;
}

#ifndef __APPLE__

int SERVstart(){
   COL_FUNCTION(SERVstart);
   COLstring Error;
   COL_VAR(SERVisSudo());
   if(!SERVisSudo()) { return EXIT_FAILURE; }
   bool Result = SDMserverStart(SDMserviceName(), &Error);
   if (Result){ return EXIT_SUCCESS; }
   COLcout << Error << newline;
   return EXIT_FAILURE;
}

int SERVstop(){
   COL_FUNCTION(SERVstop);
   COLstring Error;
   if(!SERVisSudo()) { return EXIT_FAILURE; }
   bool Result = SDMserverStop(SDMserviceName(), &Error);
   if (Result){
      COLcout << "Signalled Iguana to stop" << newline;
      return EXIT_SUCCESS;
   } else {
      COLcout << Error << newline;
      return EXIT_FAILURE;
   }
}
#endif

void SERVactionStartOutput() {
   COL_FUNCTION(SERVactionStartOutput);
   COLcout << "Signalled Iguana to start" << newline;
   COLcout << "Logging to: " << FILworkingDir() + "ServiceErrorLog.txt" << newline;
   IWEBoutputListeningAndHelp();
}

void SERVactionInstallOutput() {
   COL_FUNCTION(SERVactionInstallOutput);
#ifndef __APPLE__
   if (!USRcheckNeedsAdminUser(COLcout, true)){
      COLcout << "To start the service use:" << newline;
      COLcout << "   " << POPiguanaCommandLine(true) << " --service start" << newline;
   }
#else
   IWEBoutputListeningAndHelp();
#endif
}

void SERVactionOutput(const COLstring& Action) {
   COL_FUNCTION(SERVactionOutput);
   if(Action == "install") { return SERVactionInstallOutput(); }
   if(Action == "start")   { return SERVactionStartOutput(); }
}

int SERVaction(const COLstring& ActionIn, const COLstring& Username, const COLstring& Password) {
   COL_FUNCTION(SERVaction);
   COLstring Action = ActionIn;
   Action.toLowerCase();
   try {
      if (Action == "uninstall")      { return SERVuninstall(); }
      #ifndef __APPLE__
      else if (Action == "start")     { return SERVstart();     }
      else if (Action == "stop")      { return SERVstop();      }
      else if (Action == "restart")   { return SERVrestart();   }
      #endif
      else if (Action == "install")   { return SERVinstall(Username, Password);}
      else if (Action == "status")    { return SERVstatus();    }
      else {
         #ifdef __APPLE__
         COLcout << "Invalid service option argument. Must be one of 'install', 'uninstall' or 'status'." << newline;
         #else
         COLcout << "Invalid service option argument. Must be one of 'start', 'stop', 'restart', 'install', 'uninstall' or 'status'." << newline;
         #endif
         return EXIT_FAILURE;
      }
      return EXIT_SUCCESS;
   }
   catch (const COLerror& Error) {
#ifdef _WIN32
      int PERMISSION_ERROR = 5;
#else    // linux and macOS
      int PERMISSION_ERROR = 13;
#endif
      if (Error.code() == PERMISSION_ERROR) {
         COLcout << "Failed to execute service command. Please make sure you have admin privilege." << newline;
      } else {
         COLcout << Error.description() << newline;
      }
      return EXIT_FAILURE;
   }
}
