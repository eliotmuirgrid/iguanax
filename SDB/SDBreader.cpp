// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBreader
//
// Description:
//
// Implementation
//
// Pros and Cons:
//    The worker thread only loads the buffer. Most of the fetch from file
//    operation happen on the main thread. As a result, the code is relative
//    easy to follow.
//    Main thread has to server from cache and process data read from file, so
//    serving from cache is slower.
//
// Author: John Qi
// Date:   Tuesday 28 February 2023 - 09:30AM
// ---------------------------------------------------------------------------
#include <cstdio>

#include <SDB/SDBreader.h>

#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBepochTime.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter.h>    // for SDBfilterMatch and SDBqueueFilter
#include <SDB/SDBmessage.h>
#include <SDB/SDBnumber.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBenvVar.h>

#include <SDB/SDBcapi.h>

#include <SCK/SCKloop.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBreaderFetchFromFile(SDBreader* pReader, SCKloop* pLoop);

void SDBreaderDefaultNextFileCall(bool* pSuccess, COLstring& Filename) {
   COL_FUNCTION(SDBreaderDefaultNextFileCall);
   *pSuccess = false;
}

SDBreader::SDBreader() {
   // set default pNextFileCall
   pNextFileCall = COLnewClosure2(&SDBreaderDefaultNextFileCall);
}

SDBreader::~SDBreader() {
   while (Cache.size()) {
      SDBcontentDestroy(Cache.front());
      Cache.pop_front();
   }
}

static void SDBdestroyReader(SDBreader* pReader, COLclosure0* pCallback) {
   COL_FUNCTION(SDBdestroyReader);
   delete pReader;
   COL_TRC("Reader destroyed");
   if (pCallback) {
      pCallback->runAndDelete();
   }
}

// MAIN
static void SDBreaderServeRequest(SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderServeRequest);
   for (int i = 0; i < pReader->Cache.size(); ++i) {
      if (pReader->Requests.size() == 0) {
         COL_TRC("Done serving requests");
         return;
      }
      COL_TRC("Serving request " << i + 1);
      SDBcontent* pEntry = pReader->Cache.front();
      COL_VAR(*pEntry);
      COLclosure2<int, SDBcontent*>* pCallback = pReader->Requests.front();
      // use timer to solve the deep nesting problem if pCallback calls SDBreaderFetch()
      // pCallback->runAndDelete(Message);
      // I believe addTimer() is more efficient, but it breaks in unit tests (run on worker threads)
      pLoop->addTimer(COLnewClosure0(pCallback, &COLclosure2<int, SDBcontent*>::runAndDelete, 0, pEntry));
      // pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, SDBcontent*>::runAndDelete, 0, pEntry));
      if (!pReader->ManualRemove) {
         pReader->Cache.pop_front();
      }
      pReader->Requests.pop_front();
   }
}

// MAIN
static void SDBreaderServeError(SDBreader* pReader, SCKloop* pLoop, int ErrorCode) {
   COL_FUNCTION(SDBreaderServeError);
   COL_TRC("Reporting errors");
   while (pReader->Requests.size()) {
      COLclosure2<int, SDBcontent*>* pCallback = pReader->Requests.front();
      pReader->Requests.pop_front();
      // I believe addTimer() is more efficient, but it breaks in unit tests (run on worker threads)
      pLoop->addTimer(COLnewClosure0(pCallback, &COLclosure2<int, SDBcontent*>::runAndDelete, ErrorCode, (SDBcontent*)NULL));
      // pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, SDBcontent*>::runAndDelete, ErrorCode, (SDBcontent*)NULL));
   }
}

static bool SDBdequeueReaderCheckStartPosition(SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBdequeueReaderCheckStartPosition);
   COL_VAR(pReader->StartPosition);
   if (pReader->StartPosition.ComponentId.is_null()) {
      COL_TRC("Reader StartPosition not set. all good");
      return false;
   }
   if (SDBisDefaultPosition(pReader->StartPosition)) {
      COL_TRC("Special beginning of queue postion. all good");
      return false;
   }
   if (0 == pReader->Cache.size()) {
      COL_TRC("nothing in cache to check");
      return false;
   }
   SDBmessage* pMessage = (SDBmessage*)pReader->Cache.front();
   COL_VAR(pMessage->Id);
   if (pMessage->Id.MessageId == pReader->StartPosition) {
      COL_TRC("Found StartPosition. all good");
      // remove message because it had been dequeued already
      delete pMessage;
      pReader->Cache.pop_front();
      // clear StartPosition so that we don't run this function again
      pReader->StartPosition.reset();
      return false;
   }
   COL_TRC("StartPosition not found. report error");
   SDBreaderServeError(pReader, pLoop, SDB_READER_BAD_CONFIG);
   return true;
}

// MAIN
static void SDBreaderFetchFromCache(int Code, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromCache);
   COL_VAR2(pReader->Cache.size(), pReader->Requests.size());
   // serve what we have in cache
   if (SDBdequeueReaderCheckStartPosition(pReader, pLoop)) { return; }
   SDBreaderServeRequest(pReader, pLoop);
   if (pReader->Requests.size() == 0) {
      COL_TRC("We have served all requests");
      COL_VAR4(Code, pReader->PreFetch, pReader->Fetching, pReader->Cache.size());
      if (!Code && !pReader->Fetching && pReader->PreFetch && (pReader->Cache.size() < pReader->CacheSize/10)) {
         COL_TRC("Preemptively fetch more messages");
         SDBreaderFetchFromFile(pReader, pLoop);
      }
      return;
   }
   COL_TRC("We have more requests to serve");
   if (Code) {
      COL_VAR(Code);
      SDBreaderServeError(pReader, pLoop, Code);
      return;
   }
   if (!pReader->Fetching) {
      COL_TRC("Need to fetch more messages");
      SDBreaderFetchFromFile(pReader, pLoop);
   }
}

SDBcontent* SDBunpackContent(const char* pEnvelope, int BufferSize) {
   COL_FUNCTION(SDBunpackContent);
   COL_VAR(BufferSize);
   COLint64 Type = SDBenvelopeContentType(pEnvelope, BufferSize);
   COL_VAR(SDBtypeAsString((SDBtype)Type));
   SDBcontent* pContent = SDBcontentCreate(Type);
   int Amount = SDBreadContent(pContent, SDBenvelopeContent(pEnvelope, BufferSize), SDBenvelopeContentSize(pEnvelope, BufferSize));
   COL_VAR(*pContent);
   return pContent;
}

static void SDBfilterContent(SDBcontent* pContent, COLclosure2<const SDBcontent*, bool*>* pFilter, COLlist<SDBcontent*>* pOut) {
   COL_FUNCTION(SDBfilterContent);
   if (pFilter) {
      // NOTE: We read the message and then filter. It is more efficient if we only read messages that are accepted by the filter.
      bool Allowed = false;
      pFilter->run(pContent, &Allowed);
      if (!Allowed) {
         COL_TRC("Message " << *pContent << " is rejected by the filter.");
         SDBcontentDestroy(pContent);
         return;
      }
   }
   pOut->push_back(pContent);
}

// MAIN
int SDBextractMessages(const char* pBuffer, int BufferSize, COLclosure2<const SDBcontent*, bool*>* pFilter, COLlist<SDBcontent*>* pOut, COLlist<SDBcorruptRegion>* pCorruptions) {
   COL_FUNCTION(SDBextractMessages);
   const char* pStart = pBuffer;
   int AmountRead = 0;
   do {
      COL_VAR(BufferSize);
      int Invalid = SDBenvelopeInvalid(pBuffer, BufferSize);
      if (Invalid) {
         if (-1 == Invalid) {
            COL_TRC("More data. Parsed all messages in the buffer");
            break;
         }
         const char* pNext = SDBfindNextEnvelope(pBuffer, BufferSize);
         if (pNext) {
            COL_WRN("Corrupted region detected");
            if (pCorruptions) { pCorruptions->push_back(SDBcorruptRegion(pBuffer - pStart, pNext - pBuffer)); }  // position relative to beginning of buffer
            BufferSize -= pNext - pBuffer;
            pBuffer = pNext;
            continue;
         } else {
            COL_WRN("The rest of the buffer is corrupted");
            if (pCorruptions) { pCorruptions->push_back(SDBcorruptRegion(pBuffer - pStart, BufferSize)); }     // position relative to beginning of buffer
            COL_TRC("Done parsing all messages in the buffer");
            break;
         }
      }
      SDBcontent* pContent = SDBunpackContent(pBuffer, BufferSize);
      SDBfilterContent(pContent, pFilter, pOut);
      int EnvelopSize = SDBenvelopeNext(pBuffer, BufferSize) - pBuffer;
      COL_VAR(EnvelopSize);
      BufferSize -= EnvelopSize;
      pBuffer += EnvelopSize;
      AmountRead += EnvelopSize;
      COL_VAR(AmountRead);
      COL_TRC("After increment");
   } while (true);
   return AmountRead;
}

// MAIN
static void SDBgotNextFile(bool Success, COLstring NextFilePath, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBgotNextFile);
   pReader->Fetching = false;
   if (pReader->pShutdown) {
      COL_WRN("Shutdown requested. Shutting down now without processing the next file");
      return pReader->pShutdown->run();
   }
   if (Success) {
      COL_TRC("Next File = " << NextFilePath);
      pReader->Position = 0;
      pReader->FilePath = NextFilePath;
      SDBreaderFetchFromFile(pReader, pLoop);
   } else {
      COL_TRC("Reached End of Log!");
      SDBreaderFetchFromCache(SDB_END_OF_LOG, pReader, pLoop);
   }
}

// WORKER
static void SDBgetNextFile(COLclosure2<bool*, COLstring&>* pNextFileFunction, COLstring FilePath, SCKloop* pLoop, COLclosure2<bool, COLstring>* pCallback) {
   COL_FUNCTION(SDBgetNextFile);
   bool Success = false;
   pNextFileFunction->run(&Success, FilePath);
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<bool, COLstring>::runAndDelete, Success, FilePath));
}

// MAIN
static void SDBreaderFetchFromNextFile(SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromNextFile);
   COL_TRC("Current File = " << pReader->FilePath);
   pReader->Fetching = true;
   // SDBreader.pNextFileCall is likely to involve file I/O, run it on a worker thread.
   COLclosure2<bool, COLstring>* pCallback = COLnewClosure2(&SDBgotNextFile, pReader, pLoop);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(SDBgetNextFile, pReader->pNextFileCall.get(), pReader->FilePath, pLoop, pCallback));
}

static void SDBconvertToAbsoluteCorruptionPositions(COLuint64 FilePosition, COLlist<SDBcorruptRegion>* pCorruptions) {
   COL_FUNCTION(SDBconvertToAbsoluteCorruptionPositions);
   for (auto Corruption = pCorruptions->begin(); Corruption != pCorruptions->end(); Corruption++) {
      Corruption->Position += FilePosition;
   }
}

static void SDBbufferLoadWorker(SDBreaderConfig Config, COLstring* pFilename, COLuint64 Position, SCKloop* pLoop, COLclosure2<int, COLauto<COLstring>>* pCallback, COLauto<COLstring> pBuffer);

// MAIN
void SDBbufferLoaded(int Code, COLauto<COLstring> pBuffer, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBbufferLoaded);
   pReader->Fetching = false;
   if (pReader->pShutdown) {
      COL_WRN("Shutdown requested. Shutting down now without processing the loaded buffer");
      pReader->pShutdown->run();
      return;
   }
   if (!pBuffer->is_null()) {
      pReader->Corruptions.clear();    // empty corrupted region list as they should have been processed after each SDBreaderFetch() call
      int Amount = SDBextractMessages(pBuffer->get_buffer(), pBuffer->size(), pReader->pFilter.get(), &(pReader->Cache), &pReader->Corruptions);
      if (pReader->Corruptions.size()) {
         // positions in pReader->Corruptions are relative to the beginning of the buffer,
         // convert them to absolute file positions.
         SDBconvertToAbsoluteCorruptionPositions(pReader->Position, &pReader->Corruptions);
      }
      if (0 == Amount) {   // failed to extract any messages
         if (0 == Code && 0 == pBuffer->size() % SDB_READER_BUF_LOAD_SIZE) {
            // buffer is full, but failed to extract any messages
            COL_VAR(pBuffer->size());
            if (pBuffer->size() < SDB_READER_BUF_MAX_SIZE) {
               COL_TRC("We have a large message, load more into buffer");
               COLclosure2<int, COLauto<COLstring>>* pComplete = COLnewClosure2(&SDBbufferLoaded, pReader, pLoop);
               pReader->Position += SDB_READER_BUF_LOAD_SIZE;
               COL_VAR(pReader->Position);
               auto Task = COLnewClosure0(&SDBbufferLoadWorker, pReader->Config, &pReader->FilePath, pReader->Position, pLoop, pComplete, pBuffer);
               pLoop->threadPool()->scheduleTask(Task);
               return;
            }
            COL_ERR("Huge log message or corrupted log message size");
            Code = SDB_HUGE_MESSAGE;
         }
         if (EOF == Code) {
            // buffer not empty, reached EOF, failed to extract any messages, add to corruption list
            COL_ERR("Done reading current file. Found partial/corrupted message at EOF. Try reading the next file");
            if (pReader->Corruptions.size() == 0) {
               COL_TRC("Record corruption");
               pReader->Corruptions.push_back(SDBcorruptRegion(pReader->Position, pBuffer->size()));
            }
            // read next log file if applicable
            SDBreaderFetchFromNextFile(pReader, pLoop);
            return;
         }
      } else {    // extracted messages
         // we always load SDB_READER_BUF_LOAD_SIZE bytes at a time, but the buffer can hold more when we have large messages
         // pReader->Position is incremented each time we extended the buffer, so we don't want to double-count here.
         // e.g.
         // buffer size could be SDB_READER_BUF_LOAD_SIZE + SDB_READER_BUF_LOAD_SIZE + SDB_READER_BUF_LOAD_SIZE for a large message
         // Amount could be SDB_READER_BUF_LOAD_SIZE + SDB_READER_BUF_LOAD_SIZE + 256 after extracting the large message
         // pReader->Position is currently at SDB_READER_BUF_LOAD_SIZE + SDB_READER_BUF_LOAD_SIZE
         // we only need to advance position by 256
         int Delta = Amount % SDB_READER_BUF_LOAD_SIZE;
         if (Delta == 0) { Delta = SDB_READER_BUF_LOAD_SIZE; }
         pReader->Position += Delta;
         COL_VAR(pReader->Position);
         COL_TRC("Fetched all messages!");
         pReader->CacheSize = pReader->Cache.size();
         COL_VAR(pReader->CacheSize);
         // Code could be EOF here. However, we extracted some messages and serving them will take some time.
         // Note that there may be more fetch requests queued than messages available, and there could be more
         // messages added to the file by the time we finish serving. Set Code = 0 here to try again.
         // Note: even if we didn't extract any message because of the filter, there may be more log files to read.
         Code = 0;
      }
   } else { // pBuffer->is_null()
      if (EOF == Code) {
         COL_TRC("Done reading current file. Try reading the next file");
         SDBreaderFetchFromNextFile(pReader, pLoop);
         return;
      } else { // open, seek, or read errors
         COLstring ErrMsg = "Failed to read data from file '" + pReader->FilePath + "'. Code = " + COLintToString(Code);
         COL_ERR(ErrMsg);
         // kind of pointless to log error in source component, but we don't have to consumer id here
         // SDBIlogComponent(ErrMsg, pReader->Config.ComponentId, "#error");
      }
   }
   SDBreaderFetchFromCache(Code, pReader, pLoop);
}

static bool SDBsetFilename(SDBreaderConfig Config, COLstring* pFilename, int* pError) {
   COL_FUNCTION(SDBsetFilename);
   COL_VAR3(Config.ComponentId, Config.DateTime, Config.LogRootDir);
   COLstring& Filename = *pFilename;
   if (!Filename.is_null()) { return true; }
   if (0 == Config.DateTime) {
      COL_TRC("Special beginning of time position, find the oldest or EOF");
      Filename = SDBfindOldest(SDBdir(Config.ComponentId, Config.LogRootDir));
      COL_VAR(Filename);
      if (Filename.is_null()) {
         COL_TRC("Component " << Config.ComponentId << " has no logs");
         *pError = EOF;
         return false;
      }
   } else {
      COL_TRC("Regular position, find the file or SDB_READER_BAD_CONFIG");
      Filename = SDBfindLogFile(Config.ComponentId, Config.DateTime, Config.LogRootDir);
      COL_VAR(Filename);
      if (Filename.is_null()) {
         COL_TRC("Can't find log file for component " << Config.ComponentId << " @ " << Config.DateTime);
         *pError = SDB_READER_BAD_CONFIG;
         return false;
      }
   }
   return true;
}

// WORKER
static void SDBbufferLoadWorker(SDBreaderConfig Config, COLstring* pFilename, COLuint64 Position, SCKloop* pLoop, COLclosure2<int, COLauto<COLstring>>* pCallback, COLauto<COLstring> pBuffer) {
   COL_FUNCTION(SDBbufferLoadWorker);
   if (pBuffer) {
      COL_TRC("Extend buffer");
      // we should only reach here if we came from SDBbufferLoaded's "message larger than SDB_READER_BUF_LOAD_SIZE" case
      // feel free to remove the assert if this is also needed elsewhere
      COLASSERT(0 == pBuffer->size() % SDB_READER_BUF_LOAD_SIZE);
      pBuffer->setCapacity(pBuffer->size() + SDB_READER_BUF_LOAD_SIZE);
   } else {
      COL_TRC("Init buffer");
      pBuffer = new COLstring();
      pBuffer->setCapacity(SDB_READER_BUF_LOAD_SIZE);
   }
   int Error = 0;
   if (!SDBsetFilename(Config, pFilename, &Error)) {
      // Error is either EOF or SDB_READER_BAD_CONFIG
      pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLauto<COLstring>>::runAndDelete, Error, pBuffer));
      return;
   }
   int Code = 0, FD = -1;
   do {
      COLint64 Result = -1;
      COL_VAR(*pFilename);
      FD = SDBopenLogFile(*pFilename);
      if (FD < 0)     { COL_ERR("open error"); Code = -FD;     break; }
      Result = SDBseekLogFile(FD, Position);
      if (Result < 0) { COL_ERR("seek error"); Code = -Result; break; }
      Result = SDBreadLogFile(FD, pBuffer->get_buffer() + pBuffer->size(), SDB_READER_BUF_LOAD_SIZE);
      if (Result < 0) { COL_ERR("read error"); Code = -Result; break; }
      if (!Result)    { COL_TRC("EOF");        Code = EOF;     break; }
      pBuffer->setSize(Result + pBuffer->size());
      if (Result < SDB_READER_BUF_LOAD_SIZE) {
         COL_TRC("reached EOF after reading " << Result << " bytes");
         Code = EOF;
      }
   } while (false);
   SDBcloseLogFile(FD);
   COL_VAR(pBuffer->size());
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<int, COLauto<COLstring>>::runAndDelete, Code, pBuffer));
}

void SDBbufferLoadedEncrypted(int Code, COLauto<COLstring> pBuffer, int Consumed, COLauto<COLlist<SDBcorruptRegion>> pCorruptions, SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBbufferLoadedEncrypted);
   pReader->Fetching = false;
   if (pReader->pShutdown) {
      COL_WRN("Shutdown requested. Shutting down now without processing the loaded buffer");
      pReader->pShutdown->run();
      return;
   }
   if (!pBuffer->is_null()) {
      pReader->Corruptions.clear();    // empty corrupted region list as they should have been processed after each SDBreaderFetch() call
      if (pCorruptions->size()) {
         // copy corruptions to pReader
         for (auto i = pCorruptions->begin(); i != pCorruptions->end(); i++) {
            pReader->Corruptions.push_back(*i);
         }
         // positions in pCorruptions are relative to the beginning of the buffer,
         // convert them to absolute file positions.
         SDBconvertToAbsoluteCorruptionPositions(pReader->Position, &pReader->Corruptions);
      }
      // pBuffer contains plaintext logs and they are guaranteed to have no corruption (thanks to AES-GCM)
      int Amount = SDBextractMessages(pBuffer->get_buffer(), pBuffer->size(), pReader->pFilter.get(), &(pReader->Cache), &pReader->Corruptions);
      COLASSERT(pReader->Corruptions.size() == 0);    // no corruption
      COLASSERT(Amount > 0);  // should be able to extract at least one log
      pReader->Position += Consumed;   // advance file position for the current encrypted log file
      COL_VAR(pReader->Position);
      COL_TRC("Fetched all messages!");
      pReader->CacheSize = pReader->Cache.size();
      COL_VAR(pReader->CacheSize);
      // Code could be EOF here. However, we extracted some messages and serving them will take some time.
      // Note that there may be more fetch requests queued than messages available, and there could be more
      // messages added to the file by the time we finish serving. Set Code = 0 here to try again.
      // Note: even if we didn't extract any message because of the filter, there may be more log files to read.
      Code = 0;
   } else { // pBuffer->is_null()
      if (EOF == Code) {
         COL_TRC("Done reading current file. Try reading the next file");
         SDBreaderFetchFromNextFile(pReader, pLoop);
         return;
      } else { // open, seek, or read errors
         COLstring ErrMsg = "Failed to read data from file '" + pReader->FilePath + "'. Code = " + COLintToString(Code);
         COL_ERR(ErrMsg);
         // kind of pointless to log error in source component, but we don't have to consumer id here
         // SDBIlogComponent(ErrMsg, pReader->Config.ComponentId, "#error");
      }
   }
   SDBreaderFetchFromCache(Code, pReader, pLoop);
}

// returns -1 if need to load more data to CiphertextBuffer
// returns number of bytes consumed and decrypted data is appended to PlaintextBuffer
int SDBparseEncryptedBuffer(const COLstring& EncryptionKey, COLstring& CiphertextBuffer, COLstring& PlaintextBuffer, COLlist<SDBcorruptRegion>* pCorruptions) {
   COL_FUNCTION(SDBparseEncryptedBuffer);
   int Consumed = 0;
   // 1. we have a partial enc packet in cipher buffer
   // 2. we have 2.5 enc packets in cipher buffer
   // 3. we have 1 enc packet in cipher buffer
   // 4. we have 3 enc packets followed by corrupted packet in cipher buffer
   // 5. we have 3 enc packets followed by corrupted packet followed by another 3 enc packets in cipher buffer
   // 6. we have corrupted data in cipher buffer
   do {
      int Amount = SDBparseAndDecrypt(CiphertextBuffer.data(), CiphertextBuffer.size(), EncryptionKey, &PlaintextBuffer);
      if (Amount > 0) {
         Consumed += Amount;
         CiphertextBuffer.shift(Amount);
         continue;
      }
      if (-1 == Amount) {  // need more data
         if (Consumed > 0) {
            COL_TRC("Decrypted all messages in Ciphertext buffer");
            return Consumed;  // 2, 3
         } else {
            COL_TRC("Need to load more encrypted data to decrypt the 1st msg in ciphertext buffer");
            return -1;     // 1
         }
      } else {    // -2 to -5 : number read error, marker not found, decrypt fail
         COL_TRC("parse and decrypt failed with error " << Amount << ". Find the next encryption packet");
         int Delta = SDBfindNextEncMarker(CiphertextBuffer.data(), CiphertextBuffer.size());
         if (-1 == Delta) {
            if (Consumed > 0) {
               COL_TRC("found corruption after decrypting messages in ciphertext buffer");
               // the corrupted region will be recorded the next time SDBparseEncryptedBuffer is called
               return Consumed;  // 4
            } else {
               COL_TRC("ciphertext buffer contains corrupted data only. need more data to find the next encryption packet");
               return -1;     // 6
            }
         } else {
            pCorruptions->push_back(SDBcorruptRegion(Consumed, Delta));    // 5
            COL_TRC("Found the next encryption packet at position " << Delta);
            // skip corrupted region and try parse n decrypt
            Consumed += Delta;
            CiphertextBuffer.shift(Delta);
         }
      }
   } while (true);
}

int SDBreadAndDecrypt(int& FD, const COLstring& EncryptionKey, COLstring* pOut, int* pAmountRead, COLlist<SDBcorruptRegion>* pCorruptions) {
   COL_FUNCTION(SDBreadAndDecrypt);
   *pAmountRead = 0;
   COLstring Buffer;
   Buffer.setCapacity(SDB_READER_BUF_LOAD_SIZE);
   do {
      int Amount = SDBreadLogFile(FD, Buffer.get_buffer() + Buffer.size(), SDB_READER_BUF_LOAD_SIZE);
      if (Amount <  0) { COL_ERR("read error"); return -Amount; }
      if (Amount == 0) { COL_TRC("EOF");        return EOF;     }
      Buffer.setSize(Buffer.size() + Amount);
      COL_TRC("Read " << Amount << " bytes from file");
      int Consumed = SDBparseEncryptedBuffer(EncryptionKey, Buffer, *pOut, pCorruptions);
      if (Consumed > 0) {
         COL_TRC("consumed " << Consumed << " bytes");
         *pAmountRead += Consumed;
         return 0;   // success but not necessarily EOF
      } else {
         COLASSERT(Consumed == -1 );   // SDBparseEncryptedBuffer shouldn't return other error codes
         COL_TRC("need to read more data to process the first encrypted packet");
         COLASSERT(Buffer.size() < SDBmaxMessageSizeMB() * SDB_SIZE_MB * 2);  // TODO: what if Buffer is encrypted 10 100MB messages enqueued at the same time?
         if (Buffer.capacity() < Buffer.size() + SDB_READER_BUF_LOAD_SIZE) {
            Buffer.setCapacity(Buffer.size() + SDB_READER_BUF_LOAD_SIZE);
         }
      }
   } while (true);
}

// WORKER
static void SDBbufferLoadEncryptedWorker(SDBreaderConfig Config, COLstring* pFilename, COLuint64 Position, SCKloop* pLoop, COLclosure4<int, COLauto<COLstring>, int, COLauto<COLlist<SDBcorruptRegion>>>* pCallback, COLstring EncryptionKey) {
   COL_FUNCTION(SDBbufferLoadEncryptedWorker);
   int Consumed = 0;
   COLauto<COLstring> pBuffer = new COLstring();
   COLauto<COLlist<SDBcorruptRegion>> pCorruptions = new COLlist<SDBcorruptRegion>();
   int Error = 0;
   if (!SDBsetFilename(Config, pFilename, &Error)) {
      // Error is either EOF or SDB_READER_BAD_CONFIG
      pLoop->post(COLnewClosure0(pCallback, &COLclosure4<int, COLauto<COLstring>, int, COLauto<COLlist<SDBcorruptRegion>>>::runAndDelete, Error, pBuffer, Consumed, pCorruptions));
      return;
   }
   int Code = 0, FD = -1;
   do {
      COLint64 Result = -1;
      COL_VAR(*pFilename);
      FD = SDBopenLogFile(*pFilename);
      if (FD < 0)     { COL_ERR("open error"); Code = -FD;     break; }
      Result = SDBseekLogFile(FD, Position);
      if (Result < 0) { COL_ERR("seek error"); Code = -Result; break; }
      Code = SDBreadAndDecrypt(FD, EncryptionKey, pBuffer, &Consumed, pCorruptions);
   } while (false);
   SDBcloseLogFile(FD);
   pLoop->post(COLnewClosure0(pCallback, &COLclosure4<int, COLauto<COLstring>, int, COLauto<COLlist<SDBcorruptRegion>>>::runAndDelete, Code, pBuffer, Consumed, pCorruptions));
}

// MAIN
static void SDBreaderFetchFromFile(SDBreader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromFile);
   COL_VAR2(pReader->Config.ComponentId, pReader->Config.DateTime);
   COL_VAR3(pReader->FilePath, pReader->Position, pReader->Cache.size());
   COLASSERT(!pReader->Fetching);
   pReader->Fetching = true;
   if (pReader->EncryptionKey.is_null()) {
      COLclosure2<int, COLauto<COLstring>>* pComplete = COLnewClosure2(&SDBbufferLoaded, pReader, pLoop);
      COLclosure0* pTask = COLnewClosure0(&SDBbufferLoadWorker, pReader->Config, &pReader->FilePath, pReader->Position, pLoop, pComplete, COLauto<COLstring>());
      pLoop->threadPool()->scheduleTask(pTask);
   } else {
      COLclosure4<int, COLauto<COLstring>, int, COLauto<COLlist<SDBcorruptRegion>>>* pComplete = COLnewClosure4(&SDBbufferLoadedEncrypted, pReader, pLoop);
      COLclosure0* pTask = COLnewClosure0(&SDBbufferLoadEncryptedWorker, pReader->Config, &pReader->FilePath, pReader->Position, pLoop, pComplete, pReader->EncryptionKey);
      pLoop->threadPool()->scheduleTask(pTask);
   }
}

// MAIN
void SDBreaderFetch(SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, SDBcontent*>* pCallback) {
   COL_FUNCTION(SDBreaderFetch);
   pReader->Requests.push_back(pCallback);
   COL_VAR(pReader->Requests.size());
   if (pReader->Cache.size() > 0) {
      SDBreaderFetchFromCache(0, pReader, pLoop);
      return;
   }
   if (pReader->Fetching) {
      COL_TRC("Fetch in progress. Request will be served upon completion.");
      return;
   }
   SDBreaderFetchFromFile(pReader, pLoop);
}

static void SDBconvertContentToMessage(int Code, SDBcontent* pContent, COLclosure2<int, SDBmessage*>* pCallback) {
   if (pContent) { COLASSERT(SDBisMessage(pContent)); }
   pCallback->runAndDelete(Code, (SDBmessage*)pContent);
}

void SDBreaderFetchMessage(SDBreader* pReader, SCKloop* pLoop, COLclosure2<int, SDBmessage*>* pCallback) {
   SDBreaderFetch(pReader, pLoop, COLnewClosure2(&SDBconvertContentToMessage, pCallback));
}

COLstring SDBreaderCorruptedRegionErrorMessage(const COLlist<SDBcorruptRegion>& Corruptions, const COLstring& LogFile) {
   COL_FUNCTION(SDBreaderCorruptedRegionErrorMessage);
   if (0 == Corruptions.size()) { return ""; }
   COLstring ErrorString;
   COLostream ErrorStream(ErrorString);
   ErrorStream << "Part(s) of the logfile '" << LogFile << "' seem to be corrupted:" << newline;
   for (auto Corruption = Corruptions.begin(); Corruption != Corruptions.end(); Corruption++) {
      ErrorStream << " * offset " << Corruption->Position << " to " << Corruption->Position + Corruption->Size << newline;
   }
   return ErrorString;
}

COLstring SDBreaderCorruptedRegionErrorMessage(SDBreader* pReader) {
   COL_FUNCTION(SDBreaderCorruptedRegionErrorMessage);
   return SDBreaderCorruptedRegionErrorMessage(pReader->Corruptions, pReader->FilePath);
}

// Passing MessageId by value because its SDBmessage could be deleted in the while loop below, causing "Id == MessageId" to fail
void SDBreaderRemoveMessageFromCache(SDBreader* pReader, SDBid MessageId) {
   COL_FUNCTION(SDBreaderRemoveMessageFromCache);
   if (!pReader->ManualRemove) {
      COL_WRN("SDBreaderRemoveMessageFromCache() is supposed to be used with ManualRemove = true");
   }
   COLlist<SDBcontent*>& Cache = pReader->Cache;
   while (Cache.size()) {
      SDBcontent* pContent = Cache.front();
      COLASSERT(SDBisMessage(pContent));
      SDBid Id = ((SDBmessage*)pContent)->Id;
      COL_TRC("Removing message " << MessageId);
      if (Id <= MessageId) {
         SDBcontentDestroy(pContent);
         Cache.pop_front();
         if (Id == MessageId) {
            COL_TRC("Done");
            break;
         }
      }
   }
}

// TODO - FilePath - should perhaps be non const pointer to indicate it is changed by the function.
static void SDBnextFile(bool* pFound, COLstring& FilePath, COLstring ComponentId) {
   COL_FUNCTION(SDBnextFile);
   if (FilePath.is_null()) {
      *pFound = false;
      return;
   }
   COLstring NextFile = SDBfindNextLogFile(FilePath, ComponentId);
   *pFound = !NextFile.is_null();
   if (*pFound) {
      FilePath = NextFile;
   }
}

SDBreader* SDBcreateLogReader(const COLstring& ComponentId, COLuint64 StartDateTime, const COLstring& EncryptionKey, COLclosure2<const SDBcontent*, bool*>* pFilter, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcreateLogReader);
   COL_VAR2(ComponentId, StartDateTime);
   if (!pFilter) { pFilter = COLnewClosure2(&SDBfilterMatch, SDBlogFilter()); }
   SDBreader* pReader = new SDBreader();
   pReader->pFilter = pFilter;
   pReader->Config.ComponentId = ComponentId;
   pReader->Config.DateTime    = StartDateTime;
   pReader->Config.LogRootDir  = LogRootDir;
   pReader->pNextFileCall = COLnewClosure2(&SDBnextFile, ComponentId);
   pReader->EncryptionKey = EncryptionKey;
   return pReader;
}

SDBreader* SDBcreateDequeueReader(const SDBmessageId& Config, const COLstring& EncKey, bool ManualRemove, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcreateDequeueReader);
   // NOTE SDBqueueFilter inclusive true to include the StartPosition message for bad StartPosition detection
   // bad StartPosition could be due to missing log file or corruptions in log file
   COLclosure2<const SDBcontent*, bool*>* pQueueFilter = COLnewClosure2(&SDBfilterMatch, SDBqueueFilter(Config, true));
   SDBreader* pReader = SDBcreateLogReader(Config.ComponentId, Config.DateTime, EncKey, pQueueFilter, LogRootDir);
   pReader->ManualRemove = ManualRemove;
   pReader->StartPosition = Config;    // set StartPosition to throw an error if StartPosition is not found in logs
   return pReader;
}

SDBreader* SDBcreateQueueReader(const SDBmessageId& Config, const COLstring& EncKey, bool ManualRemove, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcreateQueueReader);
   COLclosure2<const SDBcontent*, bool*>* pQueueFilter = COLnewClosure2(&SDBfilterMatch, SDBqueueFilter(Config));
   SDBreader* pReader = SDBcreateLogReader(Config.ComponentId, Config.DateTime, EncKey, pQueueFilter, LogRootDir);
   pReader->ManualRemove = ManualRemove;
   return pReader;
}

SDBreader* SDBcreateQueueReader(const SDBfilterInfo& Filter, const COLstring& EncKey, bool ManualRemove, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcreateQueueReader);
   COLclosure2<const SDBcontent*, bool*>* pQueueFilter = COLnewClosure2(&SDBfilterMatch, Filter);
   const SDBmessageId Config = Filter.MessageId;
   SDBreader* pReader = SDBcreateLogReader(Config.ComponentId, Config.DateTime, EncKey, pQueueFilter, LogRootDir);
   pReader->ManualRemove = ManualRemove;
   return pReader;
}

SDBreader* SDBcreateJournalReader(const COLstring& JournalFilePath, const COLstring& EncryptionKey) {
   COL_FUNCTION(SDBcreateJournalReader);
   SDBreader* pReader = new SDBreader();
   pReader->FilePath = JournalFilePath;
   pReader->pFilter  = COLnewClosure2(&SDBfilterMatch, SDBjournalFilter());
   pReader->EncryptionKey = EncryptionKey;
   return pReader;
}

void SDBdeleteReader(SDBreader* pReader, COLclosure0* pCallback) {
   COLclosure0* pShutdown = COLnewClosure0(&SDBdestroyReader, pReader, pCallback);
   if (pReader->Fetching) {
      COL_TRC("Reader busy, shutdown requested");
      pReader->pShutdown = pShutdown;
   } else {
      COL_TRC("Shutting down reader now");
      pShutdown->runAndDelete();
   }
}
