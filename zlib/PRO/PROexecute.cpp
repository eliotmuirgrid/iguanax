//---------------------------------------------------------------------------
// Copyright (C) 1997-2019 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecute
//
// Description:
//
// Implementation
//
// Author: Nasron Cheong - Ported by Eliot to PRO library
// Date:   Monday, March 3rd, 2008 @ 02:34:25 PM
//---------------------------------------------------------------------------
#include <PRO/PROexecute.h>
#include <PRO/PROexecuteTrace.h>

#include <COL/COLauto.h>
#include <COL/COLutils.h>
#include <COL/COLioCheck.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool PROexecuteOS(
   const COLstring& Dir,
   const COLstring& FullCommand,
   COLsink*         pOutSink,
   COLsink*         pErrSink,
   int              OutputTimeout,
   int*             pExitCode,
   int              Try,
   bool             ExitOnOutput
);

//returns false if timed out
bool PROexecute(
   const COLstring& Dir,
   const COLstring& Command,
   COLsink* pOutSink,
   COLsink* pErrSink,
   int OutputTimeout,
   int* pExitCode,
   bool ExitOnOutput
) {
   COL_FUNCTION(PROexecute);
   COLassertIfMainThread();
   return PROexecuteOS(Dir, Command, pOutSink, pErrSink, OutputTimeout, pExitCode, 0, ExitOnOutput);
}


// Convenience wrapper to make it easier to execute a process
bool PROexecute(
   const COLstring& WorkingDir,    // The directory to run the command from
   const COLstring& FullCommand,   // The full command to run
   COLstring*       pOutString,    // Standard output is put into this string
   COLstring*       pErrString,    // Standard error  is put to this string.
   int              OutputTimeout, // Time in SECONDS to wait for output from process before killing it. -1 for no timeout.
   int*             pExitCode,     // Exit code is stored in this integer
   bool             ExitOnOutput
) {
   COL_FUNCTION(PROexecuteString);
   COLsinkString OutSink(pOutString, false);
   COLsinkString ErrSink(pErrString, false);
   COL_VAR2(WorkingDir, FullCommand);
   bool Result = PROexecute(WorkingDir, FullCommand, &OutSink, &ErrSink, OutputTimeout, pExitCode, ExitOnOutput);
   COL_VAR4(Result, *pExitCode, OutSink.string(), ErrSink.string());
   return Result;
}

int PROexecuteString(const COLstring& Dir,
                     const COLstring& Command,
                     COLstring* pOut,
                     COLstring* pErr,
                     int TimeoutInSeconds,
                     bool ExitOnOutput
) {
   COL_FUNCTION(PROexecuteString);
   COL_VAR2(Dir, Command);
   COLsinkString OutSink(pOut, false);
   COLsinkString ErrSink(pErr, false);
   int ExitCode;
   PROexecuteTrace(Command);
   const bool Success = PROexecute(Dir, Command, &OutSink, &ErrSink, TimeoutInSeconds, &ExitCode, ExitOnOutput);
   if(!Success && ExitCode == 0) { ExitCode = PRO_EXECUTE_FAILED; }
   COL_VAR3(ExitCode, *pOut, *pErr);
   return ExitCode;
}
