
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINScommitInfo.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   18/09/24 9:21 AM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <GINS/GINScommitInfo.h>
COL_LOG_MODULE;

bool GINScommitCheckInterval(time_t LastCommitTime, int Interval) {
	COL_FUNCTION(GINScommitCheckInterval);
	const time_t CurrentTime = time(nullptr);
	const double Diff			 = difftime(CurrentTime, LastCommitTime);
	COL_VAR(Diff);
	// COL_VAR(Interval);
	int IntervalSecs = Interval * 60;
	COL_VAR(IntervalSecs);
	return Diff > IntervalSecs;
}

void GINSsuccessfullPush(GINScommitInfo* pInfo, const COLstring& LastWarning) {
	COL_FUNCTION(GINSsuccessfullPush);
	pInfo->LastError.clear();
	pInfo->LastWarning      = LastWarning;
	pInfo->t_LastCommitTime = time(nullptr);
	pInfo->t_LastAttempt	= time(nullptr);
}

void GINSunsuccessfullPush(GINScommitInfo* pInfo, const COLstring& Err) {
	COL_FUNCTION(GINSunsuccessfullPush);
	pInfo->LastError		= Err;
	pInfo->t_LastAttempt = time(nullptr);
}

COLvar GINScommitInfoToVar(const GINScommitInfo& CommitInfo) {
	COL_FUNCTION(GINScommitInfoToVar);
	COLvar Var;
	Var["attempted"]    = COLint64ToString(CommitInfo.t_LastAttempt);
	Var["successful"]   = COLint64ToString(CommitInfo.t_LastCommitTime);
	Var["last_error"]   = CommitInfo.LastError;
	Var["last_warning"] = CommitInfo.LastWarning;
	COL_VAR(Var);
	return Var;
}