// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: TUNconnectionRefresh
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Friday 21 July 2023 - 12:19PM
// ---------------------------------------------------------------------------

#include <TUN/TUNconnectionRefresh.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void TUNupdateLastActionTime(time_t* pLastActivity){
   COL_FUNCTION(TUNupdateLastActionTime);
   *pLastActivity = time(NULL);
}

bool TUNneedRefresh(const time_t& LastActivity){
   COL_FUNCTION(TUNneedRefresh);
   COL_TRC(time(NULL) - TUN_IDLE_CONNECTION_REFRESH / 1000);
   COL_TRC(LastActivity);
   return (time(NULL) - TUN_IDLE_CONNECTION_REFRESH / 1000) > LastActivity;
}