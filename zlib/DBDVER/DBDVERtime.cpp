// ---------------------------------------------------------------------------
// Copyright (C) 1997-2022 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: DBDVERapi
//
// Description:  Implementation
//
// Author: Matthew Sobkowski
// Date:   Wednesday August 16, 2023
// ---------------------------------------------------------------------------
#include <time.h>
#include <DBDVER/DBDVERtime.h>

#include <DBDVER/DBDVERgitInfo.h>

#include <COL/COLvar.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

COLstring DBDVERconvertEpoch() {
   COL_FUNCTION(DBDVERconvertEpoch);
   time_t EpochTime = atoll(DBDVER_TIME_STAMP);
   struct tm* tm_info = localtime(&EpochTime);
   char Buffer[100]; // Generous, but time zones on windows can get long.
   int Written = strftime(Buffer, sizeof(Buffer), "%A %b %d %Y %H:%M (%Z)", tm_info);
   if (!Written) {
      COL_TRC("Error generating timestamp!");
   }
   return COLstring(Buffer);
}