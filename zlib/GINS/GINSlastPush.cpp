//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GINSlastPush.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   17/09/24 3:29 PM
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvar.h>
#include <COL/COLweb.h>
#include <GINS/GINScommitInfo.h>
#include <GINS/GINSlastPush.h>
COL_LOG_MODULE;

void GINSlastPush(const COLwebRequest& Request, GINScommitInfo* pCommitInfo) {
	COL_FUNCTION(GINSlastPush);
	COLvar Response = GINScommitInfoToVar(*pCommitInfo);
	COLrespondSuccess(Request.Address, Response);
}