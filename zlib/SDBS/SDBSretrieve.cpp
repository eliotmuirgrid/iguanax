//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBSretrieve.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-08-20, 12:55p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLminimumInclude.h>
#include <COL/COLvar.h>
#include <COL/COLvarUtils.h>
#include <COL/COLweb.h>
#include <PRM/PRMcallbacks.h>
#include <SCK/SCKloop.h>
#include <SDB/SDBcapi.h>
#include <SDB/SDBcontentDelete.h>
#include <SDB/SDBdir.h>
#include <SDB/SDBfilter.h>
#include <SDB/SDBfilter2.h>
#include <SDB/SDBserviceLog.h>
#include <SDBS/SDBSretrieve.h>
#include <SDBS/SDBSretrieveAsync.h>
#include <SDBS/SDBSretrieveState.h>
#include <SDBS/SDBSretrieveSync.h>
#include <SDBU/SDBUparams.h>
#include <SDBU/SDBUutils.h>

COL_LOG_MODULE;

static bool SDBScheckPermission(const COLstring& Component, const COLstring& User, const COLaddress Address) {
   COLstring Error;
   if(Component != SDB_SERVICE_COMPONENT_ID && !PRMcheckRoleAccess(false, User, Component, VIEW_LOGS, &Error)) {
      COLrespondError(Address, PRMsetError(VIEW_LOGS));
      return false;
   }
   return true;
}

// /logs/retrieve
void SDBSretrieve(const COLwebRequest& Request, COLhashmap<COLstring, SDBSretrieveState>* pStateMap, SCKloop* pLoop) {
   COL_FUNCTION(SDBSretrieve);
   COL_VAR(Request.Data.json(1));
   COLstring Component, Error;
   if(!SDBUcomponentId(COLvarGetString(Request.Data, "component", ""), &Component, &Error)){ 
      return COLrespondError(Request.Address, Error); 
   }
   if(!SDBScheckPermission(Component, Request.User, Request.Address)) { return; }

   const SDBUparams Params(Component, Request.Data);
   COL_VAR(Params.m_LogFilter.m_RelatedMessage);
   if(SDBUbadBounds(Params.m_LogFilter.m_Start, Params.m_LogFilter.m_End, Params.m_LogFilter.m_Forward)) {
      COL_TRC("Bad bounds for start and end time.");
      return COLrespondError(Request.Address, "Bad bounds for start and end time");
   }
   if(Params.m_Job.is_null()) {
      COL_TRC("No job id specified. Run sync.");
      return SDBSretrieveSync(Params, pLoop, Request.Address);
   }

   COL_TRC("Job id: " << Params.m_Job << ". Run async.");
   SDBSretrieveAsync(Params, pStateMap, pLoop);
   COLrespondSuccess(Request.Address);
}

// /logs/retrieve/results
void SDBSretrieveResults(const COLwebRequest& Request, COLhashmap<COLstring, SDBSretrieveState>* pStateMap) {
   COL_FUNCTION(SDBSretrieveResults);
   const COLstring Job = COLvarRequiredString(Request.Data, "job");
   if(Job.is_null() || Job.isWhitespace() || pStateMap->count(Job) == 0) {
      return COLrespondError(Request.Address, "Job: " + Job + " not found.");
   }
   SDBSretrieveState& State = (*pStateMap)[Job];
   if(!SDBScheckPermission(State.m_Component, Request.User, Request.Address)) { return; }
   COLvar Result;
   Result["data"]        = State.m_Result;
   Result["done"]        = !State.m_JobRunning;
   Result["max_reached"] = State.m_MaxReached;
   Result["end_of_logs"] = State.m_EndOfLogs;
   Result["next_file"]   = State.m_NextFileTime;
   State.m_Result.reset();
   State.m_Result.setArrayType();
   COL_VAR3(Request.User, Result["data"].size(), Result["done"].asBool());
   COLrespondSuccess(Request.Address, Result);
}
