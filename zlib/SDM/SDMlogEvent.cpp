// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDMlogEvent
//
// Description:
//
// Implementation
//
// Author: Eliot Muir
// Date:   Friday 04 August 2023 - 03:33PM
// ---------------------------------------------------------------------------
#include <thread>

#include <SDM/SDMlogEvent.h>

#ifdef _WIN32
#include <COL/COLwindows.h>
#define SDM_LOG_INFO EVENTLOG_INFORMATION_TYPE
#define SDM_LOG_ERR  EVENTLOG_ERROR_TYPE
#else
#include <syslog.h>
#include <stdlib.h>
#define SDM_LOG_INFO LOG_INFO
#define SDM_LOG_ERR  LOG_ERR
#endif

#include <COL/COLthreadPool.h>
#include <COL/COLclosure.h>
#include <COL/COLlog.h>
COL_LOG_MODULE;

#ifdef _WIN32

static void SDMlogEventWorker(COLstring ServiceName, int EventType, COLstring Log) {
   COL_FUNCTION(SDMlogEventWorker);
   COL_VAR2(ServiceName, Log);
   HANDLE hEventLog = NULL;
   LPCSTR lpszStrings[2] = { NULL, NULL };
   // Use RegisterEventSource to get a handle to the event log
   hEventLog = RegisterEventSourceA(NULL, ServiceName.c_str());
   if (NULL == hEventLog) {
      int LastError = ::GetLastError();
      COL_ERR("RegisterEventSource failed with " << COLerrorString(LastError));
      goto cleanup;
   }
   COL_TRC("Successfully registered event source");
   // Add string to the event
   lpszStrings[0] = Log.c_str();
   // Now report the event
   if (!ReportEventA(hEventLog, EventType, 0, 100, NULL, 1, 0, lpszStrings, NULL)) {
      int LastError = ::GetLastError();
      COL_ERR("ReportEvent failed with "  << COLerrorString(LastError));
      goto cleanup;
   }
cleanup:
   if (hEventLog) {
      DeregisterEventSource(hEventLog);
   }
}

#else    // POSIX

static void SDMlogEventWorker(COLstring ServiceName, int EventType, COLstring Log) {
   COL_FUNCTION(SDMlogEventWorker);
   COL_VAR2(ServiceName, Log);
   openlog(ServiceName.c_str(), LOG_PID, LOG_USER);
   syslog(EventType, "%s", Log.c_str());
   closelog();
}

#endif

static void SDMlogEventImp(const COLstring& ServiceName, int EventType, const COLstring& Log) {
   COL_FUNCTION(SDMlogEventImp);
   COL_VAR2(ServiceName, Log);
   // If system (not iguana) env var IFW_DISABLE_SYSTEM_LOG is set, disable log event
   // The detached thread solution below should work fine.
   // This is a fail-safe solution for the next release.
   if (getenv("IFW_DISABLE_SYSTEM_LOG")) {
      COL_TRC("System logging is disabled");
      return;
   }

   // SDMlogEventWorker could hang on Windows, so we only give it 1 second to run. See IX-3882
   COL_TRC("Run SDMlogEventWorker on a worker thread");
   std::thread Worker(SDMlogEventWorker, ServiceName, EventType, Log);
   COL_TRC("Detach worker thread to avoid blocking main");
   Worker.detach();
}

void SDMlogEvent(const COLstring& ServiceName, const COLstring& Log){
   COL_FUNCTION(SDMlogEvent);
   SDMlogEventImp(ServiceName, SDM_LOG_INFO, Log);
}

void SDMlogErrorEvent(const COLstring& ServiceName, const COLstring& Log){
   COL_FUNCTION(SDMlogErrorEvent);
   SDMlogEventImp(ServiceName, SDM_LOG_ERR, Log);
}
