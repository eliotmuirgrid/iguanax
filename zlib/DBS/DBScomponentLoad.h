#ifndef __DBS_COMPONENT_LOAD_H__
#define __DBS_COMPONENT_LOAD_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBScomponentLoad
//
// Description:
//
// Web API to load component schema.
//
// Author: Eliot Muir 
// Date:   Tuesday 08 November 2022 - 08:50AM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
struct CFGmap;

void DBScomponentLoad(const COLwebRequest& Request, CFGmap* pMap, SCKloop* pLoop);

#endif // end of defensive include
