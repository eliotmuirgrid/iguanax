// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITversion
//
// Description:
//
// Implementation
//
// Author: Orkhan Amikishiyev 
// Date:   Thursday 02 February 2023 - 05:50AM
// ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GIT/GITversion.h>
#include <PRO/PROexecute.h>
COL_LOG_MODULE;

bool GITversion(COLstring* pVersion, COLstring* pVersionError){
   COL_FUNCTION(GITversion);
   COLstring Command;
   Command = "git --version";
   int ExitCode = PROexecuteString("", Command, pVersion, pVersionError);
   COL_VAR3(Command, *pVersionError, ExitCode);
   COL_DUMP("Path:", pVersion->c_str(), pVersion->size());
   pVersion->stripAll('\n');
   pVersion->stripAll('\r');
   COL_DUMP("Path:", pVersion->c_str(), pVersion->size());
   COL_VAR(*pVersion);
   return (ExitCode == 0);
}