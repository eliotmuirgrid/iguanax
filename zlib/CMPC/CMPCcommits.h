#ifndef __CMPC_COMMITS_H__
#define __CMPC_COMMITS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcommits
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Tuesday 28 February 2023 - 10:20AM
// ---------------------------------------------------------------------------
#include <IWEB/IWEBconfig.h>

class COLwebRequest;
class COLaddress;
class SCKloop;
class COLvar;
struct CFGmap;

void CMPCcreationComplete(COLvar Result, IWEBconfig* pWebConfig, COLaddress Address);

void CMPCcommits(const COLwebRequest& Request, IWEBconfig* pWebConfig, SCKloop* pLoop);

#endif // end of defensive include
