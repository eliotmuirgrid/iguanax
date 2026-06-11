// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: PROexecutePosix
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 16 January 2023 - 03:42PM
// ---------------------------------------------------------------------------
#include <PRO/PROexecute.h>
#include <PRO/PROchild.h>

#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <math.h>

#include <COL/COLsink.h>
#include <COL/COLutils.h>
#include <COL/COLmath.h>

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

// It's possible to see this in action if you ramp up the multi-threaded test to 10000 and run with tracing on.
bool PROretry(const COLstring& Dir, const COLstring& Command, COLsink* pStdOut, COLsink* pStdErr, int Timeout, int* pExitCode, const COLstring& Reason, int Try){
   COL_FUNCTION(PROretry);
   if (Try == 20){
      COL_ERR("Invocation of command " << Command << " failed (" << Reason << ") Run out of retrys.");
      *pExitCode = PRO_RETRIES_EXCEEDED;
      return false; 
   }
   Try++;
   COL_ERR("Invocation of command " << Command << " failed (" << Reason << ").  Retry " << Try << " of 20"); 
   COLsleep(200 + COLmath::randomNumber(1000));
   return PROexecuteOS(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, Try, false);
}

bool PROcollectErr(int ResultCode, COLstring* pErr){
   // I deliberately DO NOT have tracing before we collect errno - since errno can be modified by tracing
   if (ResultCode != 0){
      *pErr = COLerrorString(errno);
      return false;
   }
   return true;
}

void PROinitPipe(int PipeFd[2]){
   COL_FUNCTION(PROinitPipe);
   // Very important that we make these -1 - we call close on them
   // If we set them as 0 then this is the standard input descriptor
   // Which I used to close in the child so that was causing an error before.
   PipeFd[0] = -1;   
   PipeFd[1] = -1;   
}

bool PROopenPipe(int PipeFd[2], COLstring* pErr){
   COL_FUNCTION(PROopenPipe);
   if (pipe(PipeFd) == 0){
      return true;
   }
   *pErr = COLerrorString(errno);  // Deliberately getting errno before doing any tracing.
   return false;
}

void PROclosePipe(int PipeFd[2]){
   COL_FUNCTION(PROclosePipe);
   COL_VAR2(PipeFd[0], PipeFd[1]);
   // TODO - only close if the FD are != -1, assign -1 to them.
   close(PipeFd[0]);
   close(PipeFd[1]);
}

void PROclean(int StandardPipe[2], int ErrPipe[2], int CommPipe[0]){
   COL_FUNCTION(PROclean);
   PROclosePipe(StandardPipe);
   PROclosePipe(ErrPipe);
   PROclosePipe(CommPipe);
}

bool PROmakeNonBlockingFd(int Fd){
   COL_FUNCTION(PROmakeNonBlockingFd);
   int Flags = fcntl(Fd, F_GETFL, 0);
   int Success = fcntl(Fd, F_SETFL, Flags | O_NONBLOCK);
   if (Success !=0 ){
      int LastErr = errno;  // Deliberately collecting errno *before* tracing since errno can be modifed by tracing.
      COL_ERR("Error setting fnctl on " << Fd << " " << COLerrorString(LastErr));
      return false;
   }
   return true;
}

void PROreadPipe(int Fd, COLsink* pSink){
   COL_FUNCTION(PROreadPipe);
   char Buffer[1024];  // TODO - what is the optimal value here?
   int CountRead;
   int LastErr = 0;
   do{
      CountRead = ::read(Fd, Buffer, sizeof(Buffer));
      LastErr = errno;  // Deliberately collecting errno *before* tracing since errno can be modifed by tracing.
      COL_VAR(CountRead);
      COL_DUMP("Err Data:", Buffer, CountRead);
      if (CountRead > 0){
         pSink->write(Buffer, CountRead);
      }
   } while(CountRead >0);
   if (CountRead < 0){
      if (LastErr != EAGAIN){ // would block
         COL_ERR("Code: " << LastErr << " - " << COLerrorString(LastErr));
      }
   }
}

void PROreadFromPipe(int Flags, int Fd, COLsink* pSink){
   COL_FUNCTION(PROreadFromPipe);
   if (Flags & POLLIN){
      PROreadPipe(Fd, pSink);
   }
}

bool PROgetExitCode(int WaitStatus, int* pExitCode){
   COL_FUNCTION(PROgetExitCode);
   COL_VAR(WaitStatus);
   if (WIFEXITED(WaitStatus)){
      COL_VAR(WEXITSTATUS(WaitStatus));
      *pExitCode = WEXITSTATUS(WaitStatus);
      return true;
   }
   if (WIFSIGNALED(WaitStatus)){ COL_ERR("Process was signalled with signal " << WTERMSIG(WaitStatus)); }
   if (  WCOREDUMP(WaitStatus)){ COL_ERR("Process core dumped.");                                       }
   if ( WIFSTOPPED(WaitStatus)){ COL_ERR("Process stopped with signal " << WSTOPSIG(WaitStatus));       }
   COL_TRC("Process didn't exit normally.");
   return false;
}

int PROgetTermSignal(int WaitStatus){
   COL_FUNCTION(PROgetTermSignal);
   if (WIFSIGNALED(WaitStatus)){
      return WTERMSIG(WaitStatus);
   }
   return -1;
}

void PROwatchFd(pollfd PCol[], int i, int Fd){
   COL_FUNCTION(PROwatchFd);
   PCol[i].fd = Fd;
   PCol[i].events = POLLIN;
}

bool PROclientReady(int CommPipe[2]){
   COL_FUNCTION(PROclientReady);
   struct pollfd PCol[1];
   PROwatchFd(PCol, 0, CommPipe[0]);
   int Ready = poll(PCol, 1, 2000);  // Wait up to 2 seconds
   if (Ready == 0){
      return false;
   }
   // Notice I don't bother to actually read the pipe - just confirm whether was ready or not if not then it implies fork failed.
   COL_TRC("Client is running - got notification of data on client pipe");
   return true;
}

// I call this from PROexecParent mainly to keep PROexecParent from being a monster function
bool PROreadLoop(pollfd PCol[2], int TimeoutInSeconds, pid_t ChildPid, int* pWaitStatus, COLsink* pStdErr, COLsink* pStdOut, int* pExitCode){
   COL_FUNCTION(PROreadLoop);
   time_t Timeup = time(NULL) + TimeoutInSeconds;
   pid_t ChildIdReturn = 0;
   do{
      int Ready = poll(PCol, 2, 1000);  // time step of 1000
      if (Ready > 0){
         COL_TRC("Reset timeup");
         Timeup = time(NULL) + TimeoutInSeconds;
      }
      COL_VAR4(Timeup, Ready,PCol[0].revents, PCol[1].revents);
      PROreadFromPipe(PCol[0].revents, PCol[0].fd, pStdErr);
      PROreadFromPipe(PCol[1].revents, PCol[1].fd, pStdOut); 
      ChildIdReturn = waitpid(ChildPid, pWaitStatus, WNOHANG);  // WNOHANG means this call is non blocking.
      if (ChildIdReturn == -1){
         int LastErr = errno;  // Deliberately getting errno before tracing since tracing can change the value of errno
         COL_ERR("waitpid failed: " << COLerrorString(LastErr));
         *pExitCode = PRO_BAD_WAIT;
         return false;
      } 
      COL_VAR3(ChildIdReturn, *pWaitStatus, *pExitCode);
      if (time(NULL) > Timeup){
         *pExitCode = PRO_TIMED_OUT;
         COL_ERR("Timed out");
         return false;
      }
   } while (ChildIdReturn == 0);
   return true;
}

bool PROexecParent(const COLstring& Dir, const COLstring& Command, pid_t ChildPid, int StandardOut[2], int StandardErr[2], int CommPipe[2],
                   COLsink* pStdOut, COLsink* pStdErr, int* pExitCode, int Timeout, int Try){
   COL_FUNCTION(PROexecParent);
   close(StandardErr[1]);                
   close(StandardOut[1]);                
   close(CommPipe[1]);
   PROmakeNonBlockingFd(StandardErr[0]); 
   PROmakeNonBlockingFd(StandardOut[0]); 
   PROmakeNonBlockingFd(CommPipe[0]);
   if (!PROclientReady(CommPipe)){  // This is seeing if the Comm pipe had something written to it to indicate we got to execvp
      close(StandardErr[0]); close(StandardOut[0]); close(CommPipe[0]);
      COL_ERR(Command << " did not start so killing it.");
      kill(ChildPid, SIGKILL);  // Zombie process needs to killed, not asked to stop (SIGINT)
      return PROretry(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, "Child process didn't fork correctly.", Try);
   }
   struct pollfd PCol[2]; 
   PROwatchFd(PCol, 0, StandardErr[0]); 
   PROwatchFd(PCol, 1, StandardOut[0]);
   int WaitStatus=0;
   if (!PROreadLoop(PCol, Timeout, ChildPid, &WaitStatus, pStdErr, pStdOut, pExitCode)){  // we timed out or got wait error
      kill(ChildPid, SIGKILL);  COL_ERR("Killed process for " << Command);
      close(StandardErr[0]); close(StandardOut[0]); close(CommPipe[0]);
      return false;
   }
   PROreadPipe(StandardErr[0], pStdErr); 
   PROreadPipe(StandardOut[0], pStdOut);
   close(StandardErr[0]); close(StandardOut[0]); close(CommPipe[0]);
   if (PROgetExitCode(WaitStatus, pExitCode)){ // Get the exit code
      COL_TRC("Exit code was " << *pExitCode);
   } else {
      COL_ERR(Command << " did not exit correctly."); 
      if (4 == PROgetTermSignal(WaitStatus)){
         // Why do I have this?  While executing PROexample with 10000 instances one of stopped with this signal.  
         // So it's a bit of a gamble to try this retry and it might bite us...
         return PROretry(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, "Process received SIGKILL", Try);
      }
      *pExitCode = PRO_BAD_EXIT;
   }
   COL_VAR(*pExitCode);
   return true;
}

void PROsetError(const COLstring& Error, COLsink* pStdErr){
   COL_FUNCTION(PROsetError);
   pStdErr->write(Error.get_buffer(), Error.size());
}

bool PROexecuteOS(const COLstring& Dir, const COLstring& Command, COLsink* pStdOut, COLsink* pStdErr, int Timeout, int* pExitCode, int Try, bool ExitOnOutput){
   COL_FUNCTION(PROexecuteOS);
   // ExitOnOutput remains a Windows-specific mode; POSIX always waits for full process completion.
   (void)ExitOnOutput;
   *pExitCode = -10;  
   int StandardOut[2];
   int StandardErr[2];
   // This is a solution to the problem that fork 'appears' to work - a child is created but it doesn't function
   // probably because we are running out of some operating system resource like file descriptors. So I have this
   // extra pipe that the child writes to before it exec's the process, so this way we have a way to validating if the
   // child fork has been successful.
   int CommPipe[2];  
   PROinitPipe(StandardErr); 
   PROinitPipe(StandardOut);
   PROinitPipe(CommPipe);

   COLstring Error;
   if (!PROopenPipe(StandardOut, &Error) || !PROopenPipe(StandardErr, &Error) || !PROopenPipe(CommPipe, &Error)){
      PROclean(StandardOut, StandardErr, CommPipe);
      return PROretry(Dir, Command, pStdOut, pStdErr, Timeout, pExitCode, "Unable to open pipe " + Error, Try);
   }
   pid_t Pid = fork();
   if (Pid == -1){
      PROcollectErr(Pid, &Error);  // I HAVE NEVER MANAGED TO EXERCISE THIS ERROR CONDITION
      PROsetError(Error, pStdErr); 
      PROclean(StandardOut, StandardErr, CommPipe);
      COL_ERR(Error << " fork failed for " << Command);
      return false;
   } else if (Pid == 0){
      // we are the child
      PROexecChild(Dir, Command, StandardOut, StandardErr, CommPipe);
      return true;
   } else {
      return PROexecParent(Dir, Command, Pid, StandardOut, StandardErr, CommPipe, pStdOut, pStdErr, pExitCode, Timeout, Try);
   } 
}
