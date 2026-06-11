// ---------------------------------------------------------------------------
// Copyright (C) 1997-2025 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: CRHdetermineCrashLoop
//
// Description:
//
// Implementation
//
// Author: Vismay Shah 
// Date:   Tuesday 25 February 2025 - 12:31PM
// ---------------------------------------------------------------------------

#include <CRH/CRHdetermineCrashLoop.h>

#include <CRH/CRHupload.h>
#include <DIR/DIRpath.h>
#include <FIL/FILdirEnumerator.h>
#include <FIL/FILpathUtils.h>
#include <time.h>

#include <COL/COLlist.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;
#define CRH_CRASH_LOOP_THRESHOLD 60

static void CRHdetermineNewestCrashes(const COLstring& CrashDir, COLstring* pNewest, COLstring* pSecondNewest, COLstring* pThirdNewest){
   COL_FUNCTION(CRHdetermineNewestCrashes);
   COLlist<COLstring> FoundFiles;
   COL_TRC("Scanning for the newest 3 crash files");
   FILdirEnumerator Dirs(CrashDir + "*.crash", false, true);
   COLstring Filename;
   while (Dirs.getFile(Filename)){ FoundFiles.push_back(Filename); }
   int Counter = 0;
   while (FoundFiles.size() && Counter < 3){
      if (0 == Counter){ *pNewest       = FoundFiles.back();}
      if (1 == Counter){ *pSecondNewest = FoundFiles.back();}
      if (2 == Counter){ *pThirdNewest  = FoundFiles.back();}
      FoundFiles.pop_back();
      Counter++;
   }
}

static bool CRHpopulateTmStruct(const COLstring& FileName, tm& TmInfo){
   COL_FUNCTION(CRHpopulateTmStruct);
   COLstring Year, Month, Day, Hour, Minutes, Seconds;
   if (FileName.size() < 15) return false;
   Year    = FileName.substr(0, 4);
   Month   = FileName.substr(4, 2);
   Day     = FileName.substr(6, 2);
   Hour    = FileName.substr(9, 2);
   Minutes = FileName.substr(11, 2);
   Seconds = FileName.substr(13, 2);
   COL_VAR3(Year, Month, Day);
   COL_VAR3(Hour, Minutes, Seconds);
   TmInfo.tm_year = atoi(Year.c_str()) - 1900;
   TmInfo.tm_mon  = atoi(Month.c_str()) - 1;
   TmInfo.tm_mday = atoi(Day.c_str());
   TmInfo.tm_hour = atoi(Hour.c_str());
   TmInfo.tm_min  = atoi(Minutes.c_str());
   TmInfo.tm_sec  = atoi(Seconds.c_str());
   return true;
   // Could use strptime with the format string %Y%m%d-%H%M%S but then that's different behaviour for posix and windows...
}

static bool CRHcompareFileTime(const COLstring& CrashFile, const time_t& CompareTime, int Threshold){
   COL_FUNCTION(CRHcompareFileTime);
   COLstring FileName = FILpathNameWithoutLastExt(CrashFile);
   struct tm TmInfo;
   memset(&TmInfo, 0, sizeof(struct tm)); // Set up empty tm_info
   if (!CRHpopulateTmStruct(FileName, TmInfo)) { 
      COL_TRC("Failed to parse the filename into tm struct");
      return false;
   }
   TmInfo.tm_isdst = -1; // strptime and our windows substringing and setting leave the value of tm_isdst indeterminate so explicitly say its undefined. 
   time_t FileTime = mktime(&TmInfo);
   if (FileTime == -1) {
      COL_TRC("Failed to convert to time_t!");
      return false;
   }
   double Result = difftime(CompareTime, FileTime);
   if (0 <= Result && Result <= Threshold) {
      COL_TRC("Crash file's timestamp is within threshold of the current time! Maybe in a crash loop");
      return true;
   } 
   COL_TRC("Crash file's timestamp not recent enough to qualify as a crash loop.");
   return false;
}

bool CRHdetermineCrashLoop(){
   COL_FUNCTION(CRHdetermineCrashLoop);
   COLstring CrashDir = DIRcrashDir();
   COLstring Newest, SecondNewest, ThirdNewest;
   if (!FILfileExists(CrashDir)) {
      COL_TRC("Crash directory not found.");
      return false; 
   }
   CRHdetermineNewestCrashes(CrashDir, &Newest, &SecondNewest, &ThirdNewest);
   COL_VAR3(Newest, SecondNewest, ThirdNewest);
   if (ThirdNewest.is_null() || SecondNewest.is_null() || Newest.is_null()) {
      COL_TRC("Don't have at least 3 crash files - not in a loop");
      return false;
   }
   time_t Now = time(NULL);
   bool InCrashLoop = false;
   COL_TRC("Checking file timestamps vs current time to see if we're in a crash loop");
   // Treating a crash loop as multiple crashes within 1 min of each other
   InCrashLoop = CRHcompareFileTime(Newest, Now, CRH_CRASH_LOOP_THRESHOLD);
   InCrashLoop = InCrashLoop && CRHcompareFileTime(SecondNewest, Now, 2 * CRH_CRASH_LOOP_THRESHOLD);
   InCrashLoop = InCrashLoop && CRHcompareFileTime(ThirdNewest,  Now, 3 * CRH_CRASH_LOOP_THRESHOLD);
   COL_VAR(InCrashLoop);
   return InCrashLoop;
}
