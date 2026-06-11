#ifndef __SDBA_OLDEST_H__
#define __SDBA_OLDEST_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBAoldest
//
// Description:
//
// Gets the time of the oldest and newest log files, used by the reposition date picker.
//
// Author: Vismay Shah 
// Date:   Thursday 21 September 2023 - 08:51AM
// ---------------------------------------------------------------------------
class COLwebRequest;

void SDBAoldest(const COLwebRequest& Request);

#endif // end of defensive include
