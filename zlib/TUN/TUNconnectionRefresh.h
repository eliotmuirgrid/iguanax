#ifndef __TUN_CONNECTION_REFRESH_H__
#define __TUN_CONNECTION_REFRESH_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNconnectionRefresh
//
// Description:
//
// Functions for interacting with the last activity timer for the TUNclientState.
//
// Author: Vismay Shah 
// Date:   Friday 21 July 2023 - 12:19PM
// ---------------------------------------------------------------------------
#include <time.h>

#define TUN_IDLE_CONNECTION_REFRESH 300000

void TUNupdateLastActionTime(time_t* pLastActivity);

bool TUNneedRefresh(const time_t& LastActivity);

#endif // end of defensive include
