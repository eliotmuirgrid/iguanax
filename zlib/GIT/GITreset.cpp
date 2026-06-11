// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITreset
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Thursday 11 May 2023 - 12:31PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITexecute.h>
#include <GIT/GITreset.h>
COL_LOG_MODULE;

// Used in cache - nice to keep separate for tracing clarity.
bool GITreset(const COLstring& Dir, const COLstring& Branch, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITreset);
   int ExitCode = GITexecuteString(Dir, "git reset --hard " + Branch, pOut, pError);
   COL_VAR3(ExitCode, *pOut, *pError);
   if (ExitCode != 0){
      COL_ERR("git reset failed"  << " Dir = " << Dir);
   }
   return ExitCode == 0;
}