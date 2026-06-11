//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITstatusChanges.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   10/06/25 9:25 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <GIT/GITstatusChanges.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITstatusChanges(const COLstring& Dir, COLstring* pContent, COLstring* pError) {
   COL_FUNCTION(GITstatusChanges);
   COLstring Command = "git status --porcelain -uall .";
   COL_VAR2(Dir, Command);
   int ExitCode = PROexecuteString(Dir, Command, pContent, pError);
   COL_VAR2(*pContent, *pError);
   return ExitCode == 0;
}