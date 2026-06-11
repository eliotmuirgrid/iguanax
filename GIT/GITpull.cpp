// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITpull
//
// Description:
//
// Implementation
//
// Author: Eliot Muir 
// Date:   Saturday 01 April 2023 - 02:04PM
// ---------------------------------------------------------------------------

#include <COL/COLlog.h>
#include <GITU/GITUconfigFlag.h>
#include <GIT/GITexecute.h>
#include <GIT/GITpull.h>
COL_LOG_MODULE;

// Used in cache - nice to keep separate for tracing clarity.
bool GITpull(const COLstring& Username, const COLstring& Dir, COLstring* pOut, COLstring* pError) {
	COL_FUNCTION(GITpull);
	const COLstring ConfigFlag = GITUconfigFlag(Username);
	const COLstring Command		= "git" + ConfigFlag + " pull";
	COL_VAR(Command);
	const int ExitCode = GITexecuteString(Dir, Command, pOut, pError);
	COL_VAR3(ExitCode, *pOut, *pError);
	if(ExitCode != 0) {
		COL_ERR("git pull failed" << " Dir = " << Dir);
	}
	if(ExitCode == 0 && pOut->find("Already up to date") != npos) { pOut->clear(); }
	return ExitCode == 0;
}