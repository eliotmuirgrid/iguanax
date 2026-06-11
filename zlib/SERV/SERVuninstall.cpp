// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVuninstall
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Monday 21 August 2023 - 02:56PM
// ---------------------------------------------------------------------------
#include <SERV/SERVuninstall.h>
#include <SERV/SERVutils.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <stdlib.h>

#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifndef __APPLE__
bool SERVstopServerWithRetry(){
   COL_FUNCTION(SERVstopServerWithRetry);
   if(!SERVisSudo()) { return false; }
   bool Result, Running;
   COLstring Error;
   int Retries = 0;
   do {
      if (!SDMserverRunning(SDMserviceName(), &Running, &Error)) {
         COL_WRN(Error);
         return false;
      }
      if (!Running) { break; }
      Retries++;
      Result = SDMserverStop(SDMserviceName(), &Error);
      COLcout << "Service stop result  : " << Result << " " << Error << " (try " << Retries << ")" << newline;
      if (Retries > 20){
         COLcout << "Failed."  << newline;
         return false;
      }
      COLsleep(2500);
   } while (true);
   return true;
}
#endif

static bool SERVremoveServer(){
   COL_FUNCTION(SERVremoveServer);
   bool Result, Exists;
   COLstring Error;
   int Retries = 0;
   do {
      if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
         COL_WRN(Error);
         return false;
      }
      if (!Exists) { break; }
      Retries++;
      Result = SDMserverRemove(SDMserviceName(), &Error);
      COLcout << "Service remove result: " << Result << " " << Error << " (try " << Retries << ")" << newline;
      if (Retries > 10){
         COLcout << "Failed."  << newline;
         return false;
      }
      COLsleep(1000);
   } while (true);
   return true;
}

int SERVuninstall(){
   COL_FUNCTION(SERVuninstall);
   bool Exists;
   COLstring Error;
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)) {
      COLcout << Error << newline;
      return EXIT_SUCCESS;
   }
   if (Exists) {
      #ifndef __APPLE__
      if (!SERVstopServerWithRetry()){
         return EXIT_FAILURE;
      }
      #endif
      if (!SERVremoveServer()){
         return EXIT_FAILURE;
      }
   } else {
      COLcout << "Service was not installed, so no uninstall needed."  << newline;
   }
   return EXIT_SUCCESS;
}
