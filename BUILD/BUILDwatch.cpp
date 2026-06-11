//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BUILDwatch
//
// Description: Implementation
//
// Author: Eliot Muir
// Date:   Tue 17 Aug 2021 14:35:06 EDT
//---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLstring.h>
#include <COL/COLsinkString.h>
#include <COL/COLutils.h>

#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool BUILDgit(const COLstring& Dir, const COLstring& Command, const COLstring& Args, COLstring* pResult){
   COL_FUNCTION(BUILDgit);
   return 0 == PROexecuteString(Dir, "git " + Command + " " + Args, pResult, pResult, 60);
}

COLstring BUILDwatch(const COLstring& Dir, COLstring* pDescription){
   COL_FUNCTION(BUILDwatch);
   COLstring Result;
   int ExitCode;
   COLstring PullLog;
   BUILDgit(Dir, "pull", "--all", &PullLog);
   COL_VAR(PullLog);
   pDescription->clear();
   BUILDgit(Dir, "log", "--max-count=1 --all --pretty=format:\"%H:%s - %an \"", pDescription);
   COLstring NewHash;
   BUILDgit(Dir, "log", "--max-count=1 --all --pretty=format:\"%H\"", &NewHash);
   return NewHash;
}

void BUILDbuild(const COLstring& Dir, const COLstring& Hash, const COLstring& Description){
   COL_FUNCTION(BUILDbuild);
   COLcout << "BUILD: " << Description << newline;
   COLstring Result;
   BUILDgit(Dir, "checkout", Hash, &Result);
   COLcout << Result << newline;
   Result.clear();
   int ExitCode = PROexecuteString(Dir, "sh build.sh ~/logs/" + Hash, &Result, &Result, 60);
   COLcout << Result << newline;
   COLcout << "Exit code: " << ExitCode << newline;
}

void BUILDloop(const COLstring& Dir){
   COL_FUNCTION(BUILDloop);
   COLstring Description;
   COLstring Hash = BUILDwatch(Dir, &Description);
   COLcout << "GIT start status:" << Description << newline;
   while (true){
      COLstring NewHash = BUILDwatch(Dir, &Description);
      if (Hash != NewHash){
         Hash = NewHash;
         BUILDbuild(Dir, Hash, Description);
      }
      COLsleep(1000);
   }
}
