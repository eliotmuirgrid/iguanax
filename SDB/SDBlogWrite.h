#ifndef __SDB_LOG_WRITE_H__
#define __SDB_LOG_WRITE_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBlogWrite
//
// Description:
//
// Everything related to writing to log file.
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure.h>
#include <COL/COLlist.h>
#include <COL/COLmap.h>
#include <SDB/SDBlogThread.h>

class COLstring;
class SDBsystem;

// IX-2054 Reduced the max the size of log files so that it is faster to load messages.
// https://bitbucket.org/interfaceware/main2/commits/972710a8446406b1526679bbf7b72af1d9ce6dbf
#define SDB_LOG_ROLLOVER_SIZE 33554432      // 32 MiB

void SDBwriteLogFiles(COLmap<COLstring, SDBlogHandle>& HandleMap, COLmap<COLstring, SDBlogTaskData>* pData, const COLstring& EncryptionKey, const COLstring& LogRootDir);

void SDBwriteLogs(SDBsystem* pSystem, COLclosure0* pCallback, const COLlist<COLstring>& ComponentList);

// for SDBrecovery
void SDBflushLogMessages(SDBlogHandle& Handle, SDBlogTaskData& Data, const COLstring& EncryptionKey, const COLstring& LogRootDir);

// for SDBserviceLog
COLstring SDBserializeLogMessages(COLlist<std::shared_ptr<SDBmessage>>* pMessageList, const COLhashmap<COLstring, SDBid>* pLogPositions, bool FlushPositions, COLstring* pBuffer, const COLstring& LogRootDir);

#endif // end of defensive include
