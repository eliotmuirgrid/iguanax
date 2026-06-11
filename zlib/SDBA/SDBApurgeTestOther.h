#ifndef __SDBA_PURGE_TEST_OTHER_H__
#define __SDBA_PURGE_TEST_OTHER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBApurgeTestOther
//
// Description:
//
// Return a list of non-component directories found in the log folder
//
// Author: Vismay Shah 
// Date:   Friday 14 February 2025 - 01:26PM
// ---------------------------------------------------------------------------
class COLwebRequest;
class SCKloop;
class SDBsystem;

void SDBApurgeTestOther(const COLwebRequest& Request, SDBsystem* pSystem);

#endif // end of defensive include
