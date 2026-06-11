#ifndef __SDB_READER_H__
#define __SDB_READER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBreader
//
// Description:
//
// Reader class for reading messages for dequeue, log browsing, etc.
//
// Author: Eliot Muir
// Date:   Tuesday 28 February 2023 - 09:30AM
// ---------------------------------------------------------------------------

#include <SDB/SDBcontent.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBid.h>

#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLlist.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

#include <time.h>

#define SDB_END_OF_LOG               -7654
#define SDB_HUGE_MESSAGE             -5432
#define SDB_READER_BAD_CONFIG        -5431   // before beginning of log
#define SDB_READER_BUF_LOAD_SIZE   8388608   // 8 MiB
#define SDB_READER_BUF_MAX_SIZE  134217728   // 128 MiB

class SCKloop;
class SDBmessage;
class SDBmessageId;

struct SDBreaderConfig {
   COLstring ComponentId;
   COLuint64 DateTime = 0;
   COLstring LogRootDir = SDBroot();
};

class SDBcorruptRegion {
public:
   SDBcorruptRegion(int iPosition, int iSize) : Position(iPosition), Size(iSize) {}
   COLuint64 Position = 0;    // file position
   int Size = 0;              // corruption size
};

class SDBreader{
public:
   SDBreader();
   ~SDBreader();
   SDBreaderConfig Config;
   COLstring EncryptionKey;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter;   // filters messages by type, date, regex, etc.
   // Current state
   COLstring FilePath;
   COLuint64 Position = 0;
   // Prefetched messages
   int  CacheSize    = 100;
   bool PreFetch     = true;
   bool Fetching     = false;
   bool ManualRemove = false;    // manual removal of messages in Cache. For SDBdequeue
   COLlist<SDBcontent*> Cache;
   COLlist<COLclosure2<int, SDBcontent*>*> Requests;
   COLlist<SDBcorruptRegion> Corruptions;    // a list of corrupted regions after the previous SDBreaderFetch() call
   // We abstract this to enable the reader logic to work on journal file or queue file
   // runs on a worker thread
   COLauto<COLclosure2<bool*, COLstring&>> pNextFileCall;
   COLauto<COLclosure0> pShutdown;
   SDBmessageId StartPosition;
};

// Async interface
// Fetches data as SDBcontent, for journal readers
// dont forget to check SDBreader.Corruptions to handle corrupted regions
void SDBreaderFetch(SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, SDBcontent*>* pCallback);
// Fetches data as SDBmessage, for log or queue readers
// dont forget to check SDBreader.Corruptions to handle corrupted regions
void SDBreaderFetchMessage(SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback);
// Creates an error message from the corrupted regions
COLstring SDBreaderCorruptedRegionErrorMessage(const COLlist<SDBcorruptRegion>& Corruptions, const COLstring& LogFile);
COLstring SDBreaderCorruptedRegionErrorMessage(SDBreader* pReader);
// Removes messages up to and including the specified MessageID from the Cache.
void SDBreaderRemoveMessageFromCache(SDBreader* pReader, SDBid MessageId);

// Creates a new reader that reads all types of messages. It's the caller's responsibility to delete SDBreader
// If StartDay = 0, reads logs from the oldest log file
// If StartDay < the oldest log file, return error SDB_READER_BAD_CONFIG
SDBreader* SDBcreateLogReader    (const COLstring& ComponentId, COLuint64 StartDay, const COLstring& EncryptionKey, COLclosure2<const SDBcontent*, bool*>* pFilter = NULL, const COLstring& LogRootDir = SDBroot());
// Creates a new reader for dequeue purposes. An error is reported if message at Config can't be found in the logs.
SDBreader* SDBcreateDequeueReader(const SDBmessageId& Config, const COLstring& EncKey, bool ManualRemove, const COLstring& LogRootDir);
// Creates a new reader that only reads messages in a component's queue (i.e. SDB_MESSAGE messages).
// If Config = SDBdefaultPosition(), reads messages from the oldest log file.
// If Config.DateTime < the oldest log file, return error SDB_READER_BAD_CONFIG
// If ManualRemove = true, SDBreaderFetchMessage() will keep returning the same SDBmessage until the messages is removed using SDBreaderRemoveMessageFromCache()
SDBreader* SDBcreateQueueReader  (const SDBmessageId& Config, const COLstring& EncryptionKey, bool ManualRemove = false, const COLstring& LogRootDir = SDBroot());
// Creates a new reader that reads and filters messages in a queue. Used for bulk message resubmission.
SDBreader* SDBcreateQueueReader(const SDBfilterInfo& QueueFilter, const COLstring& EncryptionKey, bool ManualRemove = false, const COLstring& LogRootDir = SDBroot());
// Creates a new reader that reads everything in a journal file, including positions and position map
SDBreader* SDBcreateJournalReader(const COLstring& JournalFilePath, const COLstring& EncryptionKey);

// safely destroy a reader, even if it is busy fetching messages
void SDBdeleteReader(SDBreader* pReader, COLclosure0* pCallback = NULL);

// unit test

SDBcontent* SDBunpackContent(const char* pEnvelope, int BufferSize);

void SDBbufferLoaded(int Code, COLauto<COLstring> pBuffer, SDBreader* pReader, SCKloop* pLoop);

int SDBextractMessages(const char* pBuffer, int BufferSize, COLclosure2<const SDBcontent*, bool*>* pFilter, COLlist<SDBcontent*>* pOut, COLlist<SDBcorruptRegion>* pCorruptions = NULL);

int SDBparseEncryptedBuffer(const COLstring& EncryptionKey, COLstring& CiphertextBuffer, COLstring& PlaintextBuffer, COLlist<SDBcorruptRegion>* pCorruptions);
#endif // end of defensive include
