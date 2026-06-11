#ifndef __PRO_EXECUTE_H__
#define __PRO_EXECUTE_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecute
//
// Description:
//
// Function to invoke a process and get the exit code, and standard out and err into variables.
//
// Author: Eliot Muir
// Date:   Tue  9 Jul 2019 10:28:50 EDT
//---------------------------------------------------------------------------

class COLsink;
class COLstring;

// returns false if timed out or error
// In Windows, if the the command spawns a new process, and you DO NOT want the
// new process to inherit handles such as sockets and ports, you MUST use this
// version of PROexecute. The other PROexecute allows the spawned process to
// inherit sockets, which could lead to web server restart issues.
enum PRO_ERROR_CODES{
   PRO_BAD_EXIT         = -10,
   PRO_TIMED_OUT        = -20,
   PRO_BAD_WAIT         = -30,
   PRO_RETRIES_EXCEEDED = -40,
   PRO_EXECUTE_FAILED   = -50,
   PRO_BAD_DIR          = 98   // Ugh hope this doesn't conflict with an OS value
};

// ExitOnOutput is only honored on Windows. POSIX always waits for full process completion.
int PROexecuteString(const COLstring& Dir, const COLstring& Command, COLstring* pOut, COLstring* pErr, int TimeoutInSeconds=120, bool ExitOnOutput=false);

#endif // end of defensive include
