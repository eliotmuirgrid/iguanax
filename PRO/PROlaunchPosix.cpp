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
#include "PROutils.h"
#include "PROptrArray.h"

#include <unistd.h>
#include <errno.h>
#include <signal.h>

#include <COL/COLioCheck.h>
#include <COL/COLerror.h>
#include <COL/COLstring.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int PROlaunch(const COLstring& AppDir, const COLstring& FullCommand){
   COL_FUNCTION(PROlaunch);
   COLassertIfMainThread();
   COLarray<COLstring> ExecVpArguments;
   PROptrArray<char> ExecVpArgvArray;

   PROparseCommandLine(FullCommand, &ExecVpArguments);
   PRObuildPosixArgvArray(ExecVpArguments, &ExecVpArgvArray);

   COL_TRC("Forking.");
   pid_t ForkResult = ::fork();
   if (ForkResult < 0) {
      COL_ERR("Fork call failed: " << COLerrorString(errno));
      COL_THROW_STRERROR_PLAIN("Fork call failed.");
   } else if (ForkResult > 0) { //parent
      COL_TRC("Fork returned child PID: " << ForkResult);
      COL_TRC("Parent continues to run");
   } else { //child
      signal(SIGINT, SIG_IGN); // I shall not die! 
      signal(SIGTERM, SIG_IGN);
      signal(SIGKILL, SIG_IGN);
      COL_TRC("This is child, going to execute command" << newline << FullCommand);
      chdir(AppDir.c_str());
      execvp(ExecVpArguments[0].c_str(),(char* const*)ExecVpArgvArray.array());
   }
   return ForkResult;
}