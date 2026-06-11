//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUlogFile.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-03, 11:54a.m.
//  ---------------------------------------------------------------------------
#include <COL/COLauto.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBfile.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBmessage.h>
#include <SDBU/SDBUlogFile.h>
#include <SDBU/SDBUmessageExtract.h>
COL_LOG_MODULE;

static COLstring SDBUfindOldestLogFileString(const COLstring& ComponentId, const COLstring& LogDir){
   COL_FUNCTION(SDBUfindOldestLogFileString);
   const COLstring RootDir = SDBdir(ComponentId, LogDir);
   return SDBfindOldest(RootDir);
}

SDBfile SDBUfindOldestLogFile(const COLstring& ComponentId) {
   COL_FUNCTION(SDBUfindOldestLogFile);
   SDBfile LogFile;
   LogFile.LogFile1 = SDBUfindOldestLogFileString(ComponentId, SDBroot());
   LogFile.BothDir  = !SDBroot2().is_null();
   if (LogFile.BothDir) { LogFile.LogFile2 = SDBUfindOldestLogFileString(ComponentId, SDBroot2()); }
   return LogFile;
}

static COLstring SDBUfindStartingLogFileString(const SDBmessageId& StartMessage, const COLstring& LogDir) {
   COL_FUNCTION(SDBUfindStartingLogFileString);
   const COLstring RootDir = SDBdir(StartMessage.ComponentId, LogDir);
   if(StartMessage.isZero()) return SDBfindOldest(RootDir);
   return SDBfindLogFile(StartMessage.ComponentId, StartMessage.DateTime, LogDir);
}

SDBfile SDBUfindStartingLogFile(const SDBmessageId& StartMessage) {
   COL_FUNCTION(SDBUfindStartingLogFile);
   SDBfile LogFile;
   LogFile.LogFile1 = SDBUfindStartingLogFileString(StartMessage, SDBroot());
   LogFile.BothDir  = !SDBroot2().is_null();
   if (LogFile.BothDir) { LogFile.LogFile2 = SDBUfindStartingLogFileString(StartMessage, SDBroot2()); }
   return LogFile;
}

bool SDBUloadFileToList(const SDBfile& LogFile, const SDBfilterInfo2& Filter, COLlist<SDBcontent*>* pList) {
   COL_FUNCTION(SDBUloadFileToList);
   SDBcontentDelete(pList);
   if (!LogFile.BothDir) {
      COL_TRC("Single log dir mode");
      COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter = COLnewClosure2(&SDBfilterMatch2, Filter);
      return SDBUmessageExtract(LogFile.LogFile1, pFilter, pList);
   }
   if (!LogFile.LogFile1.is_null() &&  LogFile.LogFile2.is_null()) {
      COL_TRC("Double log dir mode, but no log file in dir 2");
      COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter = COLnewClosure2(&SDBfilterMatch2, Filter);
      return SDBUmessageExtract(LogFile.LogFile1, pFilter, pList);
   }
   if ( LogFile.LogFile1.is_null() && !LogFile.LogFile2.is_null()) {
      COL_TRC("Double log dir mode, but no log file in dir 1");
      COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter = COLnewClosure2(&SDBfilterMatch2, Filter);
      return SDBUmessageExtract(LogFile.LogFile2, pFilter, pList);
   }
   // Caller SDBUlogComponentImp() ensures at least one of LogFile1 and LogFile2 is present
   COLlist<SDBcontent*> List1;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter1 = COLnewClosure2(&SDBfilterMatch2, Filter);
   SDBUmessageExtract(LogFile.LogFile1, pFilter1, &List1);
   COLlist<SDBcontent*> List2;
   COLauto<COLclosure2<const SDBcontent*, bool*>> pFilter2 = COLnewClosure2(&SDBfilterMatch2, Filter);
   SDBUmessageExtract(LogFile.LogFile2, pFilter2, &List2);
   COL_VAR2(List1.size(), List2.size());
   SDBUmergeFileToList(List1, List2, pList);
   return true;
}

SDBfile SDBUfindNextFileToRead(const SDBfile& LogFile, const COLstring& ComponentId, bool DirectionNext) {
   COL_FUNCTION(SDBUfindNextFileToRead);
   COL_VAR(LogFile);
   COL_VAR(DirectionNext);
   SDBfile File;
   File.BothDir = LogFile.BothDir;
   if (DirectionNext) {
      File.LogFile1 = SDBfindNextLogFile(LogFile.LogFile1, ComponentId);
      if (File.BothDir) {
         File.LogFile2 = SDBfindNextLogFile(LogFile.LogFile2, ComponentId);
      }
   } else {
      File.LogFile1 = SDBfindPreviousLogFile(LogFile.LogFile1, ComponentId, SDBroot());
      if (File.BothDir) {
         File.LogFile2 = SDBfindPreviousLogFile(LogFile.LogFile2, ComponentId, SDBroot2());
      }
   }
   COL_VAR(File);
   return File;
}

COLuint64 SDBUlogFileTime(const SDBfile& LogFile, const COLstring& ComponentId, bool DirectionNext) {
   COL_FUNCTION(SDBUlogFileTime);
   COLuint64 FileTime;
   if (LogFile.BothDir) {
      if (!LogFile.LogFile1.is_null() &&  LogFile.LogFile2.is_null()) {
         COL_TRC("Double log dir mode, but no log file in dir 2");
         FileTime = SDBlogFileTime(LogFile.LogFile1, ComponentId, SDBroot()) * 1000;
      }
      else if ( LogFile.LogFile1.is_null() && !LogFile.LogFile2.is_null()) {
         COL_TRC("Double log dir mode, but no log file in dir 1");
         FileTime = SDBlogFileTime(LogFile.LogFile2, ComponentId, SDBroot2()) * 1000;
      }
      else {
         COLuint64 FileTime1 = SDBlogFileTime(LogFile.LogFile1, ComponentId, SDBroot()) * 1000;
         COLuint64 FileTime2 = SDBlogFileTime(LogFile.LogFile2, ComponentId, SDBroot2()) * 1000;
         // if FileTime1 == FileTime2, doesn't matter which we use as FileTime
         if (DirectionNext) {
            FileTime = FileTime1 < FileTime2 ? FileTime1 : FileTime2;
         } else {
            FileTime = FileTime1 > FileTime2 ? FileTime1 : FileTime2;
         }
      }
   } else {
      FileTime = SDBlogFileTime(LogFile.LogFile1, ComponentId, SDBroot()) * 1000;
   }
   return FileTime;
}

void SDBUmergeFileToList(COLlist<SDBcontent*>& List1, COLlist<SDBcontent*>& List2, COLlist<SDBcontent*>* pList) {
   COL_FUNCTION(SDBUmergeFileToList);
   auto ppMessage1 = List1.begin();
   auto ppMessage2 = List2.begin();
   while (ppMessage1 != List1.end() || ppMessage2 != List2.end()) {
      if (ppMessage1 == List1.end()) {
         pList->push_back(*ppMessage2);
         ppMessage2++;
         continue;
      }
      if (ppMessage2 == List2.end()) {
         pList->push_back(*ppMessage1);
         ppMessage1++;
         continue;
      }
      SDBmessage* pMsg1 = (SDBmessage*) (*ppMessage1);
      SDBmessage* pMsg2 = (SDBmessage*) (*ppMessage2);
      COL_VAR2(pMsg1->Id, pMsg2->Id);
      if (pMsg1->Id == pMsg2->Id) {
         pList->push_back(*ppMessage1);
         delete *ppMessage2;
         ppMessage1++;
         ppMessage2++;
         continue;
      }
      // pMsg1 != pMsg2
      if (pMsg1->Id < pMsg2->Id) {
         COL_TRC("Save msg1 " << pMsg1->Id.MessageId);
         pList->push_back(*ppMessage1);
         ppMessage1++;
      } else {
         COL_TRC("Save msg2 " << pMsg2->Id.MessageId);
         pList->push_back(*ppMessage2);
         ppMessage2++;
      }
   }
}