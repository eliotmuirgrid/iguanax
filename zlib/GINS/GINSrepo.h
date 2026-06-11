#ifndef __GINS_REPO_H__
#define __GINS_REPO_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSrepo
//
// Description:
//
// Returns the upstream repository for the instance
//
// Author: Tyler Brown 
// Date:   Monday 09 September 2024 - 11:35AM
// ---------------------------------------------------------------------------
class SCKloop;
class COLwebRequest;

void GINSrepo(const COLwebRequest& Request, SCKloop* pLoop);

#endif // end of defensive include
