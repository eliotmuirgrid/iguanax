#ifndef __SDBT_RECOVERY_H__
#define __SDBT_RECOVERY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTrecovery
//
// Description:
//
// Unit test helper functions for SDBrecovery
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 02:00PM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLmap.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLarray.h>

#include <SDB/SDBmessage.h>

#include <time.h>

int SDBTcalculateLogFileSize(const COLarray<COLauto<SDBmessage>>& MessageList);

COLmap<COLstring, int> SDBTcreateTestJournalFileForNoCrash(const COLstring& FilePath, time_t Date = time(NULL), int Messages = 100, COLint64 StartId = 1, int Components = 4, int Size = 1000);

#endif // end of defensive include
