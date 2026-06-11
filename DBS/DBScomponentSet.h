#ifndef __DBS_COMPONENT_SET_H__
#define __DBS_COMPONENT_SET_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBScomponentSet
//
// Description:
//
// Web API to set database schema
//
// Author: Eliot Muir 
// Date:   Tuesday 08 November 2022 - 08:50AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
struct CFGmap;

void DBScomponentSet(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
