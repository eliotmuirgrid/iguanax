// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBrecovery
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 24 March 2023 - 02:37PM
// ---------------------------------------------------------------------------
#include <SDB/SDBrecovery.h>

#include <SDB/SDBsystem.h>
#include <SDB/SDBreader.h>
#include <SDB/SDBposition.h>
#include <SDB/SDBpositionMap.h>
#include <SDB/SDBpositionMap1.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBflush.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <INFO/INFOusage.h>

#include <FIL/FILosDefs.h>
#include <FIL/FILutils.h>

#include <CFG/CFGmap.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

struct SDBrecoverInfo {
   bool FlushPosition = false;
   SDBmessageId Position;
   COLauto<COLlist<std::shared_ptr<SDBmessage>>> LogMessages = new COLlist<std::shared_ptr<SDBmessage>>();
};

static int SDBloadBuffer(int FD, COLstring* pBuffer) {
   COL_FUNCTION(SDBloadBuffer);
   COLstring& Buffer = *pBuffer;
   if (Buffer.size() > 536870912) { return -EFBIG; }    // breaks if we have a 500 MiB message
   if (Buffer.capacity() - Buffer.size() < 8192) {
      Buffer.setCapacity(Buffer.capacity() * 2);
   }
   int Amount = SDBreadLogFile(FD, Buffer.get_buffer()+Buffer.size(), Buffer.capacity()-Buffer.size()-1);
   COL_VAR(Amount);
   if (Amount > 0) { Buffer.setSize(Amount+Buffer.size()); }
   COL_VAR(Buffer.size());
   return Amount;
}

SDBmessageId SDBreadMessageId(const char* pBuffer, int BufferSize) {
   COL_FUNCTION(SDBreadMessageId);
   SDBmessage Message;
   if (SDBreadMessage(&Message, pBuffer, BufferSize) < 0) {
      COL_ERR("Failed to extract message id");
      return SDBmessageId();
   }
   return Message.Id.MessageId;
}

// Used to determine where in the log file should iguana write the messages recovered from the journal file(s) to
// If the message ID suddenly becomes larger than TargetId, e.g. the TargetId message is corrupted, iguana should
// write the recovered message immediately after the corrupted region.
// TODO: optimize this function so that we overwrite the corrupted region as well
static int SDBfindMessageId(const char* pBuffer, int BufferSize, const SDBmessageId& TargetId, int* pConsumed) {
   COL_FUNCTION(SDBfindMessageId);
   COL_VAR(TargetId);
   int Code = 1; // more data
   const char* pStart = pBuffer;
   do {
      int Invalid = SDBenvelopeInvalid(pBuffer, BufferSize);
      if (Invalid) {
         if (-1 == Invalid) {
            COL_TRC("More data. Parsed all messages in the buffer");
            break;
         }
         const char* pNext = SDBfindNextEnvelope(pBuffer, BufferSize);
         if (pNext) {
            COL_WRN("Corrupted region detected");
            BufferSize -= pNext - pBuffer;
            pBuffer = pNext;
            continue;
         } else {
            COL_WRN("The rest of the buffer is corrupted");
            COL_TRC("Done parsing all messages in the buffer");
            break;
         }
      }
      SDBcontent* pContent = SDBunpackContent(pBuffer, BufferSize);
      if (SDBisMessage(pContent)) {
         SDBmessageId Id = ((SDBmessage*)pContent)->Id.MessageId;
         COL_VAR(Id);
         if (TargetId <= Id) {
            if (TargetId < Id) {
               COL_WRN("Found a message ID that is larger than expected. The target message is likely corrupted");
            } else {    // TargetId == Id
               COL_TRC("Found message " << TargetId);
            }
            Code = 0;
            SDBcontentDestroy(pContent);
            break;
         }
      }
      SDBcontentDestroy(pContent);

      // not a message or not the message we care about, keep searching
      int EnvelopSize = SDBenvelopeNext(pBuffer, BufferSize) - pBuffer;
      BufferSize -= EnvelopSize;
      pBuffer += EnvelopSize;
   } while (true);
   *pConsumed = pBuffer - pStart;
   COL_VAR(*pConsumed);
   return Code;
}

COLint64 SDBgetFilePosition(int FD, const SDBmessageId& TargetId) {
   COL_FUNCTION(SDBgetFilePosition);
   COL_VAR2(FD, TargetId);
   COLstring Buffer;
   Buffer.setCapacity(1048576);
   COLint64 FilePosition = 0;
   while (true) {
      int Amount = SDBloadBuffer(FD, &Buffer);
      if (0 < Amount) { FilePosition += Amount; }
      else if (0 == Amount) { return FilePosition; }
      else { return Amount; }  // negative errno
      int Size = Buffer.size();
      int Code = SDBfindMessageId(Buffer.get_buffer(), Size, TargetId, &Amount);
      if (Code < 0) {
         COL_ERR("Failed to find Message Id");
         return Code;
      } else if (0 == Code) {
         FilePosition -= Size - Amount;
         return FilePosition;
      }
      Buffer.shift(Amount);
      COL_TRC("Truncate buffer to " << Buffer.size());
   }
}

bool SDBfindMessageIdEncrypted(COLstring& Buffer, const COLstring& EncryptionKey, const SDBmessageId& TargetId, int& Consumed, COLstring* pReencrypt) {
   COL_FUNCTION(SDBfindMessageIdEncrypted);
   while (true) {
      COLstring PlaintextBuffer;
      int Amount = SDBparseAndDecrypt(Buffer.data(), Buffer.size(), EncryptionKey, &PlaintextBuffer);
      if (Amount > 0) {    // read and decrypted an enc packet
         COL_TRC("Read and decrypted an encryption packet");
         int Position = 0;
         if (0 == SDBfindMessageId(PlaintextBuffer.data(), PlaintextBuffer.size(), TargetId, &Position)) {  // found msg id in plain buf
            COL_TRC("Found target message at buffer position " << Position);
            if (Position > 0) { *pReencrypt = PlaintextBuffer.substr(0, Position); }
            return true;
         } else {
            COL_TRC("Didn't find target message in current encryption packet. Consume and check the next packet");
            Consumed += Amount;
            Buffer.shift(Amount);
            continue;
         }
      }
      if (-1 == Amount) {  // need more data
         COL_TRC("Need more data to read and decrypt the current encryption packet");
         return false;
      } else {    // corruptions
         COL_TRC("Found corruption");
         int Delta = SDBfindNextEncMarker(Buffer.data(), Buffer.size());
         if (-1 == Delta) {    // buffer is full of corruption, load more from file to find next enc packet
            COL_TRC("The rest of the buffer is corrupted. Load more from file to look for the next encryption packet");
            return false;
         } else {   // consume corruption and load more from file
            COL_TRC("Found the next encryption packet marker after " << Delta << " bytes of corruption");
            Consumed += Delta;      // what happens if the log file ends with corruption
            Buffer.shift(Delta);    // gonna reach EOF before finding the msgID, assume msgID hasn't been written yet
            continue;
         }
      }
   }
}

static COLint64 SDBgetEncryptedFilePosition(int FD, const SDBmessageId& TargetId, const COLstring& EncryptionKey) {
   COL_FUNCTION(SDBgetEncryptedFilePosition);
   int Consumed = 0;
   COLstring Buffer;
   while (true) {
      if (Buffer.capacity() < Buffer.size() + SDB_READER_BUF_LOAD_SIZE) {
         Buffer.setCapacity(Buffer.size() + SDB_READER_BUF_LOAD_SIZE);
      }
      int Amount = SDBreadLogFile(FD, Buffer.get_buffer() + Buffer.size(), SDB_READER_BUF_LOAD_SIZE);
      if (Amount == 0) { COL_TRC("EOF");        return Consumed; }
      if (Amount <  0) { COL_ERR("read error"); return Amount;   }
      Buffer.setSize(Buffer.size() + Amount);
      COLstring ReEncrypt;
      if (!SDBfindMessageIdEncrypted(Buffer, EncryptionKey, TargetId, Consumed, &ReEncrypt)) {
         COL_TRC("Didn't find the message in buffer, load more from file and try again");
         continue;
      }
      if (ReEncrypt.is_null()) {
         COL_TRC("Found message at the beginning of the encryption packet @ " << Consumed);
         return Consumed;
      }
      COL_TRC("Found message in the middle of the encryption packet @ " << Consumed);
      COL_TRC("Need to re-encrypt the messages before the target message and write it back to the log file");
      COLstring EncryptedData;
      SDBencryptAndSerialize(ReEncrypt, EncryptionKey, &EncryptedData);
      SDBseekLogFile(FD, Consumed);
      SDBwriteLogFile(FD, EncryptedData.data(), EncryptedData.size());
      if (getenv(SDB_SYNC_WRITE_ENV_NAME)) {
         COL_TRC("Flush encrypted log file for recovery");
         SDBcommitLogFile(FD);
      }
      return Consumed + EncryptedData.size();
   }
}

// WORKER
void SDBtruncateFile(const COLstring& FilePath, COLint64 Size) {
   COL_FUNCTION(SDBtruncateFile);
   COL_VAR(Size);
   FILrenameFile(FilePath, FilePath + ".orig");
   int FromFD = SDBopenLogFile(FilePath + ".orig");
   int ToFD = SDBopenLogFile(FilePath);
   int BufferSize = 1048576;
   COLstring Buffer;
   Buffer.setCapacity(BufferSize);
   int AmountRead, AmountWritten;
   while (Size) {
      int Amount = BufferSize <= Size ? BufferSize : Size;
      AmountRead    = SDBreadLogFile (FromFD, Buffer.get_buffer(), Amount);
      AmountWritten = SDBwriteLogFile(ToFD,   Buffer.data(),       AmountRead);
      if (AmountWritten != AmountRead) {
         COL_ERR("TODO: handle IO error");
         break;
      }
      Size -= AmountWritten;
   }
   SDBcloseLogFile(FromFD);
   SDBcloseLogFile(ToFD);
   FILremoveWithThrow(FilePath + ".orig");
   COL_VAR2(FilePath, FILfileSize(FilePath));
}

static int SDBprepareLogFile(const COLstring& ComponentId, const SDBrecoverInfo& Info, const COLstring& LogRootDir, const COLstring& EncryptionKey) {
   COL_FUNCTION(SDBprepareLogFile);
   COLstring LogFile = SDBfindLogFile(ComponentId, Info.Position.DateTime, LogRootDir);
   COL_VAR(LogFile);
   if (LogFile.is_null()) { return 0; }
   int FD = SDBopenLogFile(LogFile, false);
   COLint64 Size = 0;
   if (EncryptionKey.is_null()) {
      Size = SDBgetFilePosition(FD, Info.Position);
   } else {
      Size = SDBgetEncryptedFilePosition(FD, Info.Position, EncryptionKey);
   }
   COL_VAR(Size);
   SDBcloseLogFile(FD);
   if (0 > Size) {
      COL_ERR("Failed to seek to start position.");
      return -Size;     // errno
   } else if (FILfileSize(LogFile) > Size) {
      COL_TRC("Truncate file after position " << Size);
      SDBtruncateFile(LogFile, Size);
   }
   return 0;
}

// WORKER
static int SDBflushLogBuffer(const COLstring& ComponentId, const SDBrecoverInfo& Info, const COLhashmap<COLstring, SDBid>& PositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBflushLogBuffer);
   COL_VAR2(ComponentId, Info.Position);
   if (Info.LogMessages->size()) {
      int Error = SDBprepareLogFile(ComponentId, Info, LogRootDir, EncryptionKey);
      if (Error) { return Error; }
   }
   SDBlogTaskData Data;
   Data.pMessages = Info.LogMessages;
   Data.pPositions = new COLhashmap<COLstring, SDBid>(PositionMap);
   Data.FlushPosition = PositionMap.size() > 0;
   COL_VAR4(ComponentId, Data.pMessages->size(), Data.pPositions->size(), Data.FlushPosition);
   SDBlogHandle Handle;
   SDBflushLogMessages(Handle, Data, EncryptionKey, LogRootDir);
   return 0;
}

class SDBflushLogBuffersArgs {
public:
   int Code;
   SCKloop* pLoop;
   COLstring EncryptionKey;
   COLmap<COLstring, SDBrecoverInfo>* pLogBuffers;
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap;
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback;
   COLstring LogRootDir;
};

// WORKER
static void SDBflushLogBuffers(SDBflushLogBuffersArgs Args) {
   COL_FUNCTION(SDBflushLogBuffers);
   int Code = Args.Code;
   SCKloop* pLoop = Args.pLoop;
   COLstring EncryptionKey = Args.EncryptionKey;
   COLmap<COLstring, SDBrecoverInfo>* pLogBuffers = Args.pLogBuffers;
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap = Args.pPositionMap;
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = Args.pCallback;
   COLstring LogRootDir = Args.LogRootDir;

   int Result = 0;
   for (auto i = pLogBuffers->cbegin(); i != pLogBuffers->cend(); i++) {
      Result = SDBflushLogBuffer(i.key(), i.value(), (*pPositionMap)[i.key()], EncryptionKey, LogRootDir);
      if (Result) { break; }
   }
   delete pLogBuffers;
   // Code may report journal parsing error, Result may report log flushing error
   // If we have parsing and flushing errors, report parsing error
   if (Code == 0 && Result != 0) { Code = Result; }
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>::runAndDelete, Code, pPositionMap));
}

static bool SDBhandleErrors(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, COLmap<COLstring, SDBrecoverInfo>* pLogBuffers, SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBhandleErrors);
   COL_VAR2(Code, pLogBuffers->size());
   if (Code == 0) { return false; }    // no error, keep processing the journal
   COLstring EncryptionKey = pReader->EncryptionKey;
   SDBdeleteReader(pReader);
   if (SDB_END_OF_LOG == Code) {
      COL_TRC("Successfully parsed data in the journal file");
      Code = 0;
   }
   // write everything we have to log files, even if we run into an error while parsing the journal file
   if (pLogBuffers->size() > 0) {   // journal has position map, messages, and positions
      SDBIlogService("Saving messages found in journal log files...", "#startup");
      SDBflushLogBuffersArgs ClosureArgs { Code, pLoop, EncryptionKey, pLogBuffers, pPositionMap, pCallback, LogRootDir };
      pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBflushLogBuffers, ClosureArgs));
   } else if (0 == Code) {          // journal has position map only
      delete pLogBuffers;
      pCallback->runAndDelete(Code, pPositionMap);
   } else {                         // journal corrupted and we didn't find any messages
      COL_ERR("Encountered parsing error " << Code);
      // corruption may start after the position map at beginning of file, so return the position map
      pCallback->runAndDelete(Code, pPositionMap);
   }
   return true;
}

static void SDBprocessPositionMap(SDBcontent* pContent, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap) {
   COL_FUNCTION(SDBprocessPositionMap);
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pMap = NULL;
   switch (pContent->Type) {
      case SDB_POSMAP:  pMap = &((SDBpositionMap *)pContent)->PositionMap; break;
      case SDB_POSMAP1: pMap = &((SDBpositionMap1*)pContent)->PositionMap; break;
      default:          COLASSERT(false);                                  break;
   }
   for (auto i = pMap->cbegin(); i != pMap->cend(); i++) {
      (*pPositionMap)[i.key()] = i.value();
   }
   SDBcontentDestroy(pContent);
}

static void SDBprocessPosition(SDBcontent* pContent, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, COLmap<COLstring, SDBrecoverInfo>* pLogBuffers) {
   COL_FUNCTION(SDBprocessPosition);
   SDBposition* pPosition = (SDBposition*)pContent;
   const SDBid& Position = pPosition->Position;
   COL_VAR(*pPosition);
   const COLstring& QueueId = Position.MessageId.ComponentId;
   const COLstring& ConsumerId = pPosition->ConsumerId;
   // even if Position is delete-consumer, we still write it to log file for record keeping.
   // the corresponding consumer will be removed in SDBcomponentsInitialized()
   COL_TRC("Update position for queue consumer " << QueueId << " -> " << ConsumerId);
   (*pPositionMap)[QueueId][ConsumerId] = Position;
   // set the flush position flag
   (*pLogBuffers)[QueueId].FlushPosition = true;
   SDBcontentDestroy(pContent);
}

static void SDBprocessMessage(SDBcontent* pContent, COLmap<COLstring, SDBrecoverInfo>* pLogBuffers) {
   COL_FUNCTION(SDBprocessMessage);
   SDBmessage* pMessage = (SDBmessage*)pContent;
   SDBrecoverInfo& Info = (*pLogBuffers)[pMessage->Id.MessageId.ComponentId];
   if (0 == Info.Position.DateTime) { Info.Position = pMessage->Id.MessageId; }
   Info.LogMessages->push_back(std::shared_ptr<SDBmessage>(pMessage));
}

static int SDBprocessContent(SDBcontent* pContent, COLmap<COLstring, SDBrecoverInfo>* pLogBuffers, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap) {
   COL_FUNCTION(SDBprocessContent);
   COL_VAR(*pContent);
   switch (pContent->Type) {
   case SDB_POSITION:
      SDBprocessPosition(pContent, pPositionMap, pLogBuffers);
      break;
   case SDB_POSMAP:
   case SDB_POSMAP1:
      if (pLogBuffers->size() > 0) {   // found message(s) before position map, which is not how we generate journal files
         COL_ERR("Expected a Position Map at the beginning of the file");
         return -2;
      } else {
         SDBprocessPositionMap(pContent, pPositionMap);
         break;
      }
   default:
      // avoid list message types here for better maintainability
      if (SDBisMessage(pContent)) {
         SDBprocessMessage(pContent, pLogBuffers);
      } else {
         COL_ERR("Unexpected content type: " << pContent->Type);
         return -3;
      }
   }
   return 0;
}

static void SDBprocessJournalContent(int Code, SDBcontent* pContent, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, COLmap<COLstring, SDBrecoverInfo>* pLogBuffers, SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, COLstring LogRootDir) {
   COL_FUNCTION(SDBprocessJournalContent);
   if (SDBhandleErrors(Code, pPositionMap, pLogBuffers, pReader, pLoop, pCallback, LogRootDir)) { return; }
   int Result = SDBprocessContent(pContent, pLogBuffers, pPositionMap);
   if (Result) {
      pCallback->runAndDelete(Result, (COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*)NULL);
   } else {
      SDBreaderFetch(pReader, pLoop, COLnewClosure2(&SDBprocessJournalContent, pPositionMap, pLogBuffers, pReader, pLoop, pCallback, LogRootDir));
   }
}

// MAIN
void SDBrecoverJournal(COLstring JournalFile, COLstring EncryptionKey, SCKloop* pLoop, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, COLstring LogRootDir) {
   COL_FUNCTION(SDBrecoverJournal);
   SDBreader* pReader = SDBcreateJournalReader(JournalFile, EncryptionKey);
   COLmap<COLstring, SDBrecoverInfo>* pLogBuffers = new COLmap<COLstring, SDBrecoverInfo>();
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap = new COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>();
   SDBIlogService("Processing journal file " + JournalFile, "#startup");
   // All journal files begin with the queue position map (SDB_POSMAP), even if it's empty
   SDBreaderFetch(pReader, pLoop, COLnewClosure2(&SDBprocessJournalContent, pPositionMap, pLogBuffers, pReader, pLoop, pCallback, LogRootDir));
}

// WORKER
// called after successful recovery of journal.log or journal.old.log
// 1. journal.log only                             - move to journal.old.log
// 2. journal.old.log only                         - no-op, already called journal.old.log
// 3. journal.log fails, journal.old.log success   - no-op, already called journal.old.log
// moves journal.log out of the way so it can be recrated after position map reconstruction from logs
// journal.old.log will be removed after journal.log is recreated
static void SDBmoveJournalFile(COLstring LogRootDir) {
   COL_FUNCTION(SDBmoveJournalFile);
   COLstring CurJournal = SDBjournalFile(LogRootDir);
   COLstring OldJournal = SDBoldJournalFile(LogRootDir);
   // at least one of the journal file must exist to trigger journal recovery
   COLASSERT(FILfileExists(CurJournal) || FILfileExists(OldJournal));
   if (FILfileExists(CurJournal) && !FILfileExists(OldJournal)) {
      FILrenameFile(CurJournal, OldJournal);
   }
}

class SDBpostRecoveryArgs {
public:
   int Code;
   SCKloop* pLoop;
   const CFGmap* pConfigMap;
   COLstring EncryptionKey;
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap;
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback;
   COLstring LogRootDir;
};

// WORKER
static void SDBpostRecovery(SDBpostRecoveryArgs Args) {
   COL_FUNCTION(SDBpostRecovery);
   int Code = Args.Code;
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap = Args.pPositionMap;
   SCKloop* pLoop = Args.pLoop;
   const CFGmap* pConfigMap = Args.pConfigMap;
   COLstring EncryptionKey = Args.EncryptionKey;
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback = Args.pCallback;
   COLstring LogRootDir = Args.LogRootDir;
   if (Code == 0) {
      SDBmoveJournalFile(LogRootDir);
      pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>::runAndDelete, 0, pPositionMap));
      return;
   }
   COLstring CurJournal = SDBjournalFile(LogRootDir);
   COLstring OldJournal = SDBoldJournalFile(LogRootDir);
   if (FILfileExists(CurJournal)) {
      COL_ERR("Failed to recover journal.log");
      FILrenameFile(CurJournal, CurJournal + ".corrupted." + COLintToString(time(NULL)));
      if (FILfileExists(OldJournal)) {
         COL_ERR("Switch to journal.old.log recovery");
         pLoop->post(COLnewClosure0(&SDBrecoverJournal, OldJournal, EncryptionKey, pLoop, pCallback, LogRootDir));
         return;
      }
   }
   if (FILfileExists(OldJournal)) {
      COL_ERR("Failed to recover journal.old.log");
      FILrenameFile(OldJournal, OldJournal + ".corrupted." + COLintToString(time(NULL)));
   }
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>::runAndDelete, Code, pPositionMap));
}

static void SDBjournalRecovered(int Code, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, SCKloop* pLoop, const CFGmap* pConfigMap, COLstring EncryptionKey, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback,  COLstring LogRootDir) {
   COL_FUNCTION(SDBjournalRecovered);
   // Hop over to worker thread to perform file io to finish up journal recovery
   SDBpostRecoveryArgs ClosureArgs { Code, pLoop, pConfigMap, EncryptionKey, pPositionMap, pCallback, LogRootDir };
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBpostRecovery, ClosureArgs));
}

// WORKER
static bool SDBhasEnoughDiskSpace(const COLstring& LogRootDir) {
   COL_FUNCTION(SDBhasEnoughDiskSpace);
   COLuint64 SpaceNeeded = 0;
   COLstring JournalFile = SDBjournalFile(LogRootDir);
   if (FILfileExists(JournalFile)) {
      SpaceNeeded += FILfileSize(JournalFile);
   }
   JournalFile = SDBoldJournalFile(LogRootDir);
   if (FILfileExists(JournalFile)) {
      SpaceNeeded += FILfileSize(JournalFile);
   }
   // check disk
   COLuint64 DiskFree = INFOdiskStats(LogRootDir).Free;
   COL_VAR2(DiskFree, SpaceNeeded);
   return DiskFree >= SpaceNeeded;
}

// WORKER
static void SDBgetJournalFile(SCKloop* pLoop, const CFGmap* pConfigMap, COLstring EncryptionKey, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, COLstring LogRootDir) {
   COL_FUNCTION(SDBgetJournalFile);
   if (!SDBhasEnoughDiskSpace(LogRootDir)) {
      COL_ERR("Not enough disk space for journal recovery");
      COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap = NULL;
      pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>::runAndDelete, SDB_RECOVER_NO_SPACE, pPositionMap));
      return;
   }
   COLstring JournalFile = SDBjournalFile(LogRootDir);
   if (!FILfileExists(JournalFile)) {
      JournalFile = SDBoldJournalFile(LogRootDir);
      if (!FILfileExists(JournalFile)) {
         COL_TRC("Journal files not found");
         COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap = new COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>();
         pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>::runAndDelete, SDB_RECOVER_NO_JOURNAL, pPositionMap));
         return;
      }
   }
   COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pRecovered = COLnewClosure2(&SDBjournalRecovered, pLoop, pConfigMap, EncryptionKey, pCallback, LogRootDir);
   pLoop->post(COLnewClosure0(&SDBrecoverJournal, JournalFile, EncryptionKey, pLoop, pRecovered, LogRootDir));
}

// MAIN
// After a successful recovery:
// 1. SDBmoveJournalFile() rename journal.log to journal.old.log
// 2. Iguana scans the logs to reconstruct the queue position map
// 3. create journal.log to contain the queue position map
// 4. journal.old.log is removed
// As a result, we have a few cases to deal with:
// * journal.log only                  - normal or crash after 4     - recover journal.log
// * journal.old.log only              - crash between 1 and 3       - recover journal.old.log
// * journal.log and journal.old.log   - crash between 3 and 4       - recover journal.log then journal.old.log only on failure
// Algo:
//  * if journal.log exists, recover it
//  *    if successful, we don't care about journal.old.log even if it exists
//  *    else we need to recover journal.old.log
//  * else if journal.old.log exists, recover journal.old.log instead
//  * else we don't have any journal file, probably initial start after installation or user deleted the journal files.
void SDBrecover(SCKloop* pLoop, const CFGmap* pConfigMap, const COLstring& EncryptionKey, COLclosure2<int, COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>*>* pCallback, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBrecover);
   // determine which journal file to recover in a worker thread because we need FILfileExists()
   pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBgetJournalFile, pLoop, pConfigMap, EncryptionKey, pCallback, LogRootDir));
   COL_TRC("Recovery scheduled on worker thread");
}
