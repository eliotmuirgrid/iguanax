#ifndef __SDBT_READER_H__
#define __SDBT_READER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTreader
//
// Description:
//
// Unit test helper functions for SDBreader
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 02:12PM
// ---------------------------------------------------------------------------

#include <COL/COLauto.h>
#include <COL/COLmap.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLarray.h>
#include <time.h>

class SDBmessage;

COLmap<COLstring, COLarray<COLauto<SDBmessage>>> SDBTcreateTestLogData(int Messages = 100, COLint64 StartId = 1, time_t Date = time(NULL), int Components = 4, int Size = 1000);

void SDBTcreateTestJournalData(COLstring* pBuffer, const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& LogData, time_t Date = time(NULL), int StartIndex = 0);

void SDBTcreateTestJournalFile(const COLstring& FilePath, time_t Date = time(NULL), int Messages = 100, COLint64 StartId = 1, int Components = 4, int Size = 1000);
void SDBTcreateTestJournalFile(const COLstring& FilePath, const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& LogData);

void SDBTcreateTestLogData(COLstring* pBuffer, const COLarray<COLauto<SDBmessage>>& LogData);
void SDBTcreateTestLogFile(const COLstring& FilePath, time_t Date = time(NULL), int Messages = 100, COLint64 StartId = 1, const COLstring& ComponentId = "Component_1", int Size = 1000);
void SDBTcreateTestLogFile(const COLstring& FilePath, const COLarray<COLauto<SDBmessage>>& LogData);
void SDBTcreateTestLogFileLarge(const COLstring& FilePath, time_t Date = time(NULL), int Messages = 100, COLint64 StartId = 1, const COLstring& ComponentId = "Component_1");

void SDBTcreateTestLogFiles(time_t StartDate, int Days, const COLstring& LogDir, const COLstring& ComponentId = "Component_1", int Messages = 100, COLint64 StartId = 1, int Size = 1000);

#endif // end of defensive include