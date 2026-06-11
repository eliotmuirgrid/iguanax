#ifndef __CMPC_CMPCFIELDSGET_H__
#define __CMPC_CMPCFIELDSGET_H__
//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: CMPCfieldsGet.h
//
//  Description: Web api to get fields for component card
//
//
//  Author: Matthew Sobkowski
//  Date:   4/19/23 10:40 AM
//  ---------------------------------------------------------------------------
class BLUAinstanceManager;
class CFGfield;
class COLwebRequest;
class SCKloop;

struct CFGmap;

void CMPCfieldsGet(const COLwebRequest& Request, CFGmap* pMap, BLUAinstanceManager* pManager, SCKloop* pLoop);
void CMPCfieldsGetFieldsData(const COLarray<CFGfield>& Grammar, bool UseCachedEnvironment, const COLvar& CachedEnvironment,
                             const COLmap<COLstring, COLstring>& FieldValues, COLvar* pResult);

#endif // end of defensive include