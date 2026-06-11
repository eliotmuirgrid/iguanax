// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITremote
//
// Description:
//
// Implementation
//
// Author: Matthew Sobkowski 
// Date:   Thursday 24 October 2024 - 08:40AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GIT/GITremote.h>
#include <GIT/GITexecute.h>
#include <COL/COLutils.h>
COL_LOG_MODULE;

bool GITremote(const COLstring& Dir, COLstring* pOut, COLstring* pError){
   COL_FUNCTION(GITremote);
   int ExitCode = GITexecuteString(Dir, "git remote get-url origin", pOut, pError);
   *pOut = COLremoveAllWhiteSpace(*pOut);
   COL_VAR3(ExitCode, *pOut, *pError);
   return ExitCode == 0;
}

