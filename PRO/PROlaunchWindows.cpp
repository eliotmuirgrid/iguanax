//------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PROlaunch
// 
// Description
//
// Launch a process
// 
// Author: Shuo Hou
// Date:   June 5 2021
//---------------------------------------------------------------------------
#include "PROlaunch.h"
#include <COL/COLwindows.h>

#include <COL/COLioCheck.h>
#include <COL/COLstring.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int PROlaunch(const COLstring& AppDir, const COLstring& FullCommand){
   COL_FUNCTION(PROlaunch);
   COLassertIfMainThread();
   COL_VAR(AppDir);
   COL_VAR(FullCommand);
   STARTUPINFO StartupInfo;
   PROCESS_INFORMATION ProcessInfo;
   ZeroMemory( &StartupInfo, sizeof(StartupInfo) );
   StartupInfo.cb = sizeof(StartupInfo);
   ZeroMemory( &ProcessInfo, sizeof(ProcessInfo) );
   // The first parameter of CreateProcess is set to NULL because we are taking
   // the command line to the first part of pOwner->CommandString
   // Notice that the library has taken the care to enclose the first part of the command string
   // within double quotations.
   COLstring Command = FullCommand;
   if (!CreateProcessA(NULL,// name of executable module  
                        (LPSTR) Command.c_str(), // command line string
                        NULL, // default process attributes
                        NULL, // default thread attributes
                        FALSE, // handle inheritance option
                        0,// creation flags
                        NULL, // new environment block
                        (LPSTR) AppDir.c_str(), // current directory name
                        &StartupInfo,      // startup information
                        &ProcessInfo// process information
      ))
   {
      COL_ERR("Failed to create process");
   }
   COL_VAR(ProcessInfo.dwProcessId);
   return ProcessInfo.dwProcessId;
}