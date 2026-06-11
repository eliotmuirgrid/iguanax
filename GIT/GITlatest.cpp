// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITlatest
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowskiw 
// Date:   Friday 25 October 2024 - 10:00AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GIT/GITlatest.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

bool GITlatest(const COLstring& Dir, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITlatest);
   int ExitCode = GITexecuteString(Dir, "git log -1 --format=%H", pOut, pError);
   *pOut = COLremoveAllWhiteSpace(*pOut);
   COL_VAR3(ExitCode, *pOut, *pError);
   return ExitCode == 0;
}
