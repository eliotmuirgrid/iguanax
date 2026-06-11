// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTreader
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 02:12PM
// ---------------------------------------------------------------------------
#include <cmath>

#include <SDBT/SDBTreader.h>
#include <SDBT/SDBTenvelope.h>
#include <SDBT/SDBTmessage.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBlogWrite.h>
#include <SDB/SDBmessage.h>
#include <SDB/SDBposition.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

COLmap<COLstring, COLarray<COLauto<SDBmessage>>> SDBTcreateTestLogData(int Messages, COLint64 StartId, time_t Date, int Components, int Size) {
   COL_FUNCTION(SDBTcreateTestLogData);
   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData;
   for (int i = 1; i <= Components; ++i) {
      COLstring ComponentId = "Component_" + COLintToString(i);
      SDBTcreateTestMessageList(&LogData[ComponentId], Messages, ComponentId, StartId, Size, Date);
   }
   return LogData;
}

static void SDBTpushData(SDBcontent* pContent, COLstring* pBuffer) {
   COL_FUNCTION(SDBTpushData);
   COLstring TempBuffer;
   TempBuffer.setCapacity(SDBcontentSize(pContent));
   int Amount = SDBwriteContent(pContent, TempBuffer.get_buffer(), TempBuffer.capacity()-1);
   TempBuffer.setSize(Amount);
   SDBenvelopeWrite(pBuffer, TempBuffer);
}

void SDBTcreateTestJournalData(COLstring* pBuffer, const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& LogData, time_t Date, int StartIndex) {
   COL_FUNCTION(SDBTcreateTestJournalData);
   int Components = LogData.size();
   int Messages   = LogData.begin().value().size();
   SDBTcreateEnvelopedTestPositionMap(pBuffer, Components, 1, Date);
   for (int i = StartIndex; i < Messages; ++i) {
      for (auto Entry = LogData.cbegin(); Entry != LogData.cend(); Entry++) {
         const SDBmessage* pMessage = Entry.value()[i];
         SDBTpushData((SDBcontent*)pMessage, pBuffer);
         if (SDB_MESSAGE == pMessage->Type && pMessage->Id.MessageId.Index > 4) {
            SDBposition Position("Consumer", pMessage->Id);
            Position.Position.MessageId.Index -= 4;  // because we currently have 4 content types: SDB_MESSAGE, SDB_INFO, SDB_WARNING, and SDB_ERROR
            SDBTpushData((SDBcontent*)&Position, pBuffer);
         }
      }
   }
}

void SDBTcreateTestJournalFile(const COLstring& FilePath, time_t Date, int Messages, COLint64 StartId, int Components, int Size) {
   COL_FUNCTION(SDBTcreateTestJournalFile);
   COL_VAR(FilePath);
   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> LogData = SDBTcreateTestLogData(Messages, StartId, Date, Components, Size);
   SDBTcreateTestJournalFile(FilePath, LogData);
}

void SDBTcreateTestJournalFile(const COLstring& FilePath, const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& LogData) {
   COL_FUNCTION(SDBTcreateTestJournalFile);
   COL_VAR(FilePath);
   COLstring Buffer;
   int Components = LogData.size();
   int Messages = LogData.cbegin().value().size();
   int Size = LogData.cbegin().value()[0]->Data.size();
   time_t Date = LogData.cbegin().value()[0]->Id.MessageId.DateTime / 1000;
   Buffer.setCapacity(Components*Messages*(Size+100));   // just a rough estimate
   SDBTcreateTestJournalData(&Buffer, LogData, Date);
   SDBcheckLogDir(FilePath);
   FILwriteFile(FilePath, Buffer);
}

void SDBTcreateTestLogData(COLstring* pBuffer, const COLarray<COLauto<SDBmessage>>& LogData) {
   COL_FUNCTION(SDBTcreateTestLogData);
   int Messages = LogData.size();
   int Size = LogData[0]->Data.size();
   pBuffer->setCapacity(Messages*(Size+100));
   for (int i = 0; i < LogData.size(); ++i) {
      SDBTcreateEnvelopedTestMessage(pBuffer, LogData[i]);
   }
}

void SDBTcreateTestLogFile(const COLstring& FilePath, time_t Date, int Messages, COLint64 StartId, const COLstring& ComponentId, int Size) {
   COL_FUNCTION(SDBTcreateTestLogFile);
   COLarray<COLauto<SDBmessage>> MessageList;
   SDBTcreateTestMessageList(&MessageList, Messages, ComponentId, StartId, Size, Date);
   SDBTcreateTestLogFile(FilePath, MessageList);
}

void SDBTcreateTestLogFile(const COLstring& FilePath, const COLarray<COLauto<SDBmessage>>& LogData) {
   COL_FUNCTION(SDBTcreateTestLogFile);
   COLstring Buffer;
   SDBTcreateTestLogData(&Buffer, LogData);
   SDBcheckLogDir(FilePath);
   FILwriteFile(FilePath, Buffer);
}

void SDBTcreateTestLogFileLarge(const COLstring& FilePath, time_t Date, int Messages, COLint64 StartId, const COLstring& ComponentId) {
   COL_FUNCTION(SDBTcreateTestLogFileLarge);
   int Size = std::ceil(SDB_LOG_ROLLOVER_SIZE / Messages);
   SDBTcreateTestLogFile(FilePath, Date, Messages, StartId, ComponentId, Size);
}


void SDBTcreateTestLogFiles(time_t StartDate, int Days, const COLstring& LogDir, const COLstring& ComponentId, int Messages, COLint64 StartId, int Size) {
   COL_FUNCTION(SDBTcreateTestLogFiles);
   time_t EndDate = StartDate + Days * SDB_DAY;
   for (time_t Date = StartDate; Date < EndDate; Date += SDB_DAY) {
      COLstring FilePath = SDBlogFile(ComponentId, Date*1000, LogDir);
      COL_VAR(FilePath);
      SDBTcreateTestLogFile(FilePath, Date, Messages, StartId, ComponentId, Size);
      StartId += Messages;
   }
}