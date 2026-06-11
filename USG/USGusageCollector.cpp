// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: USGusageCollector
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue Oct  2 10:16:34 EDT 2024
// ---------------------------------------------------------------------------
#include <USG/USGusageCollector.h>

#include <SCK/SCKloop.h>

#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;


class USGusageCollector {
public:
   int            UploadTimerId  {0};
   SCKloop*       pLoop          {nullptr};
   COLclosure0*   pCollector     {nullptr};

   USGusageCollector();
   ~USGusageCollector();
};

static USGusageCollector& USGcollector() {
   static USGusageCollector Collector;
   return Collector;
}

USGusageCollector::USGusageCollector() {
}

USGusageCollector::~USGusageCollector() {
   USGstopUsageCollectionTimer();
}

static int USGsecondsToMidnight() {
   time_t Now = time(nullptr);
   tm* pNow = localtime(&Now);
   int SecondsSinceMidnight = (pNow->tm_hour * 3600) + (pNow->tm_min * 60) + pNow->tm_sec;
   int SecondsUntilMidnight = 86400 - SecondsSinceMidnight;
   COL_VAR(SecondsUntilMidnight);
   return SecondsSinceMidnight;
}

static void USGrunUploader();
static void USGsetTimer() {
   // To prevent all iguanas from uploading usage at the same time (@ midnight),
   // upload every 24 hours after startup instead.
   // int PollTime = USGsecondsToMidnight();
   int PollTime = 86400;   // 24 hours
   const char* pValue = ::getenv("USG_POLL_TIME");
   if (pValue) {
      int Interval = ::atoi(pValue);
      if (Interval > 0) { PollTime = Interval; }
   }
   USGcollector().UploadTimerId = USGcollector().pLoop->addTimer(COLnewClosure0(&USGrunUploader), PollTime * 1000);
}

static void USGrunUploader() {
   COL_FUNCTION(USGrunUploader);
   USGcollector().UploadTimerId = 0;
   USGcollector().pCollector->run();
   USGsetTimer();
}

void USGstartUsageCollectionTimer(SCKloop* pLoop, COLclosure0* pCollector) {
   USGcollector().pLoop = pLoop;
   USGcollector().pCollector = pCollector;
   USGsetTimer();
}

void USGstopUsageCollectionTimer() {
   if (USGcollector().UploadTimerId != 0) {
      USGcollector().pLoop->cancelTimer(USGcollector().UploadTimerId);
      USGcollector().UploadTimerId = 0;
   }
   if (USGcollector().pCollector) {
      delete USGcollector().pCollector;
      USGcollector().pCollector = nullptr;
   }
}
