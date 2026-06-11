#ifndef __BLUA_SLEEP_H__
#define __BLUA_SLEEP_H__
//---------------------------------------------------------------------------
// Copyright (C) 1997-2021 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: BLUAsleep
// Description:
//    BLUAinstance sleep functions
//
// Author: Vismay Shah
// Date:   Friday May 14 2021
//---------------------------------------------------------------------------

class BLUAinstance2;

void BLUAinstanceTearDownSleep(BLUAinstance2* pInstance);

// IMPORTANT: Must be called from Lua worker thread.
void BLUAinstanceSleep(BLUAinstance2* pInstance, int Length);

#endif // end of defensive include
