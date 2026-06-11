// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROchild
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 16 January 2023 - 05:23PM
// ---------------------------------------------------------------------------

#include <PRO/PROchild.h>
#include <PRO/PROptrArray.h>
#include <PRO/PROutils.h>
#include <PRO/PROexecute.h>

#include <unistd.h>
#include <errno.h>

#include <FIL/FILutils.h>

#include <COL/COLerror.h>
#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

// Why do we close file descriptors:
// https://utcc.utoronto.ca/~cks/space/blog/unix/ForkFDsAndRaces
// https://stackoverflow.com/questions/56650579/why-should-i-close-all-file-descriptors-after-calling-fork-and-prior-to-callin#:~:text=Not%20only%20because%20of%20security,less%20free%20file%20descriptors%20available).
// TODO - could simplify this code if we didn't need Comm
void PROcloseFileDescriptors(int CommFd){
   //COL_FUNCTION(PROcloseFileDescriptors);
   int OpenMax = sysconf(_SC_OPEN_MAX);
   if (OpenMax == -1){
      COL_ERR("sysconf failed");
      OpenMax = 1024;  // Hopefully this number will be okay - that is a lot of open files.
   }
   //COL_VAR(OpenMax);
   for (int i=3; i < OpenMax; i++){  // Why do we start from 3?  Well 0 = standard input, 1 = standard out and 2 = standard err
      if (i != CommFd) {
         close(i);
      }
   }
}

bool PROdup(int Fd1, int Fd2, COLstring* pErr){
   //COL_FUNCTION(PROdup);
  // COL_VAR2(Fd1, Fd2);
   int ReturnFd = dup2(Fd1, Fd2);
   if (ReturnFd == -1){
      *pErr = COLerrorString(errno);
      return false;
   }
   //COL_VAR(ReturnFd);
   return true;
}

// For debugging - it's hard to do tracing in the child process
void PROwritePidFile(const COLstring& Data){
   pid_t ChildPid = getpid();
   FILwriteFile(COLint32ToString(ChildPid) + ".pid", Data);
}

// This is the CHILD
void PROexecChild(const COLstring& Dir, const COLstring& Command,  int StandardOut[2], int StandardErr[2], int CommPipe[2]){
   COL_FUNCTION(PROexecChild);
   COL_VAR(Command);
   //PROwritePidFile("In PROexecChild");
   // Close the read ends of all the pipes
   close(StandardOut[0]);
   close(StandardErr[0]);
   close(CommPipe[0]);
   //COL_VAR2(StandardOut[1], StandardErr[1]);
   // We have some fancy parsing logic to parse the command line arguments into the form needed by execvp
   COLarray<COLstring> Arguments;
   PROparseCommandLine(Command, &Arguments);
   PROptrArray<char> ArgArray;
   PRObuildPosixArgvArray(Arguments, &ArgArray);
//   COL_VAR(Arguments[0].c_str());
   char** argv = ArgArray.array();
   COLstring Error;
   if (!PROdup(StandardErr[1], 2, &Error)){ COL_ERR(Error); exit(-1); }
   if (!PROdup(StandardOut[1], 1, &Error)){ COL_ERR(Error); exit(-1); }
   PROcloseFileDescriptors(CommPipe[1]);  // Very common thing to do
   // I write one character to the CommPipe[1] so we have a solid way of communicating to the parent process
   // that the child has successfully got to this point.  It's an open design question as to whether the use of an entire PIPE for
   // this purpose is overkill.
   write(CommPipe[1], "S", 1);  
   if (!Dir.is_null()) {      
      //COL_TRC("Change working directory to " << Dir);
      if (0 != chdir(Dir.c_str())){
         COLcerr << Dir << " not found.";
         _exit(PRO_BAD_DIR);
         return;
      }
   }   
   // SIGTERM etc. won't be sent to the child.
   // Without this Ctrl+C unexpected will terminate all processes running with PROexecute
   setsid(); 
   //PROwritePidFile("About to exec child.");
   execvp(Arguments[0].c_str(),argv);  // This function ONLY returns if an error has occurred.  The error is written into errno.
   int LastErr = errno;
   if(LastErr == ENOENT) { COLcerr << Arguments[0].c_str() << " is not an executable file"; }
   //PROwritePidFile("For " + Command + " Execvp failed " + COLerrorString(LastErr));
   _exit(LastErr);
}
