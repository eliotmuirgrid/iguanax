// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBenvVar
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Fri May  8 16:41:39 EDT 2026
// ---------------------------------------------------------------------------

#include <SDB/SDBenvVar.h>
#include <ENVC/ENVCcache.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

int SDBmaxMessageSizeMB() {
   return ENVCcacheGetEnvInteger(SDB_ENV_NAME_MAX_MSG_SIZE_MB, SDB_DEFAULTS_MAX_MSG_SIZE_MB);
}
