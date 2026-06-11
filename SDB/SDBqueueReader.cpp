// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBqueueReader
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Wed Jul 16 08:14:46 PDT 2025
// ---------------------------------------------------------------------------
#include <cstdio>

#include <SDB/SDBqueueReader.h>

#include <SDB/SDBcryptoFile.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBenvVar.h>

#include <SCK/SCKloop.h>

#include <COL/COLthreadPool.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

static void SDBreaderFetchFromFile(SDBqueueReader* pReader, SCKloop* pLoop);

SDBqueueReader::SDBqueueReader() {
}

static void SDBfreeMessageList(COLlist<SDBmessage*>& Messages) {
   while (Messages.size()) {
      delete Messages.front();
      Messages.pop_front();
   }
}

SDBqueueReader::~SDBqueueReader() {
   SDBfreeMessageList(Messages);
   if (pRequest) { delete pRequest; }
}

// MAIN
static void SDBreaderFetchFromCache(int Code, SDBqueueReader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromCache);
   COLclosure1<int>* pRequest = pReader->pRequest;
   pReader->pRequest = nullptr;
   COLASSERT(pRequest);
   // use timer to solve the deep nesting problem if pRequest calls SDBreaderFetchMessage()
   // pRequest->runAndDelete(Code);
   pLoop->addTimer(COLnewClosure0(pRequest, &COLclosure1<int>::runAndDelete, Code));
}

static SDBcontent* SDBunpackContent(const char* pEnvelope, int BufferSize) {
   COL_FUNCTION(SDBunpackContent);
   COL_VAR(BufferSize);
   COLint64 Type = SDBenvelopeContentType(pEnvelope, BufferSize);
   COL_VAR(SDBtypeAsString((SDBtype)Type));
   SDBcontent* pContent = SDBcontentCreate(Type);
   int Amount = SDBreadContent(pContent, SDBenvelopeContent(pEnvelope, BufferSize), SDBenvelopeContentSize(pEnvelope, BufferSize));
   COL_VAR(*pContent);
   return pContent;
}

static void SDBfilterContent(SDBcontent* pContent, const SDBmessageId& StartPosition, COLlist<SDBmessage*>* pOut) {
   COL_FUNCTION(SDBfilterContent);
   if (SDB_MESSAGE == pContent->Type) {
      SDBmessage* pMessage = (SDBmessage*) pContent;
      if (pMessage->Id.MessageId >= StartPosition) {   // include message at StartPosition to check for invalid start position
         pOut->push_back(pMessage);
         return;
      }
   }
   SDBcontentDestroy(pContent);
}

// MAIN
int SDBextractMessages(const char* pBuffer, int BufferSize, const SDBmessageId& StartPosition, COLlist<SDBmessage*>* pOut, COLlist<SDBcorruptRegion>* pCorruptions) {
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
      SDBfilterContent(pContent, StartPosition, pOut);
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
static void SDBgotNextFile(bool Success, COLstring NextFilePath, SDBqueueReader* pReader, SCKloop* pLoop) {
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
static void SDBgetNextFile(COLstring ComponentId, COLstring FilePath, SCKloop* pLoop, COLclosure2<bool, COLstring>* pCallback) {
   COL_FUNCTION(SDBgetNextFile);
   bool Success = false;
   if (!FilePath.is_null()) {
      COLstring NextFile = SDBfindNextLogFile(FilePath, ComponentId);
      Success = !NextFile.is_null();
      if (Success) { FilePath.swap(&NextFile); }
   }
   // else FilePath not set because component has no logs
   pLoop->post(COLnewClosure0(pCallback, &COLclosure2<bool, COLstring>::runAndDelete, Success, FilePath));
}

// MAIN
static void SDBreaderFetchFromNextFile(SDBqueueReader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromNextFile);
   COL_TRC("Current File = " << pReader->FilePath);
   pReader->Fetching = true;
   COLclosure2<bool, COLstring>* pCallback = COLnewClosure2(&SDBgotNextFile, pReader, pLoop);
   pLoop->threadPool()->scheduleTask(COLnewClosure0(SDBgetNextFile, pReader->Config.ComponentId, pReader->FilePath, pLoop, pCallback));
}

static void SDBconvertToAbsoluteCorruptionPositions(COLuint64 FilePosition, COLlist<SDBcorruptRegion>* pCorruptions) {
   COL_FUNCTION(SDBconvertToAbsoluteCorruptionPositions);
   for (auto Corruption = pCorruptions->begin(); Corruption != pCorruptions->end(); Corruption++) {
      Corruption->Position += FilePosition;
   }
}

static void SDBbufferLoadWorker(SDBreaderConfig Config, COLstring* pFilename, COLuint64 Position, SCKloop* pLoop, COLclosure2<int, COLauto<COLstring>>* pCallback, COLauto<COLstring> pBuffer);

// MAIN
void SDBbufferLoaded(int Code, COLauto<COLstring> pBuffer, SDBqueueReader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBbufferLoaded);
   pReader->Fetching = false;
   if (pReader->pShutdown) {
      COL_WRN("Shutdown requested. Shutting down now without processing the loaded buffer");
      pReader->pShutdown->run();
      return;
   }
   if (!pBuffer->is_null()) {
      pReader->Corruptions.clear();    // empty corrupted region list as they should have been processed after each SDBreaderFetch() call
      int MessageCount = pReader->Messages.size();
      int Amount = SDBextractMessages(pBuffer->get_buffer(), pBuffer->size(), pReader->StartPosition, &(pReader->Messages), &pReader->Corruptions);
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
         // Code could be EOF here. However, we extracted some messages and serving them will take some time.
         // Note that there may be more fetch requests queued than messages available, and there could be more
         // messages added to the file by the time we finish serving. Set Code = 0 here to try again.
         if (pReader->Messages.size() > MessageCount) {     // set Code 0 only if we extracted msg
            Code = 0;
         } else { // size didn't change
            if (EOF == Code) { Code = SDB_END_OF_LOG; }     // convert EOF to SDB_END_OF_LOG
         }
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

void SDBbufferLoadedEncrypted(int Code, COLauto<COLstring> pBuffer, int Consumed, COLauto<COLlist<SDBcorruptRegion>> pCorruptions, SDBqueueReader* pReader, SCKloop* pLoop) {
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
      int MessageCount = pReader->Messages.size();
      int Amount = SDBextractMessages(pBuffer->get_buffer(), pBuffer->size(), pReader->StartPosition, &(pReader->Messages), &pReader->Corruptions);
      COLASSERT(pReader->Corruptions.size() == 0);    // no corruption
      COLASSERT(Amount > 0);  // should be able to extract at least one log
      pReader->Position += Consumed;   // advance file position for the current encrypted log file
      COL_VAR(pReader->Position);
      COL_TRC("Fetched all messages!");
      // Code could be EOF here. However, we extracted some messages and serving them will take some time.
      // Note that there may be more fetch requests queued than messages available, and there could be more
      // messages added to the file by the time we finish serving. Set Code = 0 here to try again.
      if (pReader->Messages.size() > MessageCount) {     // set Code 0 only if we extracted msg
         Code = 0;
      } else { // size didn't change
         if (EOF == Code) { Code = SDB_END_OF_LOG; }     // convert EOF to SDB_END_OF_LOG
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

// returns -1 if need to load more data to CiphertextBuffer
// returns number of bytes consumed and decrypted data is appended to PlaintextBuffer
static int SDBparseEncryptedBuffer(const COLstring& EncryptionKey, COLstring& CiphertextBuffer, COLstring& PlaintextBuffer, COLlist<SDBcorruptRegion>* pCorruptions) {
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

static int SDBreadAndDecrypt(int& FD, const COLstring& EncryptionKey, COLstring* pOut, int* pAmountRead, COLlist<SDBcorruptRegion>* pCorruptions) {
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
static void SDBreaderFetchFromFile(SDBqueueReader* pReader, SCKloop* pLoop) {
   COL_FUNCTION(SDBreaderFetchFromFile);
   COL_VAR2(pReader->Config.ComponentId, pReader->Config.DateTime);
   COL_VAR3(pReader->FilePath, pReader->Position, pReader->Messages.size());
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
void SDBreaderFetchMessage(SDBqueueReader* pReader, SCKloop* pLoop, COLclosure1<int>* pCallback) {
   COL_FUNCTION(SDBreaderFetchMessage);
   if (pReader->pRequest) {
      COL_TRC("I am working on it. Stop bothering me");
      delete pCallback;
      return;
   }
   pReader->pRequest = pCallback;
   SDBreaderFetchFromFile(pReader, pLoop);
}

static COLstring SDBreaderCorruptedRegionErrorMessage(const COLlist<SDBcorruptRegion>& Corruptions, const COLstring& LogFile) {
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

COLstring SDBreaderCorruptedRegionErrorMessage(SDBqueueReader* pReader) {
   COL_FUNCTION(SDBreaderCorruptedRegionErrorMessage);
   return SDBreaderCorruptedRegionErrorMessage(pReader->Corruptions, pReader->FilePath);
}

SDBqueueReader* SDBcreateDequeueReader(const SDBmessageId& StartPosition, const COLstring& EncryptionKey, const COLstring& LogRootDir) {
   COL_FUNCTION(SDBcreateDequeueReader);
   SDBqueueReader* pReader = new SDBqueueReader();
   pReader->StartPosition      = StartPosition;
   pReader->EncryptionKey      = EncryptionKey;
   pReader->Config.ComponentId = StartPosition.ComponentId;
   pReader->Config.DateTime    = StartPosition.DateTime;
   pReader->Config.LogRootDir  = LogRootDir;
   return pReader;
}

static void SDBdestroyReader(SDBqueueReader* pReader, COLclosure0* pCallback) {
   COL_FUNCTION(SDBdestroyReader);
   delete pReader;
   COL_TRC("Reader destroyed");
   if (pCallback) {
      pCallback->runAndDelete();
   }
}

void SDBdeleteReader(SDBqueueReader* pReader, COLclosure0* pCallback) {
   COLclosure0* pShutdown = COLnewClosure0(&SDBdestroyReader, pReader, pCallback);
   if (pReader->Fetching) {
      COL_TRC("Reader busy, shutdown requested");
      pReader->pShutdown = pShutdown;
   } else {
      COL_TRC("Shutting down reader now");
      pShutdown->runAndDelete();
   }
}
