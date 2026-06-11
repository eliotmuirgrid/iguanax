#ifndef __SDB_QUEUE_READER_H__
#define __SDB_QUEUE_READER_H__
// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueueReader
//
// Description:
//
// Reader class for SDBdequeue. A simplified version of SDBreader.
// Reads a chunk of data from log file and extracts the SDB_MESSAGE messages inside.
// Don't call SDBreaderFetchMessage() again before pCallback is called.
// Check the Messages and Corruptions data member in pCallback for messages and corruptions
//
// Author: John Qi
// Date:   Wed Jul 16 08:14:46 PDT 2025
// ---------------------------------------------------------------------------

#include <SDB/SDBid.h>

#include <COL/COLauto.h>
#include <COL/COLclosure6.h>
#include <COL/COLlist.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLstring.h>

#define SDB_END_OF_LOG               -7654
#define SDB_HUGE_MESSAGE             -5432
#define SDB_BAD_QID                  -5430
#define SDB_READER_BAD_CONFIG        -5431   // before beginning of log
#define SDB_READER_BUF_LOAD_SIZE   8388608   // 8 MiB
#define SDB_READER_BUF_MAX_SIZE  134217728   // 128 MiB

class SCKloop;
class SDBmessage;

struct SDBreaderConfig {
   COLstring ComponentId;
   COLuint64 DateTime;
   COLstring LogRootDir;
};

class SDBcorruptRegion {
public:
   SDBcorruptRegion(int iPosition, int iSize) : Position(iPosition), Size(iSize) {}
   COLuint64 Position = 0;    // file position
   int Size = 0;              // corruption size
};

// COLclosure1<SDBreaderResult> pRequest
// class SDBreaderResult {
// public:
//    int Code { 0 };
//    COLstring Message;
// };

class SDBqueueReader{
public:
   SDBqueueReader();
   ~SDBqueueReader();
   SDBreaderConfig Config;
   SDBmessageId StartPosition;
   COLstring EncryptionKey;
   // Current state
   COLstring FilePath;
   COLuint64 Position = 0;
   // shutdown
   bool Fetching = false;
   COLauto<COLclosure0> pShutdown;
   // data
   COLclosure1<int>* pRequest { nullptr };
   COLlist<SDBmessage*> Messages;            // a list of fetched messages
   COLlist<SDBcorruptRegion> Corruptions;    // a list of corrupted regions
};

// Async interface
// Fetches enqueued messages from component logs
// messages and corruptions are APPENDED to pReader's Messages and Corruptions
// Don't call again until pCallback is called with result code
void SDBreaderFetchMessage(SDBqueueReader* pReader, SCKloop* pLoop, COLclosure1<int>* pCallback);

// Creates an error message from the corrupted regions
COLstring SDBreaderCorruptedRegionErrorMessage(SDBqueueReader* pReader);

// Creates a new reader for dequeue purposes.
// It only reads enqueued (SDB_MESSAGE) messages in the component log.
// Reports SDB_READER_BAD_CONFIG if message at StartPosition can't be found in the logs.
// If Config = SDBdefaultPosition(), i.e. beginning of queue, starts from the oldest component log.
SDBqueueReader* SDBcreateDequeueReader(const SDBmessageId& StartPosition, const COLstring& EncryptionKey, const COLstring& LogRootDir);

// safely destroy a reader, even if it is currently busy fetching messages
void SDBdeleteReader(SDBqueueReader* pReader, COLclosure0* pCallback = NULL);

#endif // end of defensive include
