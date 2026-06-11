// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: INSTexists
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Wednesday 09 August 2023 - 11:18AM
// ---------------------------------------------------------------------------

#include <INST/INSTexists.h>

#include <FIL/FILutils.h>
#include <FIL/FILpathUtils.h>

#include <COL/COLstring.h>
#include <COL/COLvarUtils.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

void INSTexists(const COLwebRequest& Request){
   COL_FUNCTION(INSTexists);
   const COLvar& Data = Request.Data;
   COLstring Dir = COLvarRequiredString(Data, "dir");
   const COLstring IgauanaExe = "IguanaX/iguana.exe";
   COLstring Path = FILpathAppend(Dir, IgauanaExe);
   FILcorrectPathSeparators(Path);
   Path = FILexpand(Path);
   COLvar Result;
   Result["exists"] = FILfileExists(Path);
   Result["path"]   = Path;
   COL_VAR(Result.json(1));
   return COLrespondSuccess(Request.Address, Result);
}