#ifndef __CMPC_GIT_HISTORY_H__
#define __CMPC_GIT_HISTORY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCgitHistory
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 30 July 2024 - 09:46AM
// ---------------------------------------------------------------------------
#include <IWEB/IWEBconfig.h>

class COLwebRequest;
class SCKloop;
struct CFGmap;

void CMPCgitHistory(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop);

#endif // end of defensive include
