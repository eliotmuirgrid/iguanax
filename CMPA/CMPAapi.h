#ifndef __CMPA_API_H__
#define __CMPA_API_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCapi
//
// Description:
//
// Fresh API to create components.
//
// Author: Eliot Muir
// Date:   Wednesday 15 February 2023 - 10:29AM
// ---------------------------------------------------------------------------
#include <COL/COLhashmap.h>
#include <COL/COLweb.h>
#include <IWEB/IWEBconfig.h>

#include <COL/COLauto.h>

struct CFGmap;

class SCKloop;
class BLUAinstanceManager;

void CMPAapiMain(COLhashmap<COLstring, COLauto<COLclosure1<const COLwebRequest&>>>& ApiMap, SCKloop* pLoop,
                 IWEBconfig* pWebConfig, BLUAinstanceManager* pManager, CFGmap* pMap, bool* pGitCacheRefreshing);

#endif  // end of defensive include
