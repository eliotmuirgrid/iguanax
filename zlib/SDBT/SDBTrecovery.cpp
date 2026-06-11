// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBTrecovery
//
// Description:
//
// Implementation
//
// Author: John Qi
// Date:   Friday 31 March 2023 - 02:00PM
// ---------------------------------------------------------------------------

#include <SDBT/SDBTrecovery.h>
#include <SDBT/SDBTenvelope.h>
#include <SDBT/SDBTmessage.h>
#include <SDBT/SDBTreader.h>

#include <SDB/SDBcontent.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBenvelope.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBposition.h>

#include <FIL/FILutils.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;

int SDBTcalculateLogFileSize(const COLarray<COLauto<SDBmessage>>& MessageList) {
   COL_FUNCTION(SDBTcalculateLogFileSize);
   int Size = 0;
   for (int i = 0; i < MessageList.size(); ++i) {
      Size += SDBenvelopeSizeFromContent((SDBcontent*)MessageList[i].get());
   }
   return Size;
}

static COLmap<COLstring, int> SDBTcalculateLogFileSizes(const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& Logs) {
   COL_FUNCTION(SDBTcalculateLogFileSizes);
   COLmap<COLstring, int> Sizes;
   for (auto Log = Logs.cbegin(); Log != Logs.cend(); Log++) {
      Sizes[Log.key()] = SDBTcalculateLogFileSize(Log.value());
   }
   return Sizes;
}

static void SDBTserializeJournalToFile(const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& Logs, int StartIndex, time_t Date, int Size, const COLstring& LogDir) {
   COL_FUNCTION(SDBTserializeJournalToFile);
   COLstring Buffer;
   // components * messages per component * message size estimate
   Buffer.setCapacity(Logs.size()*(Logs.begin().value().size()-StartIndex)*(Size+100));   // just a rough estimate
   SDBTcreateTestJournalData(&Buffer, Logs, Date, StartIndex);
   COLstring JournalFile = SDBjournalFile(LogDir);
   SDBcheckLogDir(JournalFile);
   FILwriteFile(JournalFile, Buffer);
}

static void SDBTremoveAndAppendLogData(COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& InOut, int Shift, const COLmap<COLstring, COLarray<COLauto<SDBmessage>>>& Data) {
   COL_FUNCTION(SDBTremoveAndAppendLogData);
   for (auto Log = InOut.begin(); Log != InOut.end(); Log++) {
      COLASSERT(Shift <= Log.value().size());
      for (int i = 0; i < Shift; ++i) {
         Log.value().remove(0);
      }
      for (int i = 0; i < Data[Log.key()].size(); ++i) {
         Log.value().push_back(Data[Log.key()][i]);
      }
   }
}

static COLmap<COLstring, COLmap<time_t, int>> SDBTmergeLogFileSizes(COLmap<COLstring, int> SizesYesterday, COLmap<COLstring, int> SizesToday, time_t Today) {
   COL_FUNCTION(SDBTmergeLogFileSizes);
   COLmap<COLstring, COLmap<time_t, int>> ExpectedSizes;
   for (auto i = SizesYesterday.cbegin(); i != SizesYesterday.cend(); i++) {
      ExpectedSizes[i.key()][Today-SDB_DAY] = i.value();
      ExpectedSizes[i.key()][Today] = SizesToday[i.key()];
   }
   return ExpectedSizes;
}

COLmap<COLstring, int> SDBTcreateTestJournalFileForNoCrash(const COLstring& LogDir, time_t Date, int Messages, COLint64 StartId, int Components, int Size) {
   COL_FUNCTION(SDBTcreateTestJournalFileForNoCrash);
   COLmap<COLstring, COLarray<COLauto<SDBmessage>>> Logs = SDBTcreateTestLogData(0, StartId, Date, Components, Size);
   // position map only
   SDBTserializeJournalToFile(Logs, 0, Date, Size, LogDir);
   // compute expected log file sizes
   COLmap<COLstring, int> ExpectedSizes = SDBTcalculateLogFileSizes(Logs);
   return ExpectedSizes;
}
