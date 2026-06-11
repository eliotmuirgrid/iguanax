#ifndef __GITW_CACHE_TIMESTAMPS_H__
#define __GITW_CACHE_TIMESTAMPS_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITWcacheTimestamps
//
// Description:
//
// Returns a map of timestamp -> [git url array] for sorting the templates
// when adding a component.
//
// Author: Vismay Shah 
// Date:   Wednesday 03 December 2025 - 10:38AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void GITWcacheTimestamps(const COLwebRequest& Request);

#endif // end of defensive include
