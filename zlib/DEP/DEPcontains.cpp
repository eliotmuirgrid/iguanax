// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DEPcontains
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 16 April 2025 - 08:55AM
// ---------------------------------------------------------------------------

#include <DEP/DEPcontains.h>

#include <FIL/FILutils.h>
#include <MATCH/MATCHsearchDir.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

bool DEPdirContainsFunc(const COLstring Dir, const COLstring FunctionName){
   COL_FUNCTION(DEPdirContainsFunc);
   if(FunctionName.is_null()){
      return false;
   }
   COLvar Result;
   COL_VAR2(Dir, FunctionName);
   bool Found = MATCHsearchDir(FunctionName, Dir, true, &Result);
   COL_VAR2(Result, Found);
   return Found;
}
