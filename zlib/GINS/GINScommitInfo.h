#ifndef __GINSCOMMITINFO_H__
#define __GINSCOMMITINFO_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINScommitInfo.h
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   18/09/24 9:21 AM
//  ---------------------------------------------------------------------------
#include <ctime>
#include <COL/COLstring.h>

class COLvar;

struct GINScommitInfo {
	time_t t_LastCommitTime = 0;
	time_t t_LastAttempt	= 0;
	COLstring LastError		= "";
	COLstring LastWarning	= "";
};

bool GINScommitCheckInterval(time_t LastCommitTime, int Interval);

void GINSsuccessfullPush(GINScommitInfo* pInfo, const COLstring& LastWarning);
void GINSunsuccessfullPush(GINScommitInfo* pInfo, const COLstring& Err);

COLvar GINScommitInfoToVar(const GINScommitInfo& CommitInfo);

#endif  // end of defensive include
