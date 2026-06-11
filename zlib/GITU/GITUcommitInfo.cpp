// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcommitInfo
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Monday 27 March 2023 - 01:31PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GIT/GITbranch.h>
#include <GITU/GITUcommitInfo.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITUcommitInfoCore(const COLstring& Dir, COLstring* pCommit, COLstring* pMessage, COLstring* pAuthor, COLstring* pTimeStamp, COLstring* pBranch){
   COL_FUNCTION(GITUcommitInfoCore);
   COLstring Command = "git log -n 1 --pretty=format:\"%h%n%an%n%ar%n%B\"";
   COL_VAR2(Dir, Command);
   COLstring Output, Err;
   int ExitCode = PROexecuteString(Dir, Command, &Output, &Err);
   if (ExitCode != 0){
      return false;
   }
   COL_VAR3(ExitCode, Output, Err);
   Output.split(*pCommit,    Output, "\n");
   Output.split(*pAuthor,    Output, "\n");
   Output.split(*pTimeStamp, Output, "\n");
   Output.split(*pMessage,   Output, "\n");
   *pBranch = GITbranchCurrent(Dir);
   return true;
}

bool GITUcommitInfo(const COLstring& Dir, COLvar* pResult){
   COL_FUNCTION(GITUcommitInfo);
   COLstring Commit, Author, Timestamp, Branch, Message;
   bool Result = GITUcommitInfoCore(Dir, &Commit, &Message, &Author, &Timestamp, &Branch);
   COLvar& Data = *pResult;
   Data["commit"]     = Commit;
   Data["author"]     = Author;
   Data["time_stamp"] = Timestamp;
   Data["message"]    = Message;
   Data["branch"]     = Branch;
   COL_VAR(Data);
   return Result;  
}

bool GITUcommitInfoInString(const COLstring& Dir, COLstring* pResult){
   COL_FUNCTION(GITUcommitInfoInString);
   COLstring Commit, Author, Timestamp, Branch, Message;
   bool Result = GITUcommitInfoCore(Dir, &Commit, &Message, &Author, &Timestamp, &Branch);
   if (!Result){
      *pResult = "Unable to obtain GIT information on this.";
      return false;
   } 
   *pResult = COL_T("Commit ") + Commit + " " + Message+ " by " + Author + " " + Timestamp + " on branch " + Branch;
   return true;
}