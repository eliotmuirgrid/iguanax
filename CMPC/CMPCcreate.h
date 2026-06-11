#ifndef __CMPC_CREATE_H__
#define __CMPC_CREATE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CMPCcreate
//
// Description: Api to create a list of components or one from url
//
// Author: Matthew Sobkowski
// Date:   Wednesday 26 March 2025 - 10:31AM
// ---------------------------------------------------------------------------
class BLUAinstanceManager;
class COLwebRequest;
class COLvar;
class IWEBconfig;
class SCKloop;

struct CFGmap;

void CMPCcreate(const COLwebRequest& Request, IWEBconfig* pWebConfig, CFGmap* pMap, BLUAinstanceManager* pManager,
                SCKloop* pLoop);

#endif // end of defensive include
