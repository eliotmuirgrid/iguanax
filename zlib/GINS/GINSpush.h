#ifndef __GINS_PUSH_H__
#define __GINS_PUSH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSpush
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Wednesday 11 September 2024 - 12:37PM
// ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

struct GINScommitInfo;

void GINSpush(const COLwebRequest& Request, SCKloop* pLoop, GINScommitInfo* pCommitInfo);

#endif // end of defensive include
