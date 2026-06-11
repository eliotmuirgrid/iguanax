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

#include <PRO/PROexecute.h>
#include <FIL/FILutils.h>

#include <COL/COLerror.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef __linux__

// since linux always uses commandline seems easier to just have this helper
static void SDMserverCommand(const COLstring& Command, COLstring* pOut, COLstring* pErr, int* pExitCode) {
   COL_FUNCTION(SDMserverCommand);
   COL_VAR(Command);
   *pExitCode = PROexecuteString(".", Command, pOut, pErr);
   COL_VAR(*pErr);
   pErr->replace("Failed to open /dev/tty: No such device or address\n",""); // IX-1995 we run in a non interactive shell
   COL_VAR(*pErr);
   COL_VAR(*pExitCode);
}

bool SDMserverExists(const COLstring& Name, bool* pExists, COLstring* pError) {
   COL_FUNCTION(SDMserverExists);
   COLstring Command = "systemctl list-unit-files --type=service";
   COLstring StdErr, StdOut;
   int ExitCode;
   SDMserverCommand(Command, &StdOut, &StdErr, &ExitCode);
   *pExists = StdOut.find(Name) != npos;
   COL_VAR(*pExists);
   return true;
}

bool SDMserverRunning(const COLstring& Name, bool* pRunning, COLstring* pError) {
   COL_FUNCTION(SDMserverRunning);
   COLstring Command = "systemctl is-active " + Name + ".service";
   COLstring StdErr, StdOut;
   int ExitCode;
   SDMserverCommand(Command, &StdOut, &StdErr, &ExitCode);
   COL_VAR(ExitCode);
   // 0 == service running, 3 == inactive, non-zero = error
   *pRunning = ExitCode == 0;
   return true;
}

bool SDMserverStart(const COLstring& Name, COLstring* pError){
   COL_FUNCTION(SDMserverStart);
   COLstring Command = "sudo systemctl start " + Name + ".service";
   COLstring StdOut;
   int ExitCode;
   SDMserverCommand(Command, &StdOut, pError, &ExitCode);
   COL_VAR3(ExitCode, StdOut, *pError);
   return ExitCode == 0;
}

bool SDMserverStop(const COLstring& Name, COLstring* pError){
   COL_FUNCTION(SDMserverStop);
   COLstring Command = "sudo systemctl stop " + Name + ".service";
   COLstring StdOut;
   int ExitCode;
   SDMserverCommand(Command, &StdOut, pError, &ExitCode);
   COL_VAR3(ExitCode, StdOut, *pError);
   return ExitCode == 0;
}

#endif // __linux__
