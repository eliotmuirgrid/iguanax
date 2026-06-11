// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITcommits
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 28 February 2023 - 10:26AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>
#include <GIT/GITcommits.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

void GITparseCommits(const COLstring& LogString, COLvar* pOut) {
   COL_FUNCTION(GITparseCommits);
   COLstring Rest, Log;
   Rest = LogString + "\n";
   (*pOut).setArrayType();
   while(Rest.split(Log, Rest, "\n")) {
      COLstring Hash, Author, Date, Timestamp, Msg, Branch;
      Log.split(Hash, Log,"::");
      Log.split(Author, Log, "::");
      Log.split(Date, Log, "::");
      Log.split(Timestamp, Log, "::");
      Log.split(Msg, Log, "::");
      Log.split(Branch, Log, "::");
      COL_VAR4(Hash, Author, Date, Msg);
      COL_VAR(Branch);
      COLvar LogData;
      LogData["hash"]    = Hash;
      LogData["author"]  = Author;
      LogData["date"]    = Date;
      LogData["timestamp"] = Timestamp;
      LogData["message"] = Msg;
      LogData["branch"] = Branch;
      (*pOut).push_back(LogData);
   }
}

bool GITcommits(const COLstring& Dir, COLstring* pContent, COLstring* pError, bool Full, bool All){
   COL_FUNCTION(GITcommits);
	if(!FILfileExists(Dir)) {
		COL_TRC(Dir + " does not exist");
		return true;
	}
   COLstring Out,Err;
   int ExitCode=PROexecuteString(Dir,"git rev-parse --verify --quiet HEAD",&Out,&Err);
   COL_VAR3(ExitCode,Out,Err);
   if(ExitCode!=0){
      if(pContent)*pContent="";
      if(pError)*pError="";
      return true;
   }
   // All == true: show commits reachable from HEAD OR its upstream (covers behind/ahead),
   //              without including unrelated branches.
   // All == false: show commits reachable from the currently checked-out HEAD.
   COLstring Args = All ? "HEAD @{u}" : "HEAD";
   COLstring Command = "git log -n 200 " + Args + " --pretty=format:\"%h::%an::%ad::%ct::%s::%d\"";
   if(!Full) {
      // If we want only commits relevant to this directory:
      // Since we run the command with working directory = Dir, "." is the safe pathspec.
      Command += " -- .";
   }
   COL_VAR2(Dir, Command);
   // If @{u} is not configured, the command will fail. Fall back to HEAD-only.
   COLstring LocalErr;
   COLstring* ErrPtr = pError ? pError : &LocalErr;
   ExitCode = PROexecuteString(Dir, Command, pContent, ErrPtr);
   COL_VAR(ExitCode);
   if(All && ExitCode != 0){
      COLstring Fallback = "git log -n 200 HEAD --pretty=format:\"%h::%an::%ad::%ct::%s::%d\"";
      if(!Full) { Fallback += " -- ."; }
      COL_VAR(Fallback);
      ExitCode = PROexecuteString(Dir, Fallback, pContent, ErrPtr);
      COL_VAR(ExitCode);
      // If fallback succeeded, suppress the upstream error.
      if(ExitCode == 0 && pError) { pError->clear(); }
   }
   if (ExitCode != 0){
      if(pError) { COL_ERR(*pError); }
   }
   return ExitCode == 0;
}

bool GITlastCommitTime(const COLstring& Dir, COLstring* pContent, COLstring* pError){
   COL_FUNCTION(GITcommits);
	if(!FILfileExists(Dir)) {
		COL_TRC(Dir + " does not exist");
		return true;
	}
   COLstring Command = "git log -n 1 --all --pretty=format:\"%ct\"";
   COL_VAR2(Dir, Command);
   int ExitCode = PROexecuteString(Dir, Command, pContent, pError);
   COL_VAR(ExitCode);
   if (ExitCode != 0){
      COL_ERR(*pError);
   }
   return ExitCode == 0;
}