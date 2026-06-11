#ifndef __GITU_CACHE_H__
#define __GITU_CACHE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITUcache
//
// Description:
//
// Function to create git cache.
//
// Author: Eliot Muir
// Date:   Friday 31 March 2023 - 04:17PM
// ---------------------------------------------------------------------------
#include <COL/COLclosure.h>

class COLstring;
class GITUlist;

class COLthreadPool;
class SCKloop;

void GITUcacheLog(const COLstring& Log, const COLstring& Tags = "");

bool GITUcacheInit(const COLstring& Username, COLstring* pLog);
void GITUcloneList(const COLstring& Username, const GITUlist& List, COLstring* pLog, bool* pResult);
void GITUcloneCollection(const COLstring& Username, SCKloop* pLoop, const GITUlist& List, COLclosure2<bool, COLstring>* pFinished);

// These functions return true if there's an ongoing action and false if its safe to start another action.
bool GITUisCacheRefreshInProgress (bool* pGitCacheRefreshing, COLstring* pError);
#endif // end of defensive include
