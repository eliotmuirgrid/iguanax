#ifndef __SDB_DIR_H__
#define __SDB_DIR_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBdir
//
// Description:
//
// Functions defining the directories for the logging system
//
// COLstring Today = SDBdate();     // 20230302
// COLstring Year  = SDByear();     // 2023
// time_t    IFW   = 857334466;
// COLstring D     = SDBdate(IFW)   // 19970302
// COLstring Y     = SDByear(IFW)   // 1997
//
// Author: Eliot Muir
// Date:   Thursday 16 February 2023 - 03:49PM
// ---------------------------------------------------------------------------

#include <time.h>

#include <COL/COLstring.h>

#define SDB_DAY  60 * 60 * 24

class COLvar;

bool SDBreadLogConfig(COLvar* pConfig, COLstring* pError);
COLstring SDBgetLogParentDir(const COLvar& LogConfig);
bool SDBsetUpLogDirectory(COLstring* pLogDirPath, bool AllowAltDir, COLstring* pError);
bool SDBlogEncryptionEnabled();

COLstring SDBroot();
COLstring SDBroot2();
COLstring SDBdir(const COLstring& ComponentId, const COLstring& RootDir = SDBroot());

// Timestamp is the millisecond timestamp of the first message in the log file
COLstring SDBlogFile(const COLstring& ComponentId, COLuint64 Timestamp, const COLstring& RootDir = SDBroot());

COLstring SDBoldJournalFile(const COLstring& RootDir = SDBroot());
COLstring SDBjournalFile(const COLstring& RootDir = SDBroot());

// COLstring SDBdate(time_t Timestamp = time(NULL), const char* Format = "%Y%m%d");

COLstring SDBfindNextLogFile(const COLstring& CurrentFile, const COLstring& ComponentId);
COLstring SDBfindPreviousLogFile(const COLstring& CurrentFile, const COLstring& ComponentId, const COLstring& RootDir = SDBroot());

// Returns the path of the log file in which message(s) with the specified millisecond Timestamp are found
COLstring SDBfindLogFile(const COLstring& ComponentId, COLuint64 Timestamp, const COLstring& RootDir = SDBroot());
time_t SDBlogFileTime(const COLstring& LogFilePath, const COLstring& ComponentId, const COLstring& RootDir = SDBroot());

COLstring SDBfindOldest(const COLstring& ParentDir);
COLstring SDBfindNewest(const COLstring& ParentDir);

#endif // end of defensive include
