// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITexecute
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 07 November 2023 - 11:02AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <COL/COLutils.h>
#include <DIR/DIRpath.h>
#include <GIT/GITexecute.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;


int GITexecuteString(const COLstring& Dir, const COLstring& Command, COLstring* pOut, COLstring* pErr) {
   COL_FUNCTION(GITexecuteString);
   COL_VAR2(Dir, Command);
   const int MaxRetries = 8;
   int ExitCode;
   for (int attempt = 0; attempt < MaxRetries; ++attempt) {
      ExitCode = PROexecuteString(Dir, Command, pOut, pErr);
      if (ExitCode == 0) {
         COL_TRC("Git command succeeded");
         return ExitCode;
      }
      
      if (pErr && COLstringContains(*pErr, "index.lock")) {
         COL_TRC("Git command failed due to index.lock file, retrying attempt " + COLintToString(attempt + 1));
         // If a lock file error is detected, wait for a delay and then retry
         pOut->clear();
         pErr->clear();
         COLsleep(200);
      } else {
         // If the error is not due to a lock file, return false
         COL_TRC("Git command failed due to reason other than presence of index.lock file");
         return ExitCode;
      }
   }
   // Add in reached max attempts to the output and include the git command that failed

   return ExitCode;
}