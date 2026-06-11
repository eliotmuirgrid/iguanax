#ifndef __SDB_ENV_VAR_H__
#define __SDB_ENV_VAR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2026 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBflush
//
// Description:
//
// SDB environemnt variables
//
// Author: John Q
// Date:   Fri May  8 16:41:39 EDT 2026
// ---------------------------------------------------------------------------
#include <COL/COLminimumInclude.h>

#define SDB_SIZE_MB  1000000

// max message file size
#define SDB_ENV_NAME_MAX_MSG_SIZE_MB              "IFW_MAX_MESSAGE_SIZE_MB"
#define SDB_DEFAULTS_MAX_MSG_SIZE_MB              128  // MB

int SDBmaxMessageSizeMB();

#endif // end of defensive include
