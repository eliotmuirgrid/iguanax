#ifndef __GITW_TEMPLATES_REFRESH_H__
#define __GITW_TEMPLATES_REFRESH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWtemplatesRefresh
//
// Description:
//
// Refresh the templates
//
// Author: Eliot Muir 
// Date:   Tuesday 14 February 2023 - 04:34PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class SCKloop;

void GITWtemplatesRefresh(const COLwebRequest& Request, bool* pGitCacheRefreshing, SCKloop* pLoop);

#endif // end of defensive include
