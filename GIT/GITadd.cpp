// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITadd
//
// Description:
//
// Implementation
//
// Author: Tyler Brown 
// Date:   Tuesday 04 April 2023 - 04:56PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GIT/GITadd.h>
#include <GIT/GITexecute.h>
COL_LOG_MODULE;

// TODO unit test
bool GITadd(const COLstring& Dir, const COLstring& File, bool Stage, COLstring* pError){
   COL_FUNCTION(GITadd);
   COLstring Out;
   COLstring Command = Stage ? "git add " + File : "git add -u " + File;
   COL_VAR2(Dir, Command);
   int ExitCode = GITexecuteString(Dir, Command, &Out, pError);
   COL_VAR2(Out, *pError);
   return ExitCode == 0;
}
