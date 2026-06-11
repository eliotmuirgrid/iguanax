// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBjournalSwap
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Tuesday 25 April 2023 - 05:09PM
// ---------------------------------------------------------------------------

#include <SDB/SDBfile.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBjournalSwap.h>
#include <SDB/SDBjournalThread.h>
#include <SDB/SDBpositionMap.h>
#include <SDB/SDBsystem.h>
#include <SDB/SDBcryptoFile.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

void SDBpositionMapEnvelope(const SDBpositionMap* pPositionMap, COLstring* pBuffer) {
   COL_FUNCTION(SDBpositionMapEnvelope);
   COLstring TempBuffer;
   TempBuffer.setCapacity(SDBpositionMapSize(pPositionMap));
   int Amount = SDBwritePositionMap(pPositionMap, TempBuffer.get_buffer(), TempBuffer.capacity()-1);
   TempBuffer.setSize(Amount);
   SDBenvelopeWrite(pBuffer, TempBuffer);
}

static void SDBcreateJournalFile(const SDBpositionMap* pPositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir, int& FD) {
   COL_FUNCTION(SDBcreateJournalFile);
   COL_VAR(*pPositionMap);
   COLstring PositionData;
   SDBpositionMapEnvelope(pPositionMap, &PositionData);
   FD = SDBopenLogFile(SDBjournalFile(LogRootDir));

   const char* pBuffer = PositionData.get_buffer();
   int Size = PositionData.size();
   COLstring EncryptedData;
   if (!EncryptionKey.is_null()) {
      COL_TRC("Encrypting data");
      SDBencryptAndSerialize(PositionData, EncryptionKey, &EncryptedData);
      pBuffer = EncryptedData.get_buffer();
      Size = EncryptedData.size();
   }
   COL_TRC("Writing data to file");
   int Amount = SDBwriteLogFile(FD, pBuffer, Size);
   COL_VAR(Amount);
   if (getenv(SDB_SYNC_WRITE_ENV_NAME)) {
      COL_TRC("Flush position map to journal file");
      SDBcommitLogFile(FD);
   }
}

// WORKER THREAD
void SDBreplaceJournalFile(int& FD, const SDBpositionMap* pPositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBreplaceJournalFile);
   COL_VAR(pPositionMap->PositionMap.size());
   COLstring JournalFile = SDBjournalFile(LogRootDir);
   if (FILfileExists(JournalFile)) {
      if (-1 != FD) { SDBcloseLogFile(FD); }
      // back up old journal file
      FILrenameFile(JournalFile, SDBoldJournalFile(LogRootDir));
      SDBcreateJournalFile(pPositionMap, EncryptionKey, LogRootDir, FD);
      // remove the backup
      FILremoveWithThrow(SDBoldJournalFile(LogRootDir));
   } else {
      COL_TRC("First startup!");
      if (pPositionMap->PositionMap.size()) {
         SDBcreateJournalFile(pPositionMap, EncryptionKey, LogRootDir, FD);
      }
   }
}

static void SDBjournalSwapped(int* pCount, int Total, COLclosure0* pSwapped) {
   COL_FUNCTION(SDBjournalSwapped);
   COL_VAR2(Total, *pCount);
   if (*pCount == Total) {
      // TODO: probably need to write to at least to 2 log dirs to tolerate network partitions
      COL_TRC("A log dir finished the journal swap. moving on");
      pSwapped->runAndDelete();
   }
   (*pCount)--;
   if (*pCount == 0) {
      COL_TRC("all log dirs finished the journal swap");
      delete pCount;
   }
}

// MAIN THREAD
void SDBswapJournal(SDBsystem* pSystem, COLclosure0* pSwapped) {
   COL_FUNCTION(SDBswapJournal);
   SDBpositionMap* pPosMap = new SDBpositionMap();
   COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>& PosMap = pPosMap->PositionMap;
   auto& ComponentMap = pSystem->ComponentMap;
   for (auto Queue = ComponentMap.cbegin(); Queue != ComponentMap.cend(); Queue++) {
      auto& Consumers = Queue.value().Consumers;
      for (auto Consumer = Consumers.begin(); Consumer != Consumers.end(); Consumer++) {
         COL_VAR(Consumer.value().Position);
         PosMap[Queue.key()][Consumer.key()] = Consumer.value().Position;
      }
   }
   int* pCount = new int(1);
   if (!SDBroot2().is_null()) {     // swap journal in 2nd log dir
      COL_TRC("Swap journal file in " << SDBroot2());
      (*pCount)++;
      SDBpositionMap* pPosMap2 = new SDBpositionMap(*pPosMap);
      // Have to copy pDone to pass the correct *pCount value to SDBjournalSwapped()
      COL_VAR(*pCount);
      COLclosure0* pDone = COLnewClosure0(&SDBjournalSwapped, pCount, *pCount, pSwapped);
      pSystem->JournalTaskQueue2.put(SDBjournalTask(SDB_JOURNAL_SWAP, pDone, NULL, pPosMap2));
   }
   COL_TRC("Swap journal file in " << SDBroot());
   COLclosure0* pDone = COLnewClosure0(&SDBjournalSwapped, pCount, *pCount, pSwapped);
   pSystem->JournalTaskQueue.put(SDBjournalTask(SDB_JOURNAL_SWAP, pDone, NULL, pPosMap));
}

void SDBsetUpJournalFile(const COLhashmap<COLstring, COLhashmap<COLstring, SDBid>>* pPositionMap, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBsetUpJournalFile);
   COLstring FilePath = SDBjournalFile(LogRootDir);
   COL_VAR(FilePath);
   SDBcheckLogDir(FilePath);  // TODO: handle failed to create log dir error
   int FD = -1;
   SDBpositionMap Positions(*pPositionMap);
   SDBcreateJournalFile(&Positions, EncryptionKey, LogRootDir, FD);
   SDBcloseLogFile(FD);
}
