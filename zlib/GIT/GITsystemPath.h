#ifndef __GIT_SYSTEM_PATH_H__
#define __GIT_SYSTEM_PATH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: GITsystemPath
//
// Description:
//
// Get the system PATH variable - deliberately it's own concern.
//
// Author: Eliot Muir 
// Date:   Thursday 09 March 2023 - 11:19AM
// ---------------------------------------------------------------------------

class COLwebRequest;

void GITsystemPath(const COLwebRequest& Request);

#endif // end of defensive include
