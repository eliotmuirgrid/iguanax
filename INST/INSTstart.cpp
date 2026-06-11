// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTstart
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski  
// Date:   Thursday 15 June 2023 - 11:05AM
// ---------------------------------------------------------------------------
#include <INST/INSTstart.h>
#include <INST/INSTwaitForEnter.h>

#ifdef _WIN32
#   include <windows.h>
#endif

#include <IWEB/IWEBiguanaUrl.h>

#include <FIL/FILutils.h>

#include <SDM/SDMserver.h>

#include <PRO/PROexecute.h>
#include <PRO/PROutils.h>
#include <PRO/PROweb.h>

#include <COL/COLsimulateOs.h>
#include <COL/COLutils.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#include <cstdio>
#endif

void INSTstartup(const IWEBconfig& Config) {
   COL_FUNCTION(INSTstartup);
   if (!PROweb(IWEBiguanaUrl(Config))){
      COLcout << "Please open:" << newline << IWEBiguanaUrl(Config) << newline;
   }
}

#ifdef _WIN32
void INSTrunAsAdmin(const COLstring& Args){
   COL_FUNCTION(INSTrunAsAdmin);
   SHELLEXECUTEINFO execInfo;
   memset(&execInfo, 0, sizeof(SHELLEXECUTEINFO));
   execInfo.cbSize = sizeof(SHELLEXECUTEINFO);
   execInfo.lpVerb = "runas";  // Request elevation
   COLstring FullPath = FILexecutableDir() + FILexecutableFileName();
   // Specify the path to the executable you want to run with admin privileges
   execInfo.lpFile = FullPath.c_str();
   execInfo.lpParameters = Args.c_str();
   COL_VAR(execInfo.lpFile);  
   // Set the show mode to be normal (can be changed to SW_HIDE for a hidden window)
   execInfo.nShow = SW_NORMAL;
    // Launch the process with administrator privileges
   if (!ShellExecuteEx(&execInfo)) {
      COLcout << "Failed to run the program with administrator privileges." << newline;
      return;
   }
}
#endif // _WIN32

COLstring INSTcommandLine(int argc, const char** argv){
   COL_FUNCTION(INSTcommandLine);
   COLarray<COLstring> Args;
   for (int i = 1; i < argc; i++){
      Args.push_back(argv[i]);
   }
   return PRObuildCommandLine(Args);
}

bool INSTisAdmin(){
   COL_FUNCTION(INSTisAdmin);
   bool Result;
   #ifdef __APPLE__
   return true;   // NOT TRUE BUT NOT NEEDED
   #endif  
   #ifdef __linux__
   Result = geteuid() == 0;
   #endif
   #ifdef _WIN32
   SID_IDENTIFIER_AUTHORITY NtAuthority = SECURITY_NT_AUTHORITY;
   BOOL IsAdmin = FALSE;
   PSID AdministratorsGroup;
   if(AllocateAndInitializeSid(&NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                               DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0,
                               &AdministratorsGroup)) {
     CheckTokenMembership(NULL, AdministratorsGroup, &IsAdmin);
     FreeSid(AdministratorsGroup);
   }
   Result = (IsAdmin == TRUE);
   COL_VAR(IsAdmin);
   #endif
   COL_VAR(Result);
   return Result;
}


bool INSTrerunAsAdmin(int argc, const char** argv){
   COL_FUNCTION(INSTrerunAsAdmin);
   COLstring Args = INSTcommandLine(argc, argv);
   COL_VAR(Args);
   bool IsAdmin = INSTisAdmin();
#ifdef _WIN32
   // Check if we're already running as administrator
   if(!IsAdmin) { // If we're not running as administrator, relaunch the program as administrator
      COLcout << "To install IguanaX we need administrative privileges." << newline;
      INSTrunAsAdmin(Args);
      return true;
   }
#endif
#ifdef __linux__
   if (!IsAdmin){
      COLcout << "To install IguanaX you need to have sudo root privileges.  Please rerun with sudo." << newline;
      return true; 
   }
#endif
   return false;
}

void INSTpauseIfNotWindowsService(){
   COL_FUNCTION(INSTpauseIfNotWindowsService);
   if (COLisWindows()) {
      COL_TRC("We have windows");
      if (!SDMrunningAsService()){
         COL_TRC("Running as command line so we need to pause before we finish.");
         COLcout << "Press enter to finish and exit or wait 30 seconds." << newline << flush;
	 INSTwaitForEnter(30);
      } else {
         COL_TRC("Running as service so we just exit.");
      }
   }
}
