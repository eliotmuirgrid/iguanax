#ifndef __TRNC_SET_TAGS_H__
#define __TRNC_SET_TAGS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TRNCsetTags
//
// Description:
//
// TODO
//
// Author: Tyler Brown 
// Date:   Friday 16 February 2024 - 11:26AM
// ---------------------------------------------------------------------------
class COLwebRequest;
struct CFGmap;
class SCKloop;

void TRNCsetTags(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
