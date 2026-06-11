// ---------------------------------------------------------------------------
// Copyright (C) 1997-2023 iNTERFACEWARE Inc.  All Rights Reserved
//
// Module: SDBpurgeManual
//
// Description:
//
// Implementation
//
// Author: John Q
// Date:   Tue 15 Oct 2024 16:56:26 EDT
// ---------------------------------------------------------------------------
#include <SDB/SDBpurge.h>
#include <SDB/SDBpurgeAuto.h>
#include <SDB/SDBpurgeComponent.h>
#include <SDB/SDBserviceLog.h>
#include <SDB/SDBsystem.h>

#include <SCK/SCKloop.h>

#include <COL/COLlog.h>
COL_LOG_MODULE;


static void SDBpurgeLogFiles(COLstring ComponentId, SDBmessageId StopId, SCKloop* pLoop, COLclosure2<COLstring, COLstring>* pCallback) {
   COL_FUNCTION(SDBpurgeLogFiles);
   COL_VAR(ComponentId);
   COLstring FileTime1 = SDBpurgeComponentLogs(ComponentId, StopId, 0, SDBroot());
   COLstring FileTime2;
   if (!SDBroot2().is_null()) {
      FileTime2 = SDBpurgeComponentLogs(ComponentId, StopId, 0, SDBroot2());
   }
   COL_VAR2(FileTime1, FileTime2);
   if (pCallback) {
      pLoop->post(COLnewClosure0(pCallback, &COLclosure2<COLstring, COLstring>::runAndDelete, FileTime1, FileTime2));
   }
}

void SDBpurgeLog(const SDBsystem* pSystem, const COLstring& ComponentId, COLclosure2<COLstring, COLstring>* pCallback) {
   COL_FUNCTION(SDBpurgeLog);
   SDBmessageId StopId;
   if (SDB_SERVICE_COMPONENT_ID != ComponentId) {
      const auto& Component = pSystem->ComponentMap[ComponentId];
      COLASSERT(!Component.LastJournaledId.MessageId.isNull());   // or something is terribly wrong
      StopId = SDBdetermineStopId(Component);
   }
   COL_VAR(StopId);
   pSystem->pLoop->threadPool()->scheduleTask(COLnewClosure0(&SDBpurgeLogFiles, ComponentId, StopId, pSystem->pLoop, pCallback));
}
