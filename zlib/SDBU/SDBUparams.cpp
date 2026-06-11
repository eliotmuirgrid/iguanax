//  ---------------------------------------------------------------------------
//  Copyright (C) 1997-2024 iNTERFACEWARE Inc.  All Rights Reserved
//
//  Module: SDBUparams.cpp
//
//  Description: Implementation
//
//  Author: Matthew Sobkowski
//  Date:   2025-09-04, 2:58p.m.
//  ---------------------------------------------------------------------------
#include <COL/COLlog.h>
#include <COL/COLvarUtils.h>
#include <SDBU/SDBUparams.h>
COL_LOG_MODULE;

static SDBmessageId SDBUcreateRelatedMessage(const COLvar& Data) {
   COL_FUNCTION(SDBUcreateRelatedMessage);
   const COLstring RelatedId           = COLvarGetString(Data, "rel_comp", "");
   const COLuint64 RelatedStartTime    = atoll(COLvarGetString(Data, "rel_time", "0").c_str());
   const COLuint64 RelatedMessageIndex = atoll(COLvarGetString(Data, "rel_id", "0").c_str());
   COL_VAR3(RelatedId, RelatedStartTime, RelatedMessageIndex);
   return SDBmessageId(RelatedId, RelatedStartTime, RelatedMessageIndex);
}

static void SDBUcheckTimeAndId(SDBfilterInfo2& Filter) {
   COL_FUNCTION(SDBUcheckTimeAndId);
   if(!Filter.m_Forward) {
      if(Filter.m_Start.DateTime == 0)  { Filter.m_Start.DateTime = COL_UINT_64_MAX; }
      if(Filter.m_Start.Index == 0)     { Filter.m_Start.Index = COL_UINT_64_MAX; }
   } else {
      if(Filter.m_End.DateTime == 0)  { Filter.m_End.DateTime = COL_UINT_64_MAX; }
      if(Filter.m_End.Index == 0)     { Filter.m_End.Index = COL_UINT_64_MAX; }
   }
}

SDBUparams::SDBUparams(const COLstring& Component, const COLvar& RequestData)
    : m_Limit(COLvarGet(RequestData, "limit", 20)),
      m_Component(Component),
      m_Search(COLvarGetString(RequestData, "search", "")),
      m_TagSearch(COLvarGetString(RequestData, "tags", "")),
      m_IncludeNonMatches(COLvarGetBool(RequestData, "include_non_matches", false)),
      m_Formatted(COLvarGetBool(RequestData, "formatted", false)),
      m_Minify(COLvarGetBool(RequestData, "minify", false)),
      m_Relevant(COLvarGetBool(RequestData, "relevant", false)),
      m_Job(COLvarGetString(RequestData, "job", "")),
      m_Timeout (0) {
   bool Critical = COLvarGetBool(RequestData, "critical", true);
   if (!Critical) { m_Timeout = time(nullptr) + 5; }
   m_LogFilter.m_Exact          = COLvarGetBool(RequestData, "exact", false);
   m_LogFilter.m_Forward        = COLvarGetBool(RequestData, "forward", false);
   m_LogFilter.m_Exclusive      = COLvarGetBool(RequestData, "exclusive", false);
   m_LogFilter.m_OnlyMessages   = COLvarGetBool(RequestData, "only_messages", false);
   const COLuint64 StartTime    = atoll(COLvarGetString(RequestData, "start", "0").c_str());
   const COLuint64 EndTime      = atoll(COLvarGetString(RequestData, "end", "0").c_str());
   m_LogFilter.m_Start          = SDBmessageId(Component, StartTime, atoll(COLvarGetString(RequestData, "start_id", "0").c_str()));
   m_LogFilter.m_End            = SDBmessageId(Component, EndTime, atoll(COLvarGetString(RequestData, "end_id", "0").c_str()));
   m_LogFilter.m_RelatedMessage = SDBUcreateRelatedMessage(RequestData);
   SDBUcheckTimeAndId(m_LogFilter);
   SDBfilterAllMessages(&m_LogFilter);
}

// the component card periodically requests the last 3 error messages - {"component": "ComWatch_eIXZ3JIGA5MAiz","tags": "#error","exact": true,"limit": 1}
// the log browser periodically requests the last error message       - {"component": "ComWatch_eIXZ3JIGA5MAiz","tags": "#error","exact": true,"limit": 3,"formatted": true,"relvant": true}
// limit the time spent for these requests in case log has no error messages at all
bool SDBSnonCriticalJobTimedOut(const SDBUparams& Params) {
   COL_FUNCTION(SDBSnonCriticalJobTimedOut);
   if (0 == Params.m_Timeout) { return false; }
   time_t Now = time(nullptr);
   if (Now < Params.m_Timeout) { return false; }
   return true;
}