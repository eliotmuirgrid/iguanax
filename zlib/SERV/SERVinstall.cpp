// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SERVinstall
//
// Description:
//
// Implementation
//
// Author: Tyler Brown
// Date:   Monday 21 August 2023 - 02:53PM
// ---------------------------------------------------------------------------
#include <SERV/SERVinstall.h>
#include <SERV/SERVaction.h>
#include <SERV/SERVuninstall.h>
#include <SERV/SERVutils.h>
#include <SERV/SERVselinuxCheck.h>

#include <DBD/DBDpreStart.h>

#include <INST/INSTinstall.h>
#include <IWEB/IWEBiguanaUrl.h>

#include <SDM/SDMmanage.h>
#include <SDM/SDMname.h>

#include <FIL/FILutils.h>

#include <POP/POPcolor.h>

#include <PRO/PROexecute.h>

#include <SCK/SCKloop.h>

#include <USR/USRload.h>
#include <USR/USRpermissions.h>
#include <USR/USRgenerateAdminIfEmpty.h>

#include <WEB/WEBresponse.h>

#include <COL/COLutils.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static int SERVinstallError(const COLstring& Msg) { COLcout << Msg << newline; return EXIT_FAILURE;}

static bool SERVblockRootInstallLinux(const COLstring& Username) {
   COL_FUNCTION(SERVblockRootInstallLinux);
   if(Username == "root") {
      COLcout << "Cannot install as root user -- please specify another" << newline;
      return true;
   }
   return false;
}

void SERVinstallService(const COLstring& Username, const COLstring& Password) {
   COL_FUNCTION(SERVinstallReplace);
   COLstring Error;
   bool Result;
   COLstring Exe = FILexecutableDir() + FILexecutableFileName();
   COL_VAR(Exe);
   Result = INSTinstall(Exe, Username, Password, &Error);
   if (Result){
      COLcout << "Successfully installed service: " << SDMserviceName() << " under user " << Username << newline;
   } else {
      COLcout << "Failed to install service. " << Error << newline;
   }
}

bool SERVinstallReplace() {
   COL_FUNCTION(SERVinstallReplace);
   COLstring Error;
   bool Exists;
   if (!SDMserverExists(SDMserviceName(), &Exists, &Error)){
      COLcout << "Could not see if server exists:" << newline << Error << newline;
      return false;
   }
   if (Exists && EXIT_SUCCESS != SERVuninstall()){
      COL_TRC("UNINSTALL FAILED");
      return false;
   }
   return true;
}

int SERVinstallLinux(const COLstring& Username, const COLstring& Password) {
   COL_FUNCTION(SERVinstallLinux);
   if(SERVblockRootInstallLinux(Username))     { return EXIT_FAILURE; }
   COLcout << "Removing the existing IguanaX Service..." << newline;
   if(!SERVinstallReplace())                   { return EXIT_FAILURE; }
   COLcout << "Creating a default user if necessary..." << newline;
   USRgenerateAdminIfEmpty(Username);
   COLcout << "Installing IguanaX Service under " << Username << newline;
   SERVinstallService(Username, Password);
   COLcout << "Checking SELinux settings..." << newline;
   SERVselinuxCheck();
   COLcout << "Starting IguanaX Service..." << newline;
   if(SERVaction("start", Username, "") == EXIT_FAILURE)  { return SERVinstallError("Error starting service"); }
   COLsleep(3000);
   if(SERVaction("status", Username, "") == EXIT_FAILURE) { return SERVinstallError("Error getting service status"); }
   return EXIT_SUCCESS;
}

int SERVinstallWindows(const COLstring& Username, const COLstring& Password) {
   COL_FUNCTION(SERVinstallWindows);
   if(!SERVinstallReplace()) { return EXIT_FAILURE; }
   SERVinstallService(Username, Password);
   return EXIT_SUCCESS;
}

int SERVinstall(const COLstring& Username, const COLstring& Password){
   COL_FUNCTION(SERVinstall);
#ifdef __linux__
   return SERVinstallLinux(Username, Password);
#else
   return SERVinstallWindows(Username, Password);
#endif
}
