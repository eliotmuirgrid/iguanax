#ifndef __SDBA_PURGE_NOW_H__
#define __SDBA_PURGE_NOW_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeNow
//
// Description:
//
// Api to run log purge with purge rules.
//
// Author: Vismay Shah 
// Date:   Thursday 05 October 2023 - 04:21PM
// ---------------------------------------------------------------------------

class COLwebRequest;
class SDBsystem;

void SDBApurgeNow(const COLwebRequest& Request, SDBsystem* pSystem);

#endif // end of defensive include
