#ifndef __GITC_GITCAPI_H__
#define __GITC_GITCAPI_H__
// ---------------------------------------------------------------------------
//  Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: GITCapi.h
//
//  Description
//
//  Author: Matthew Sobkowski
//  Date:   10/19/22 12:49 PM
// ---------------------------------------------------------------------------

#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

class SCKloop;
struct GSVitem;
class USRuser;

void GITCworker(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);
void GITCmain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
				  bool* pGitCacheRefreshing, COLmap<COLstring, USRuser>* pUserMap, SCKloop* pLoop);

#endif // end of defensive include