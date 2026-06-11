// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeComponent
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 15 Oct 2024 14:56:43 EDT
// ---------------------------------------------------------------------------
#include <SDB/SDBcapi.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBmessageId.h>
#include <SDB/SDBpurge.h>
#include <SDB/SDBpurgeComponent.h>

#include <FIL/FILpathUtils.h>
#include <FIL/FILutils.h>

#include <COL/COLarray.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


COLuint64 SDBgetPurgeLimit(const SDBmessageId& StopId, int DaysToKeep) {
   COL_FUNCTION(SDBgetPurgeLimit);
   COL_VAR2(StopId, DaysToKeep);
   COLASSERT(COL_INT_32_MAX != DaysToKeep);  // COL_INT_32_MAX mean infinity
   COLuint64 PurgeLimit = (time(NULL) - DaysToKeep * 86400) * 1000;
   if (!StopId.isNull() && PurgeLimit > StopId.DateTime) { PurgeLimit = StopId.DateTime; }
   return PurgeLimit;
}

COLstring SDBgetPurgeLimitFile(const COLstring& ComponentId, const SDBmessageId& StopId, int DaysToKeep, const COLstring& LogDir) {
   COL_FUNCTION(SDBgetPurgeLimitFile);
   COLuint64 PurgeLimitTime = SDBgetPurgeLimit(StopId, DaysToKeep);
   COLstring LimitFile = SDBfindLogFile(ComponentId, PurgeLimitTime, LogDir);
   if (LimitFile.is_null()) {
      COL_TRC("No file older than " << PurgeLimitTime << ", return the oldest file since instead");
      LimitFile = SDBfindOldest(SDBdir(ComponentId, LogDir));
   }
   COL_VAR(LimitFile);
   return LimitFile;
}

static void SDBlistLogFilesToPurge(const COLstring& ComponentId, const COLstring& PurgeLimitFile, COLarray<COLstring>* pPurgeList, const COLstring &RootDir = SDBroot()) {
   COL_FUNCTION(SDBlistLogFilesToPurge);
   COLarray<COLstring>& PurgeList = *pPurgeList;
   PurgeList.clear();
   COLstring File = SDBfindOldest(SDBdir(ComponentId, RootDir));
   // the oldest file should be older than to equal to PurgeLimitFile
   while (!File.is_null() && File != PurgeLimitFile) {
      COL_VAR(File);
      PurgeList.push_back(File);
      File = SDBfindNextLogFile(File, ComponentId);
   }
}

static void SDBdeleteLogFiles(const COLarray<COLstring>& PurgeList) {
   COL_FUNCTION(SDBdeleteLogFiles);
   for (int i = 0; i < PurgeList.size(); ++i) {
      const COLstring& File = PurgeList[i];
      try {
         if (FILfileExists(File)) {
            COL_TRC("Deleting file " << File);
            FILremove(File, true);
         }
         COLstring ParentDir = FILparentDir(File);
         if (FILisDirectoryEmpty(ParentDir)) {
            COL_TRC("Deleting empty directory " << ParentDir);
            FILremoveDir(ParentDir, true);
         }
      } catch (const COLerror& Error) {
         COL_ERR(Error);
         SDBIlogService("Failed to delete '" + File + "'. Error: " + Error.description(), "#purge");
      }
   }
}

COLstring SDBpurgeComponentLogs(const COLstring& ComponentId, const SDBmessageId& StopId, int DaysToKeep, const COLstring &RootDir){
   COL_FUNCTION(SDBpurgeComponentLogs);
   COL_VAR2(StopId, DaysToKeep);
   COLstring LogMessage;
   COLostream Stream(LogMessage);
   Stream << "Purging logs for component '" + SDBInameFromGuid(ComponentId) + "' in '" << RootDir << "':" << newline;
   Stream << "Requirements: file age is older than '" << DaysToKeep << "' days";
   if (StopId.isNull()) {
      Stream << "." << newline;
   } else {
      Stream << " and file doesn't contain messages newer than '" << StopId << "'." << newline;
   }
   if (COL_INT_32_MAX == DaysToKeep) {
      COL_TRC("Need to keep all logs.");
      Stream << "Need to keep all logs according to purge rules." << newline;
      SDBIlogService(LogMessage, "#purge");
      return "";
   }
   COLarray<COLstring> PurgeList;
   COLstring PurgeLimitFile = SDBgetPurgeLimitFile(ComponentId, StopId, DaysToKeep, RootDir);
   if (PurgeLimitFile.is_null()) {
      COL_TRC("Component doesn't have any logs");
      Stream << "The component does't have any log files." << newline;
      SDBIlogService(LogMessage, "#purge");
      return "";
   }
   Stream << "Need to purge log files older than '" << PurgeLimitFile << "'." << newline;
   SDBlistLogFilesToPurge(ComponentId, PurgeLimitFile, &PurgeList, RootDir);
   if (PurgeList.size() == 0) {
      COL_TRC("No log file meets the purge criteria.");
      Stream << "Didn't find any purgable log files." << newline;
      SDBIlogService(LogMessage, "#purge");
      return "";
   }
   Stream << "Deleting the following files:" << newline;
   for (const auto& File : PurgeList) {
      Stream << " - " << File << newline;
   }
   SDBdeleteLogFiles(PurgeList);
   Stream << "Completed." << newline;
   SDBIlogService(LogMessage, "#purge");
   return PurgeLimitFile;
}
