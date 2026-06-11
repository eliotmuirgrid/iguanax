// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITclean
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 17 September 2024 - 08:40AM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITclean.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

bool GITclean(const COLstring& Dir, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITclean);
   int ExitCode = GITexecuteString(Dir, "git clean -dffx", pOut, pError);
   COL_VAR3(ExitCode, *pOut, *pError);
   return ExitCode == 0;
}

