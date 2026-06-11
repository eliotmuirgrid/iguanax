#ifndef __TRNC_FIELDS_REFRESH_ALL_H__
#define __TRNC_FIELDS_REFRESH_ALL_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCfieldsRefreshAll
//
// Description:
//
// Api to refresh all custom field information
//
// Author: Vismay Shah 
// Date:   Monday 27 January 2025 - 11:44AM
// ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void TRNCfieldsRefreshAll(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
