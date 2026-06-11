#ifndef __GINS_API_H__
#define __GINS_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GINSapi
//
// Description:
//
// All of the APIs for Git Instance settings.
//
// Author: Tyler Brown 
// Date:   Monday 09 September 2024 - 11:34AM
// ---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

class SCKloop;
class USRuser;

struct GINScommitInfo;

void GINSapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop,
				 COLmap<COLstring, USRuser>* pUserMap, GINScommitInfo* pCommitInfo);

#endif // end of defensive include
