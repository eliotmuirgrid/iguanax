#ifndef __SDBT_FILE_H__
#define __SDBT_FILE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTfile
//
// Description:
//
// Test functions for log files?
//
// Author: Eliot Muir
// Date:   Monday 27 February 2023 - 10:15AM
// ---------------------------------------------------------------------------

#include <COL/COLminimumInclude.h>

#include <time.h>

class COLstring;

void SDBTmakeTestMessageSet(const COLstring& LogFile, COLuint64 MessageIdStart, int Count);

void SDBTmakeTestMessageFile(const COLstring& LogFile, COLuint64 MessageIdStart, int Count, int Size = 50000, time_t Date = 0);

#endif // end of defensive include
