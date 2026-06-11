#ifndef __GITW_API_H__
#define __GITW_API_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWapi
//
// Description: GIT API functions
//
// Author: Eliot Muir 
// Date:   Tue 14 Feb 2023 14:44:17 EST
//---------------------------------------------------------------------------
#include <COL/COLweb.h>
#include <COL/COLhashmap.h>

class SCKloop;

struct GSVitem;

void GITWapiAsync(COLhashmap<COLstring, COLwebHandlerFunc>& ApiMap);
void GITWapi(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap,
				 bool* pGitCacheRefreshing, SCKloop* pLoop);

#endif // end of defensive include
