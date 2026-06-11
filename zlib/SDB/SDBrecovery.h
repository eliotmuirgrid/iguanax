#ifndef __SDB_RECOVERY_H__
#define __SDB_RECOVERY_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBrecovery
//
// Description:
//
// Recovers committed data in journal and flush them to log files after a crash.
//
// Author: John Qi
// Date:   Friday 24 March 2023 - 02:37PM
// ---------------------------------------------------------------------------

#include <COL/COLclosure6.h>
#include <COL/COLhashmap.h>
#include <COL/COLlist.h>
#include <COL/COLmap.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBid.h>

class SCKloop;
class SDBposition;
struct CFGmap;

#define SDB_RECOVER_NO_SPACE   -6543
#define SDB_RECOVER_NO_JOURNAL -4321

// recovers data in journal files and flush them to log files.
// returns status code and queue position map through pCallback
// error code:
// -1 : failed to seek to the beginning of log file for write
// -2 : position map not found at the beginning of the journal file.
// -3 : unexpected content types.
// positive integers : errno for file io errors
void SDBrecover(SCKloop* pLoop, const CFGmap* pConfigMap, const COLstring& EncryptionKey, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, const COLstring& LogRootDir = SDBroot());

void SDBrecoverPositionsFromLogs(COLlist<COLstring>* pComponentList, SCKloop* pLoop, const COLstring& EncryptionKey, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, const COLstring& LogRootDir = SDBroot());


// helper
void SDBtruncateFile(const COLstring& FilePath, COLint64 Size);

// unit test

COLint64 SDBgetFilePosition(int FD, const SDBmessageId& TargetId);
int SDBseekToContent(int FD, COLint64 TargetId);
// recovers data in the specified journal file
void SDBrecoverJournal(COLstring JournalFile, COLstring EncryptionKey, SCKloop* pLoop, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, COLstring LogRootDir);

SDBmessageId SDBreadMessageId(const char* pBuffer, int BufferSize);

bool SDBfindMessageIdEncrypted(COLstring& Buffer, const COLstring& EncryptionKey, const SDBmessageId& TargetId, int& Consumed, COLstring* pReencrypt);

#endif // end of defensive include
