//------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module:  PROkill
// 
// Description
//
// kill a process
// 
// Author: Shuo Hou
// Date:   June 5 2021
//---------------------------------------------------------------------------
#include "PROkill.h"

#include <COL/COLwindows.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

// void SendSIGINT( HANDLE hProcess )
// {
//    DWORD pid = GetProcessId(hProcess);
//    FreeConsole();
//    if (AttachConsole(pid))
//    {
//          // Disable Ctrl-C handling for our program
//          SetConsoleCtrlHandler(NULL, true);

//          GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0); // SIGINT

//          //Re-enable Ctrl-C handling or any subsequently started
//          //programs will inherit the disabled state.
//          SetConsoleCtrlHandler(NULL, false);

//          WaitForSingleObject(hProcess, 10000);
//    }
// }

void PROkill(int PID){
   COL_FUNCTION(PROkill);
   HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, TRUE, PID);;
   TerminateProcess(hProcess,1);
   CloseHandle(hProcess);
   // SendSIGINT(hProcess);
}